.386
stack  segment use16 stack   ;主程序的堆栈段
       db 200 dup(0)
stack ends
code segment use16
     assume cs:code,ds:code,ss:stack
;新的INT 08H使用的变量 
     count   db 18                  ;“滴答计数”
     hour    db ?,?,':'             ;时的ASCLII码
     min     db ?,?,':'             ;分的ASCII码
     sec     db ?,?                 ;秒的ASCII码
     buf_len=$-hour                 ;计算显示信息长度
     cursor  dw ?                   ;原光标位置
     old_int dw ?,?                 ;原INT 08H的中断矢量
     str0    db 'main process finished',0AH,0
     str1    db 'fail:program exists',0AH,0
     installed dw 1
;新的INT 08H的代码
NEW08H proc far
       pushf
       call dword ptr cs:old_int    ;完成原功能（变量在汇编后使用的默认段寄存器）
                                    ;为DS，故必须加段前缀CS：
       dec  cs:count                ;（倒）计数
       jz disp                      ;计满18次，转时钟显示
       iret                         ;未计满，中断返回
disp:  mov cs:count ,18             ;重置计数初值
       sti                          ;开中断
       pusha                        ;保护现场
       push ds
       push es
       mov ax,cs                    ;将DS，ES指向CS
       mov ds,ax
       mov es,ax
       call get_time                ;获取当前时间，并转换成ASCII码
       mov bh,0
       mov ah,3
       int 10H
       mov cursor,dx                ;保存原光标位置
       mov bp,offset hour           ;ES:[BP]指向显示信息的起始地址
       mov bh,0                     ;显示到0号界面
       mov dh,0                     ;显示在第0行
       mov dl,80-buf_len            ;显示在最后几列（光标位置设到右上角）
       mov bl,07H                   ;显示字符的属性（白色）
       mov cx,buf_len               ;显示的字符串长度
       mov al,0                     ;BL包含显示属性，写后光标不动
       mov ah,13H                   ;调用显示字符串的功能
       int 10H                      ;在右上角显示出当前时间
       mov bh,0                     ;对0号页面操作
       mov dx,cursor                ;恢复原来的光标位置
       mov ah,2                     ;设置光标位置的功能号
       int 10H                      ;还原光标位置（保证主程序的光标位置不受影响）
       
       pop es
       pop ds
       popa                         ;恢复现场
       iret                         ;中断返回
NEW08H endp

get_time proc
;取时间子程序。从RT/CMOS RAM中取得时分秒并转化成ASCII码存放到对应变量中
         mov al,4                   ;4是“时”信息的偏移地址
         out 70H,al                 ;设定将要访问的单元是偏移值为4的“时”的信息
         jmp $+2                    ;延时，保证端口操作的可靠性
         in al,71H                  ;读取“时”信息
         mov ah,al                  ;将2位压缩的BCD码转换成未压缩的BCD码
         and al,0fH
         shr ah,4
         add ax,3030H               ;转换成对应的ASCII码
         xchg ah,al
         mov word ptr hour,ax       ;保存的到hour变量指示的前2个字节中

         mov al,2                   ;2时“分”信息的偏移地址
         out 70H,al
         jmp $+2
         in al,71H                  ;读取“分”信息
         mov ah,al                  ;转换成对应的ASCII码
         and al,0fH
         shr ah,4
         add ax,3030H
         xchg ah,al
         mov word ptr min,ax        ;保存到min变量指示的前2个字节中

         mov al,0
         out 70H,al
         jmp $+2
         in al,71H                  ;读取“秒”信息
         mov ah,al
         and al,0FH                 ;转换成对应的ASCII码
         shr ah,4
         add ax,3030H
         xchg ah,al
         mov word ptr sec,ax        ;保存到SEC变量指示的2个字节中
         ret
get_time endp


begin: 
        

        push cs
        pop ds
        mov ax,3508H                 ;获取原08H的中断矢量
        int 21H 
        
        mov di,bx
        sub di,2
        cmp word ptr [di],1
        jz  fail

        mov old_int,bx               ;保存中断矢量
        mov old_int+2,es
        mov dx,offset NEW08H
        mov ax,2508H                 ;设置新的08H中断矢量
        int 21H
        jmp done
fail:
        mov si,offset str1
        jmp print
done:
        mov si,offset STR0
print:
        mov dl,[si]
        cmp dl,0
        jz  exit
        mov ah,02H
        int 21h
        inc si
        jmp print

exit:
        mov ah,31h
        mov dx,offset begin+15
        mov cl,4
        shr dh,cl
        add dh,10H
        mov al,0
        int 21h


code ends
end begin

