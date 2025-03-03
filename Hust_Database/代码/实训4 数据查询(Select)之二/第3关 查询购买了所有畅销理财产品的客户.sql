-- 3) 查询购买了所有畅销理财产品的客户
--   请用一条SQL语句实现该查询：

select p1.pro_c_id
from property as p1
where not exists(
   select p2.pro_pif_id from property as p2
   where p2.pro_type=1 and p2.pro_pif_id not in
   (select p3.pro_pif_id from property as p3 
   where p3.pro_c_id=p1.pro_c_id and p3.pro_type=1) 
   and p2.pro_pif_id in
   (select p3.pro_pif_id from property as p3
   where p3.pro_type=1
   group by p3.pro_pif_id
   having count(distinct p3.pro_c_id)>2))
group by p1.pro_c_id
order by p1.pro_c_id asc;

/*  end  of  your code  */