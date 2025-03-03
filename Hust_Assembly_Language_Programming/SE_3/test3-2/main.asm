.686P 
.model flat,c
 ExitProcess proto  stdcall :dword
 main_menu_print proto c
 login proto c
 add_data proto c:dword
 display proto:dword
 printf      proto c :ptr sbyte, :vararg
 scanf		 proto c :ptr sbyte, :vararg
 strcmp      proto c :ptr sbyte, :vararg

 includelib  kernel32.lib
 includelib  libcmt.lib
 includelib  legacy_stdio_definitions.lib
 
 ;被其他文件调用的公共量一定要设为public，否则会报错
 public str6,str7,str8,str10,buf,midf

 ;数据结构
 SAMPLES  STRUCT
  SAMID DB 9 DUP(0)  ;每组数据的流水号
  SDA   SDWORD  256809   ;状态信息a
  SDB   SDWORD  -1023    ;状态信息b
  SDC   SDWORD   1265    ;状态信息c
  SF    SDWORD   0       ;处理结果f
 SAMPLES  ENDS
 N  = 5


.data
 buf   SAMPLES <'20220301',100,200,300,0>
       SAMPLES <'20220302',-100,-200,-300,0>
       SAMPLES <'20220303',1,2,3,0>
       SAMPLES N-3 DUP(<>)
 lowf SAMPLES N DUP(<>)
 midf SAMPLES N DUP(<>)
 highf SAMPLES N DUP(<>)
;用户名和密码
bname db '1',0
iname db 20 dup(0)
bpass db '1',0
ipass db 20 dup(0)

;输出用到的字符串
str1 db '用户名: ',0
str2 db '密码: ',0
str3 db '用户名不存在',0ah,0
str4 db '密码错误',0ah,0
str5 db '登录成功',0ah,0
str6 db '操作成功',0ah,0
str7 db '请选择要执行的操作：',0
str8 db 'SAMID：%s  SDA：%d  SDB：%d  SDC：%d  SF：%d',0ah,0
str9 db '请输入有效字符: ',0
str10 db 'midf的数据信息如下：',0ah,0
count1 dd 0	;记录登录的次数
count2 dd 0
choice dw ?
flag db 3
lpfmt db '%s',0
numtype db '%hd',0

 
.stack 200
.code

;计算f的值
calculate proc
  mov ecx, 0   
  lea ebx, buf
  sub ebx, 25
Loop1:
  inc ecx
  add ebx, 25
  imul eax, [ebx].SAMPLES.SDA, 5
  add eax, [ebx].SAMPLES.SDB
  sub eax, [ebx].SAMPLES.SDC
  add eax, 100
  mov [ebx].SAMPLES.SF, eax
  sar [ebx].SAMPLES.SF, 7   ;优化：用sar取代idiv指令
  cmp ecx, N
  jnz Loop1
  ret
calculate endp

;复制每组数据
copy proc
  lea ebx, midf   ;优化：用lea取代mov指令
  sub ebx, 25
  lea edx, lowf
  sub edx, 25
  lea eax, highf
  sub eax, 25
  lea esi, buf
  sub esi, 25
  mov ecx,0
  mov count2,ecx
Loop2:
  mov ecx, count2   ;记录循环次数
  inc ecx
  mov count2, ecx
  add esi, 25
  .if [esi].SAMPLES.SF>0
      jmp high1
  .elseif [esi].SAMPLES.SF<0
          jmp low1
  .else 
        jmp mid1
  .endif
mid1:      
  ;sf = 100的情况
  add ebx, 25 
  mov edi, ebx
  jmp over
low1:  
  ;sf<100的情况
  add edx, 25
  mov edi, edx
  jmp over
high1:
  ;sf>100的情况
  add eax, 25
  mov edi, eax 
  ;复制buf中的内容
over:
  mov ecx, 25
  rep movsb

  mov ecx, count2
  cmp ecx, N
  jnz Loop2
  ret
copy endp

main proc
  ;登录
  .while(flag > 0)
    call login
    cmp eax,0
    jnz main_menu
    dec flag
  .endw
  jmp exit

  ;菜单界面
main_menu:
  
  call main_menu_print            
  invoke scanf,offset numtype,offset choice
  cmp choice,1
  jz choice1
  cmp choice,2
  jz choice2
  cmp choice,3
  jz exit

choice1:
  invoke add_data, offset buf
  jmp main_menu
choice2:
  call calculate
  call copy
  invoke display, offset midf
  jmp main_menu

exit:
  invoke ExitProcess, 0
main endp
end
