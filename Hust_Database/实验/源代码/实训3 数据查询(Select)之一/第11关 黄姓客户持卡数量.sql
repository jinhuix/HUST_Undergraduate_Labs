-- 11) 给出黄姓用户的编号、名称、办理的银行卡的数量(没有办卡的卡数量计为0),持卡数量命名为number_of_cards,按办理银行卡数量降序输出,持卡数量相同的,依客户编号排序。
-- 请用一条SQL语句实现该查询：

# 用count计算办卡数量，b_number为空时，不计算入内
select c_id,c_name,ifnull(count(b_number),0) as number_of_cards
# 将两个表按客户编号连接
from client left outer join bank_card on(c_id=b_c_id)
# 用户姓黄
where c_name like '黄%'
# 需要按客户id分组
group by c_id
# 排序要求
order by number_of_cards desc,c_id;

/*  end  of  your code  */ 