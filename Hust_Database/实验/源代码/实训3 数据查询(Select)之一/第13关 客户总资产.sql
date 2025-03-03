-- 13) 综合客户表(client)、资产表(property)、理财产品表(finances_product)、
--     保险表(insurance)、基金表(fund)和投资资产表(property)，
--     列出所有客户的编号、名称和总资产，总资产命名为total_property。
--     总资产为储蓄卡余额，投资总额，投资总收益的和，再扣除信用卡透支的金额
--     (信用卡余额即为透支金额)。客户总资产包括被冻结的资产。
--    请用一条SQL语句实现该查询：

# 总资产和上一题计算方法类似，需要加上储蓄卡余额、投资总收益的和
select c_id,c_name,sum(ifnull(pro_quantity,0)*(ifnull(p_amount,0)+ifnull(i_amount,0)+ifnull(f_amount,0))+ifnull(pro_income,0))+ifnull(card,0) as total_property
# 连接几个表
from client left outer join property on (pro_c_id=c_id)
    left outer join finances_product on (pro_pif_id=p_id ) and pro_type=1
    left outer join insurance on (pro_pif_id=i_id ) and pro_type=2
    left outer join fund on (pro_pif_id=f_id ) and pro_type=3
    # 需要讨论是信用卡还是储蓄卡
    left outer join 
        (select b_c_id,sum(if(b_type='储蓄卡',b_balance,0)-if(b_type='信用卡',b_balance,0)) as card 
        from bank_card 
        group by b_c_id) as bank on (b_c_id=c_id)

group by c_id,c_name;

/*  end  of  your code  */ 