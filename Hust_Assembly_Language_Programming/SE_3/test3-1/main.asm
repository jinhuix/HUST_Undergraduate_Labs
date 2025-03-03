.686P 
.model flat,c
 ExitProcess proto  stdcall :dword
 includelib  kernel32.lib
 includelib  ucrt.lib
 display proto:dword
 clock    proto
 QueryPerformanceCounter proto stdcall :dword
 printf      proto c :ptr sbyte, :vararg
 scanf		 proto c :ptr sbyte, :vararg
 strcmp      proto c :ptr sbyte, :vararg
 includelib  libcmt.lib   
 includelib  legacy_stdio_definitions.lib
 
 ;数据结构
 SAMPLES  STRUCT
  SAMID DB 9 DUP(0)  ;每组数据的流水号
  SDA   SDWORD  256809   ;状态信息a
  SDB   SDWORD  -1023    ;状态信息b
  SDC   SDWORD   1265    ;状态信息c
  SF    SDWORD   0       ;处理结果f
 SAMPLES  ENDS
 N  = 5

 ;公共变量
 public str8 

.data
 buf   SAMPLES <'20220301',100,200,300,0>
       SAMPLES <'20220302',-100,-200,-300,0>
       SAMPLES <'20220303',1,2,3,0>
       SAMPLES N-3 DUP(<>)
 lowf SAMPLES N DUP(<>)
 midf SAMPLES N DUP(<>)
 highf SAMPLES N DUP(<>)
;用户名和密码
bname db 'xujinhui',0
iname db 20 dup(0)
bpass db 'U202011675',0
ipass db 20 dup(0)

;输出用到的字符串
str1 db '用户名: ',0
str2 db '密码: ',0
str3 db '用户名不存在',0ah,0
str4 db '密码错误',0ah,0
str5 db '登录成功',0ah,0
str6 db 'R.显示midf  Q.退出程序',0ah,0
str7 db '请选择要执行的操作：',0
str8 db 'SAMID：%s  SDA：%ld  SDB：%ld  SDC：%ld  SF：%ld',0ah,0
str9 db 'midf中的数据如下：',0ah,0
count1 dd 0	;记录登录的次数
count2 dd 0
choice db 0
choice2 db 0
lpfmt db '%s',0

 
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
  mov ecx, 0
  mov count2, ecx
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



;比较用户名和密码的宏
cmp_str macro s1, s2
  invoke strcmp,offset s1,offset s2
  endm

main proc
  ;登录界面
login:
  mov ecx, count1
  inc ecx
  mov count1, ecx
  invoke printf,offset str1
  invoke scanf,offset lpfmt,offset iname
  cmp_str bname, iname
  cmp eax,0
  jnz name_error	;输入的用户名错误
  invoke printf,offset str2
  invoke scanf,offset lpfmt,offset ipass
  cmp_str bpass, ipass
  cmp eax,0
  jz pass_right	;输入的用户名和密码都正确
  invoke printf,offset str4
  jmp login_over
name_error:
  invoke printf,offset str3
login_over:
  mov ecx, count1
  cmp ecx, 3
  jnz login
  jmp exit
pass_right:
  invoke printf,offset str5

  call calculate
  call copy
  invoke display, offset midf

;进入主菜单，选择要进行的操作
  invoke printf,offset str6
main_menu:
  invoke printf,offset str7
  invoke scanf,offset lpfmt,offset choice
  cmp choice,'Q'
  jz exit
  cmp choice,'R'
  call calculate
  call copy
  invoke printf,offset str9
  invoke display, offset midf
  jmp main_menu  


exit:
  invoke ExitProcess, 0
main endp
end 