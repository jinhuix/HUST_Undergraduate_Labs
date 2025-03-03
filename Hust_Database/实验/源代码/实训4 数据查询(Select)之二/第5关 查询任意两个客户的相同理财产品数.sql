-- 5) 查询任意两个客户的相同理财产品数
--   请用一条SQL语句实现该查询：

select distinct p1.pro_c_id as pro_c_id, p2.pro_c_id as pro_c_id,count(distinct p3.pro_pif_id) as total_count
from property p1,property p2,property p3
where p1.pro_c_id!=p2.pro_c_id and p3.pro_type=1 and p3.pro_pif_id in 
    (select pro_pif_id from property where pro_type=1
    and pro_c_id=p2.pro_c_id) and p3.pro_pif_id in
    (select pro_pif_id from property where pro_type=1
    and pro_c_id=p1.pro_c_id)
group by p1.pro_c_id,p2.pro_c_id
having count(distinct p3.pro_pif_id)>1;       

/*  end  of  your code  */