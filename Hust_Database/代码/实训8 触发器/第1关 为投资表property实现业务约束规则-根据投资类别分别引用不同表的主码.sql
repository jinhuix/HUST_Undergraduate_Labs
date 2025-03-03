use finance1;
drop trigger if exists before_property_inserted;
-- 请在适当的地方补充代码，完成任务要求：
delimiter $$
CREATE TRIGGER before_property_inserted BEFORE INSERT ON property
FOR EACH ROW 
BEGIN
declare msg varchar(40);
if new.pro_type <> 1 and new.pro_type <> 2 and new.pro_type <> 3 then
    set msg=concat('type ',new.pro_type,' is illegal!');
    signal sqlstate '45000' set message_text=msg;
end if;

if new.pro_type=1 and not exists(select* from property,finances_product where new.pro_pif_id=finances_product.p_id) then
    set msg=concat('finances product #',new.pro_pif_id,' not found!');
    signal sqlstate '45000' set message_text=msg;
end if;

if new.pro_type=2 and not exists(select* from property,insurance where new.pro_pif_id=insurance.i_id) then
    set msg=concat('insurance #',new.pro_pif_id,' not found!');
    signal sqlstate '45000' set message_text=msg;
end if;

if new.pro_type=3 and not exists(select* from property,fund where new.pro_pif_id=fund.f_id) then
    set msg=concat('fund #',new.pro_pif_id,' not found!');
    signal sqlstate '45000' set message_text=msg;
end if;
END$$
delimiter ;