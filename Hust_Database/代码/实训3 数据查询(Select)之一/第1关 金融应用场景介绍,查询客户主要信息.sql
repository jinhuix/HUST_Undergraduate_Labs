-- 1) 查询所有客户的名称、手机号和邮箱信息。查询结果按照客户编号排序。
--    请用一条SQL语句实现该查询：

select c_name,c_phone,c_mail    # 查询的属性名
from client # 从哪个表查询
order by c_id;  # 结果的排序

/*  end  of  your code  */