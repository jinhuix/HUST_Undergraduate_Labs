-- 编写一存储过程，自动安排某个连续期间的大夜班的值班表:

delimiter $$
create procedure sp_night_shift_arrange(in start_date date, in end_date date)
begin

declare now_date date;
declare not_found INT default FALSE;    #表示是否找到值班的医生
declare weekend_doctor INT default FALSE;   #表示科室主任是否轮到周末的夜班
declare d_name,n_name1,n_name2 char(30);
declare t_name char(30);
declare d_type int default(0);

declare doctor_cursor cursor for select e_name,e_type from employee where e_type <> 3;
declare nurse_cursor cursor for select e_name from employee where e_type = 3;

declare continue handler for NOT FOUND set not_found = TRUE;

open doctor_cursor;
open nurse_cursor;

fetch doctor_cursor into d_name,d_type;
fetch nurse_cursor into n_name1;
fetch nurse_cursor into n_name2;

set now_date=start_date;
while now_date<=end_date do
    # 如果科室主任轮到周末夜班，则安排给下一个医生
    if d_type=1 and (dayofweek(now_date)=1 or dayofweek(now_date)=7) then
        set t_name=d_name;
        set weekend_doctor=TRUE;
        fetch doctor_cursor into d_name,d_type;
        # 如果后面没有医生了，再次轮流安排
        if not_found=TRUE then 
            set not_found=FALSE;
            close doctor_cursor;
            open doctor_cursor;
            fetch doctor_cursor into d_name,d_type;
        end if;
        insert into night_shift_schedule values(now_date,d_name,n_name1,n_name2);
        # 日期向后递推1天
        set now_date=date_add(now_date,interval 1 day);
        # 继续向后取出医生和护士数据
        fetch doctor_cursor into d_name,d_type;
        if not_found = TRUE then
			set not_found = FALSE;
			close doctor_cursor;
			open doctor_cursor;
			fetch doctor_cursor into d_name,d_type;
        end if;
        fetch nurse_cursor into n_name1;
        if not_found = TRUE then 
            set not_found=FALSE;
            close nurse_cursor;
            open nurse_cursor;
            fetch nurse_cursor into n_name1;
        end if;
        fetch nurse_cursor into n_name2;
        if not_found = TRUE then 
            set not_found=FALSE;
            close nurse_cursor;
            open nurse_cursor;
            fetch nurse_cursor into n_name2;
        end if;
    # 如果日期是周一且科室主任之前被安排到了周末，则应安排主任轮班
    elseif dayofweek(now_date)=2 and weekend_doctor=TRUE then 
        set weekend_doctor=FALSE;
        insert into night_shift_schedule values(now_date,t_name,n_name1,n_name2);
        # 递推，日期加一
        set now_date=date_add(now_date,interval 1 day);
        # 继续取出护士数据，不用取出医生数据
        fetch nurse_cursor into n_name1;
        if not_found=TRUE then 
            set not_found=FALSE;
            close nurse_cursor;
            open nurse_cursor;
            fetch nurse_cursor into n_name1;
        end if;
        fetch nurse_cursor into n_name2;
        if not_found=TRUE then 
            set not_found=FALSE;
            close nurse_cursor;
            open nurse_cursor;
            fetch nurse_cursor into n_name2;
        end if;
    else 
        insert into night_shift_schedule values(now_date,d_name,n_name1,n_name2);
        set now_date =date_add(now_date,interval 1 day);
        fetch doctor_cursor into d_name,d_type;
        if not_found=TRUE then 
            set not_found=FALSE;
            close doctor_cursor;
            open doctor_cursor;
            fetch doctor_cursor into d_name,d_type;
        end if;
        fetch nurse_cursor into n_name1;
        if not_found=TRUE then 
            set not_found=FALSE;
            close nurse_cursor;
            open nurse_cursor;
            fetch nurse_cursor into n_name1;
        end if;
        fetch nurse_cursor into n_name2;
        if not_found=TRUE then
            set not_found=FALSE;
            close nurse_cursor;
            open nurse_cursor;
            fetch nurse_cursor into n_name2;
        end if;
    end if;
end while;
close doctor_cursor;
close nurse_cursor;

end$$
delimiter ;

/*  end  of  your code  */ 