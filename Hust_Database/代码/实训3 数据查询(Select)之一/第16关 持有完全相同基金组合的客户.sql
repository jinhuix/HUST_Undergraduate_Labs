-- 16) 查询持有相同基金组合的客户对，如编号为A的客户持有的基金，编号为B的客户也持有，反过来，编号为B的客户持有的基金，编号为A的客户也持有，则(A,B)即为持有相同基金组合的二元组，请列出这样的客户对。为避免过多的重复，如果(1,2)为满足条件的元组，则不必显示(2,1)，即只显示编号小者在前的那一对，这一组客户编号分别命名为c_id1,c_id2。
-- 请用一条SQL语句实现该查询：

select distinct p1.pro_c_id as c_id1, p2.pro_c_id as c_id2
from property p1,property p2
# 只显示编号小者在前的那一对
where p1.pro_c_id<p2.pro_c_id 
    # 不存在A客户有的基金B客户没有
    and not exists
    (select* from property
        where p1.pro_c_id=property.pro_c_id and pro_type=3 and pro_pif_id not in 
            (select pro_pif_id from property where p2.pro_c_id=property.pro_c_id
                and pro_type=3))
    # 也不存在B客户有的基金A客户没有
    and not exists
    (select* from property
        where p2.pro_c_id=property.pro_c_id and pro_type=3 and pro_pif_id not in 
            (select pro_pif_id from property where p1.pro_c_id=property.pro_c_id
                and pro_type=3));

/*  end  of  your code  */