
create table V6Play
(
MyNumber NUMBER,
MyVarchar VARCHAR2(200),
myDate date,
myTimeStamp timestamp,
MyNumber2 NUMBER
);

ALTER TABLE V6Play add MyNumber3 NUMBER;

DELETE FROM V6Play;
SELECT * FROM V6Play;
INSERT INTO V6Play(MYDATE) VALUES(to_date('10-DEC-21 10:34:33','yyyy/mm/dd hh24:mi:ss'));
