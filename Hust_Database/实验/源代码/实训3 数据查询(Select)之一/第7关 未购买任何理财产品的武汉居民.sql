-- 7) 已知身份证前6位表示居民地区，其中4201开头表示湖北省武汉市。查询身份证隶属武汉市没有买过任何理财产品的客户的名称、电话号、邮箱。依客户编号排序
--    请用一条SQL语句实现该查询：

select c_name,c_phone,c_mail
from client
# 属于武汉市
where c_id_card like '4201%' and not exists
# 没有买过任何理财产品
    (select* from property where c_id=pro_c_id and pro_type='1')
# 依客户编号排序
order by c_id;

/*  end  of  your code  */