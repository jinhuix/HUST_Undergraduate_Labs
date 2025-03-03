-- 9) 查询购买了货币型(f_type='货币型')基金的用户的名称、电话号、邮箱。依客户编号排序。
--   请用一条SQL语句实现该查询：

select c_name,c_phone,c_mail
from client
where c_id in
    # 购买了货币型(f_type='货币型')基金
    (select pro_c_id from property,fund
    where pro_type='3' and pro_pif_id=f_id and f_type='货币型')
# 依客户编号排序
order by c_id;

/*  end  of  your code  */