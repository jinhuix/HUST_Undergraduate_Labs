-- 2) Ͷ�ʻ�����ƫ��������Ʒ�Ŀͻ�
--   ����һ��SQL���ʵ�ָò�ѯ��

select pro_c_id
from property left outer join fund on (pro_c_id=f_id) and pro_type=3
    left outer join finances_product on (pro_pif_id=p_id ) and pro_type=1
group by pro_c_id
having count(distinct p_id)>3 and count(distinct p_id)>count(distinct f_id)
order by pro_c_id asc;

/*  end  of  your code  */