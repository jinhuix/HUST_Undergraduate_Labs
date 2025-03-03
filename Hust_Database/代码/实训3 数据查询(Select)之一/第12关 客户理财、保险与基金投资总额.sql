 -- 12) 综合客户表(client)、资产表(property)、理财产品表(finances_product)、保险表(insurance)和
 --     基金表(fund)，列出客户的名称、身份证号以及投资总金额（即投资本金，
 --     每笔投资金额=商品数量*该产品每份金额)，注意投资金额按类型需要查询不同的表，
 --     投资总金额是客户购买的各类资产(理财,保险,基金)投资金额的总和，总金额命名为total_amount。
 --     查询结果按总金额降序排序。
 -- 请用一条SQL语句实现该查询：

# 每笔投资金额=商品数量*该产品每份金额，用ifnull计算不同产品类型
select c_name,c_id_card,sum(ifnull(pro_quantity,0)*(ifnull(p_amount,0)+ifnull(i_amount,0)+ifnull(f_amount,0))) as total_amount
# 连接这些表
from client left outer join property on (pro_c_id=c_id)
    left outer join finances_product on (pro_pif_id=p_id ) and pro_type=1
    left outer join insurance on (pro_pif_id=i_id ) and pro_type=2
    left outer join fund on (pro_pif_id=f_id ) and pro_type=3
# 按c_name,c_id_card分组
group by c_name,c_id_card
order by total_amount desc;
/*  end  of  your code  */ 