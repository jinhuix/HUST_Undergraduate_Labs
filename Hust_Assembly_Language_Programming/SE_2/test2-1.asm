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
 N  = 10000

.data
 buf   SAMPLES <'202203001',100,200,300,0>
       SAMPLES <'202203002',-100,-200,-300,0>
       SAMPLES <'202203003',1,2,3,0>
       SAMPLES N-3 DUP(<>)
 start_time dd  0    ; 开始时间
 end_time   dd  0    ; 结束时间
 used_time  dd  0    ; 使用时间
 ;pFmt2 db "SAMID：%s  SDA：%ld  SDB：%ld  SDC：%ld  SF：%ld"  ,0AH,0
 pFmt1 db  "程序优化后的运行时间为： %d",0ah,0
 lowf SAMPLES N DUP(<>)
 midf SAMPLES N DUP(<>)
 highf SAMPLES N DUP(<>)
 
.stack 200
.code
main proc  
  ;开始计时
  invoke QueryPerformanceCounter, offset start_time
  mov ecx, 0   ;buf计数
  lea ebp, midf   ;优化：用lea取代mov指令
  sub ebp, 25
  lea edi, lowf
  sub edi, 25
  lea esi, highf
  sub esi, 25
  lea ebx, buf
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
  mov [ebx].SAMPLES.SF, eax
  sar [ebx].SAMPLES.SF, 7   ;优化：用sar取代idiv指令
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
  add ebp, 25   ;优化：减少重复代码
  mov edx, ebp
  jmp over
low1:  
  ;sf<100的情况
  add edi, 25
  mov edx, edi
  jmp over
high1:
  ;sf>100的情况
  add esi, 25
  mov edx, esi
over:
  mov eax, dword ptr [ebx].SAMPLES.SAMID
  mov dword ptr [edx].SAMPLES.SAMID, eax
  mov eax, [ebx].SAMPLES.SDA
  mov [edx].SAMPLES.SDA, eax
  mov eax, [ebx].SAMPLES.SDB
  mov [edx].SAMPLES.SDB, eax
  mov eax, [ebx].SAMPLES.SDC
  mov [edx].SAMPLES.SDC, eax
  mov eax, [ebx].SAMPLES.SF
  mov [edx].SAMPLES.SF, eax 
  cmp ecx, N
  jnz L
  ;结束计时
  invoke QueryPerformanceCounter, offset end_time

  mov    eax, end_time
  sub    eax,  start_time
  invoke printf, offset pFmt1, eax
  invoke ExitProcess, 0
main endp
end 