# 请在以下适当的空白处填写SQL语句，完成任务书的要求。空白行可通过回车换行添加。 

create database TestDb; # 创建数据库
use TestDb; # 指定这个数据库为当前工作数据库

create table t_emp( # 创建表
id int primary key, # 为表建主码，但不用考虑主码约束的命名
name varchar(32),
deptId int,
salary float);

/* *********** 结束 ************* */