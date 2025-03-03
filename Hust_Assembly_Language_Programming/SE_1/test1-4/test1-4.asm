.686P 
.model flat,c
 ExitProcess proto  stdcall :dword
 includelib  kernel32.lib
 includelib  ucrt.lib
 clock    proto
 QueryPerformanceCounter proto stdcall :dword
 ;printf      proto c :vararg
 printf      proto c :ptr byte, :vararg
 includelib  libcmt.lib   
 includelib  legacy_stdio_definitions.lib
 
 SAMPLES  STRUCT
  SAMID DB 9 DUP(0)  ;每组数据的流水号
  SDA   SDWORD  256809   ;状态信息a
  SDB   SDWORD  -1023    ;状态信息b
  SDC   SDWORD   1265    ;状态信息c
  SF    SDWORD   0       ;处理结果f
 SAMPLES  ENDS
 N  = 200

.data
 buf   SAMPLES <'202203001',100,200,300,0>
       SAMPLES <'202203002',-100,-200,-300,0>
       SAMPLES <'202203003',1,2,3,0>
       SAMPLES N-3 DUP(<>)
 start_time dd  0    ; 开始时间
 end_time   dd  0    ; 结束时间
 used_time  dd  0    ; 使用时间
 lowc dd 0
 midc dd 0
 highc dd 0
 divisor sword 128
 ;pFmt2 db "SAMID：%s  SDA：%ld  SDB：%ld  SDC：%ld  SF：%ld"  ,0AH,0
 pFmt1 db  "%d",0dh,0ah,0
 lowf SAMPLES N DUP(<>)
 midf SAMPLES N DUP(<>)
 highf SAMPLES N DUP(<>)
 
.stack 200
.code
main proc  
  ;开始计时
  ;invoke  clock
  ;mov    start_time, eax
  invoke QueryPerformanceCounter, offset start_time
  mov ecx, 0   ;buf计数
  mov ebp, offset midf   ;midf的首地址
  sub ebp, 25
  mov edi, offset lowf
  sub edi, 25
  mov esi, offset highf
  sub esi, 25
  mov ebx, offset buf
  sub ebx, 25
  ;循环计算各组SF并保存结果
L:
  inc ecx
  ;计算SF
  add ebx, 25
  imul eax, [ebx].SAMPLES.SDA, 5
  add eax, [ebx].SAMPLES.SDB
  sub eax, [ebx].SAMPLES.SDC
  add eax, 100
  mov dx, 0
  idiv divisor   
  mov [ebx].SAMPLES.SF, eax
  ;按处理结果存储数据
  .if [ebx].SAMPLES.SF>0
      jmp high1
  .elseif [ebx].SAMPLES.SF<0
          jmp low1
  .else 
        jmp mid1
  .endif
mid1:      
  ;sf = 100的情况
  mov eax, midc
  inc eax
  mov midc, eax
  imul eax, 25
  add ebp, eax
  mov eax, dword ptr [ebx].SAMPLES.SAMID
  mov dword ptr [ebp].SAMPLES.SAMID, eax
  mov eax, [ebx].SAMPLES.SDA
  mov [ebp].SAMPLES.SDA, eax
  mov eax, [ebx].SAMPLES.SDB
  mov [ebp].SAMPLES.SDB, eax
  mov eax, [ebx].SAMPLES.SDC
  mov [ebp].SAMPLES.SDC, eax
  mov eax, [ebx].SAMPLES.SF
  mov [ebp].SAMPLES.SF, eax
  jmp over
low1:  
  ;sf<100的情况
  mov eax, lowc
  inc eax
  mov lowc, eax
  imul eax, 25
  add edi, eax
  mov eax, dword ptr [ebx].SAMPLES.SAMID
  mov dword ptr [edi].SAMPLES.SAMID, eax
  mov eax, [ebx].SAMPLES.SDA
  mov [edi].SAMPLES.SDA, eax
  mov eax, [ebx].SAMPLES.SDB
  mov [edi].SAMPLES.SDB, eax
  mov eax, [ebx].SAMPLES.SDC
  mov [edi].SAMPLES.SDC, eax
  mov eax, [ebx].SAMPLES.SF
  mov [edi].SAMPLES.SF, eax
  jmp over
high1:
  ;sf>100的情况
  mov eax, highc
  inc eax
  mov highc, eax
  imul eax, 25
  add esi, eax
  mov eax, dword ptr[ebx].SAMPLES.SAMID
  mov dword ptr [esi].SAMPLES.SAMID, eax
  mov eax, [ebx].SAMPLES.SDA
  mov [esi].SAMPLES.SDA, eax
  mov eax, [ebx].SAMPLES.SDB
  mov [esi].SAMPLES.SDB, eax
  mov eax, [ebx].SAMPLES.SDC
  mov [esi].SAMPLES.SDC, eax
  mov eax, [ebx].SAMPLES.SF
  mov [esi].SAMPLES.SF, eax
over:
  cmp ecx, N
  jnz L
  ;结束计时

  ;invoke clock
  invoke QueryPerformanceCounter, offset end_time
  mov    eax, end_time
  sub    eax,  start_time
  invoke printf, offset pFmt1, eax
 
  invoke ExitProcess, 0
main endp
end 