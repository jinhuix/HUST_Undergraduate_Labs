-- 4) 	�������Ƶ���Ʋ�Ʒ
--   ����һ��SQL���ʵ�ָò�ѯ��

select pro_pif_id,count(pro_c_id) as cc,dense_rank() over(order by count(pro_c_id) desc) as prank
from property
where pro_pif_id!= 14 and pro_type=1
    and pro_pif_id in(
        # ���ҳ�����������ǰ3���û����������Ʋ�Ʒ
        select pro_pif_id from property
        # ���ҳ�����������ǰ3���û�
        where pro_c_id in(
            select pro_c_id from(
                select pro_c_id,dense_rank() over(order by count(pro_pif_id) desc) as rk from property
                where pro_type=1 and pro_pif_id= 14
            )
            where rk<=3
        )
    )
group by pro_pif_id
order by prank asc,pro_pif_id asc
having prank<=3;

/*  end  of  your code  */