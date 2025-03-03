-- 6) 众数是一组数据中出现次数最多的数值，有时众数在一组数中会有好几个。查询资产表中所有资产记录里商品收益的众数和它出现的次数，出现的次数命名为presence。
--    请用一条SQL语句实现该查询：

select pro_income,count(pro_income) as presence
from property
group by pro_income # 按商品收益分组
# 某商品收益出现的次数大于其他所有商品的
having count(pro_income) >= all (select count(pro_income) from property group by pro_income);

/*  end  of  your code  */