
#include <iostream>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <sys/resource.h>

/* ================ Start of Metrics measurement code ==================*/

/*
Currently it is supported for linux, for others os it is not supported.
In order for easy copying to measure metrics, all class definitions are handled in single file 
*/

///@brief Computation of elapsed time of program
class ElapsedTime
{
private:
    ///@brief Message for elapsed time
    std::string m_startMsg;

public:

    ///@brief time point which holds the start and end time
    std::chrono::time_point<std::chrono::steady_clock> begin, end;

    ///@brief elapsed time
    double m_elapsedTime;

    ///@brief    start capturing elapsed time
    ///@return   None
    void start()
    {
        m_elapsedTime = 0;
        begin = std::chrono::steady_clock::now();
    }

    ///@brief    start capturing elapsed time and accumulated with previous metrics
    ///@param ArgName : ArgDesc
    ///@return   None
    void startAccumulate(const std::string &s)
    {
        m_startMsg = s;
        begin = std::chrono::steady_clock::now();
    }

    ///@brief    start capturing elapsed time
    ///@param string : message to display at end
    ///@return   None
    void start(const std::string &s)
    {
        m_startMsg = s;
        start();
    }

    ///@brief    constructor
    ElapsedTime()
    {
        start();
        m_elapsedTime = 0;
    }

    ///@brief    print the elapsed time
    ///@return   None
    void printTime()
    {
        std::cout << std::fixed << "Elapsed time[" << m_startMsg << "](seconds): " << m_elapsedTime << '\n';
    }

    ///@brief    stop capturing elapsed time and print values
    ///@return   None
    void stop()
    {
        end = std::chrono::steady_clock::now();
        std::chrono::duration<double> timeTaken = end - begin;

        m_elapsedTime += timeTaken.count();
        

        std::cout<< std::fixed << "Elapsed time[" << m_startMsg << "](seconds): " << timeTaken.count() << '\n';
    }

    ///@brief    stopAccumulate the elapsed time and print values
    ///@return   None
    void stopAccumulate()
    {
        end = std::chrono::steady_clock::now();
        std::chrono::duration<double> timeTaken = end - begin;
        m_elapsedTime += timeTaken.count();

    }

    ///@brief    destructor
    ~ElapsedTime()
    {
    }
};

class CppMemoryMetrics
{
public:
    //Returns the peak memory usage so far in bytes
    size_t getPeakMemoryUsageInBytes()
    {
        struct rusage rusage;
        getrusage(RUSAGE_SELF, &rusage);
        // rusage.ru_maxrss returns in KB, so coverting to bytes
        return (size_t)(rusage.ru_maxrss * 1024L);
    }

    void printMemoryUsageInKb()
    {
        //V6P - change to macro
        std::cout << "Memory usage(KB): " << getPeakMemoryUsageInBytes() / 1024 << std::endl;
    }
};

///@brief Calculation of CPU related metrics of the program
class CppCpuMetrics
{
private:
    ///@brief resource usage 
    struct rusage cpuResUsage;

    ///@brief start and end time of cpu
    double  m_endCpuTime;

    ///@brief total cpu time
    long double  m_totalCpuTime;

    ///@brief message to display
    std::string m_descriptionStr;

    const int numberOfMicroSecInSec=1000000;

public:
    ///@brief    constructor
    ///@return   None
    CppCpuMetrics()
    {
        m_endCpuTime = 0;
        m_totalCpuTime = 0;
    }

    ///@brief    Returns user cpu time from beginning of program to this point
    ///@return   double
    double getUserCpuTimeInSeconds()
    {
        getrusage(RUSAGE_SELF, &cpuResUsage);
        //tv_usec is microseconds, so dividing by 1000000
        return (cpuResUsage.ru_utime.tv_sec + (cpuResUsage.ru_utime.tv_usec / static_cast<double>(numberOfMicroSecInSec)));
    }

    ///@brief    print the user cpu time
    ///@return   None
    void printUserCpuTimeInSeconds()
    {
        std::cout << "User CPU time(Seconds): " << getUserCpuTimeInSeconds() << '\n';
    }

    ///@brief    Returns system cpu time from beginning of program to this point
    ///@return   double
    double getSystemCpuTimeInSeconds()
    {
        getrusage(RUSAGE_SELF, &cpuResUsage);
        //tv_usec is microseconds, so dividing by 1000000
        return (cpuResUsage.ru_stime.tv_sec + (cpuResUsage.ru_stime.tv_usec / static_cast<double>(numberOfMicroSecInSec)));
    }

    ///@brief    printSystemCpuTimeInSeconds
    ///@return   None
    void printSystemCpuTimeInSeconds()
    {
        std::cout << "System CPU time(Seconds): " << getSystemCpuTimeInSeconds() << '\n';
    }

    ///@brief    Starts the cpu time when this function is called
    ///@return   None
    void start()
    {
    }

    ///@brief    Starts the cpu time when this function is called
    ///@param descriptionStr : descriptionStr
    ///@return   None
    void start(const std::string &descriptionStr)
    {
        m_descriptionStr = descriptionStr;
    }

    ///@brief    stops the cpu time calculation when this function is called
    ///@return   None
    void stop()
    {
        m_endCpuTime = getUserCpuTimeInSeconds() + getSystemCpuTimeInSeconds();
        std::cout << "CPU Time [ " << m_descriptionStr << " ] : " << m_endCpuTime << " seconds" << '\n';
    }
};

class CppProgramMetrics
{
private:
    ElapsedTime elapsedObj;
    CppCpuMetrics cpuMetrics;
    CppMemoryMetrics memMetrics;

public:
    CppProgramMetrics()
    {
    }

    void start(std::string metricsName)
    {
        elapsedObj.start(metricsName);
    }

    void stop()
    {
        elapsedObj.stop();
        cpuMetrics.printUserCpuTimeInSeconds();
        cpuMetrics.printSystemCpuTimeInSeconds();
        memMetrics.printMemoryUsageInKb();
    }
};

/* ================ End of Metrics measurement code ==================*/


// Driver function
int main()
{
     CppCpuMetrics cpuMetrics;
    cpuMetrics.start("DEMO");

    CppProgramMetrics mainPgmMetrics;
    mainPgmMetrics.start("Main Elapsed Time");

    {
        // Main code goes here
    }

    mainPgmMetrics.stop();


    ElapsedTime elapTime;
    elapTime.start("Demo");
   std::this_thread::sleep_for(std::chrono::microseconds(1000));
    elapTime.stop();

    ElapsedTime elapTimeAcc;
    elapTimeAcc.startAccumulate("Demo Acc");
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
    elapTimeAcc.stopAccumulate();

    elapTimeAcc.startAccumulate("Demo Acc");
    std::this_thread::sleep_for(std::chrono::microseconds(1000));
    elapTimeAcc.stopAccumulate();

    elapTimeAcc.printTime();


   
    int sum=0;
        for(int i=0;i<10000;i++)
        sum=sum+i;

    std::cout << sum << '\n';
            
    cpuMetrics.stop();
    
}
