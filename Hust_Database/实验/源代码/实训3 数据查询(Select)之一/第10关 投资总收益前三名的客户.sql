-- 10) 查询当前总的可用资产收益(被冻结的资产除外)前三名的客户的名称、身份证号及其总收益，按收益降序输出，总收益命名为total_income。不考虑并列排名情形。
--    请用一条SQL语句实现该查询：

# 总收益用sum计算
select c_name,c_id_card,sum(pro_income) as total_income
from client,property
# 只统计未被冻结的资产
where c_id=pro_c_id and pro_status='可用'
# 按客户编号分组
group by c_id
# 按收益降序输出
order by total_income desc
# 前三名
limit 3

/*  end  of  your code  */ 