-- 19) 以日历表格式列出2022年2月每周每日基金购买总金额，输出格式如下：
-- week_of_trading Monday Tuesday Wednesday Thursday Friday
--               1
--               2    
--               3
--               4
--   请用一条SQL语句实现该查询：

select week(pro_purchase_time)-5 as week_of_trading,sum(if((weekday(pro_purchase_time))=0,pro_quantity*f_amount,null)) as Monday,sum(if((weekday(pro_purchase_time))=1,pro_quantity*f_amount,null)) as Tuesday,sum(if((weekday(pro_purchase_time))=2,pro_quantity*f_amount,null)) as Wednesday,sum(if((weekday(pro_purchase_time))=3,pro_quantity*f_amount,null)) as Thursday,sum(if((weekday(pro_purchase_time))=4,pro_quantity*f_amount,null)) as Friday
from property,fund
# 购买日期为2月，类型为基金
where pro_type=3 and pro_pif_id=f_id and pro_purchase_time >= '2022-02-07' and pro_purchase_time <= '2022-02-28'
group by week_of_trading
order by week_of_trading;

/*  end  of  your code  */