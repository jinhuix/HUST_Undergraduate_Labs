-- 6) 查找相似的理财客户
--   请用一条SQL语句实现该查询：

select* from(
select distinct p1.pro_c_id as pac, p2.pro_c_id as pbc,count(distinct p3.pro_pif_id) as common,rank() over(partition by p1.pro_c_id order by count(distinct p3.pro_pif_id) desc,p2.pro_c_id asc) as crank
from property p1,property p2,property p3
where p1.pro_c_id!=p2.pro_c_id and p3.pro_type=1 and p3.pro_pif_id in 
    (select pro_pif_id from property where pro_type=1
    and pro_c_id=p2.pro_c_id) and p3.pro_pif_id in
    (select pro_pif_id from property where pro_type=1
    and pro_c_id=p1.pro_c_id)
group by p1.pro_c_id,p2.pro_c_id
having count(distinct p3.pro_pif_id)>0
order by p1.pro_c_id asc,common desc,p2.pro_c_id asc
) as aa
where crank<3;

/*  end  of  your code  */