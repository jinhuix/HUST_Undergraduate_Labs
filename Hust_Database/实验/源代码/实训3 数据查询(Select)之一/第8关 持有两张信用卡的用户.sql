-- 8) 查询持有两张(含）以上信用卡的用户的名称、身份证号、手机号。查询结果依客户编号排序。
--    请用一条SQL语句实现该查询：

select c_name,c_id_card,c_phone
from client,bank_card
where b_c_id=c_id and b_type='信用卡'   # 卡为信用卡
group by b_c_id # 按客户id分组
having count(c_id_card)>1  # 持有两张(含）以上
order by c_id;  # 依客户编号排序

/*  end  of  your code  */