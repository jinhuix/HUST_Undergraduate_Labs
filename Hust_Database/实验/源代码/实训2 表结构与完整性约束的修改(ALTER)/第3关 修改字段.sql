use MyDb;

#请在以下空白处添加适当的SQL语句，实现编程要求
# 将QQ号的数据类型改为char(12)
# 将列名weixin改为wechat
alter table addressBook modify QQ char(12),      
       rename column weixin to wechat;