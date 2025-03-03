-- 15) 查询资产表中客户编号，客户基金投资总收益,基金投资总收益的排名(从高到低排名)。
--     总收益相同时名次亦相同(即并列名次)。总收益命名为total_revenue, 名次命名为rank。
--     第一条SQL语句实现全局名次不连续的排名，
--     第二条SQL语句实现全局名次连续的排名。

-- (1) 基金总收益排名(名次不连续)
# 名次不连续用rank
select pro_c_id,total_revenue,rank() over(order by total_revenue desc) as 'rank' 
# 先查询出客户基金投资总收益
from (select pro_c_id,sum(pro_income) as total_revenue
    from property where pro_type=3 group by pro_c_id) as pa
order by total_revenue desc,pro_c_id asc;

-- (2) 基金总收益排名(名次连续)
# 名次连续用dense_rank()
select pro_c_id,total_revenue,dense_rank() over(order by total_revenue desc) as 'rank' 
# 先查询出客户基金投资总收益
from (select pro_c_id,sum(pro_income) as total_revenue
    from property where pro_type=3 group by pro_c_id) as pa
order by total_revenue desc,pro_c_id asc;

/*  end  of  your code  */