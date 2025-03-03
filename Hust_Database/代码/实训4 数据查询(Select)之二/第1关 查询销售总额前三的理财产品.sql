-- 1) ��ѯ�����ܶ�ǰ������Ʋ�Ʒ
--   ����һ��SQL���ʵ�ָò�ѯ��

(select pyear,rank() over (order by sumamount desc) as rk,p_id,sumamount
from (select year(pro_purchase_time) as pyear,p_id,sum(pro_quantity*p_amount) as sumamount
    from property, finances_product
    where pro_pif_id=p_id and pro_type=1 and pro_purchase_time like '2010%'
    group by p_id,pyear
)as pfinance
order by rk asc,p_id asc
limit 3)
union
(select pyear,rank() over (order by sumamount desc) as rk,p_id,sumamount
from (select year(pro_purchase_time) as pyear,p_id,sum(pro_quantity*p_amount) as sumamount
    from property, finances_product
    where pro_pif_id=p_id and pro_type=1 and pro_purchase_time like '2011%'
    group by p_id,pyear
)as pfinance
order by pyear asc,rk asc,p_id asc
limit 3);

/*  end  of  your code  */