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
  SAMID DB 9 DUP(0)  ;ÿ�����ݵ���ˮ��
  SDA   SDWORD  256809   ;״̬��Ϣa
  SDB   SDWORD  -1023    ;״̬��Ϣb
  SDC   SDWORD   1265    ;״̬��Ϣc
  SF    SDWORD   0       ;������f
 SAMPLES  ENDS
 N  = 10000

.data
 buf   SAMPLES <'202203001',100,200,300,0>
       SAMPLES <'202203002',-100,-200,-300,0>
       SAMPLES <'202203003',1,2,3,0>
       SAMPLES N-3 DUP(<>)
 start_time dd  0    ; ��ʼʱ��
 end_time   dd  0    ; ����ʱ��
 used_time  dd  0    ; ʹ��ʱ��
 ;pFmt2 db "SAMID��%s  SDA��%ld  SDB��%ld  SDC��%ld  SF��%ld"  ,0AH,0
 pFmt1 db  "�����Ż��������ʱ��Ϊ�� %d",0ah,0
 lowf SAMPLES N DUP(<>)
 midf SAMPLES N DUP(<>)
 highf SAMPLES N DUP(<>)
 
.stack 200
.code
main proc  
  ;��ʼ��ʱ
  invoke QueryPerformanceCounter, offset start_time
  mov ecx, 0   ;buf����
  lea ebp, midf   ;�Ż�����leaȡ��movָ��
  sub ebp, 25
  lea edi, lowf
  sub edi, 25
  lea esi, highf
  sub esi, 25
  lea ebx, buf
  sub ebx, 25
  ;ѭ���������SF��������
L:
  inc ecx
  ;����SF
  add ebx, 25
  imul eax, [ebx].SAMPLES.SDA, 5
  add eax, [ebx].SAMPLES.SDB
  sub eax, [ebx].SAMPLES.SDC
  add eax, 100
  mov [ebx].SAMPLES.SF, eax
  sar [ebx].SAMPLES.SF, 7   ;�Ż�����sarȡ��idivָ��
  ;���������洢����
  .if [ebx].SAMPLES.SF>0
      jmp high1
  .elseif [ebx].SAMPLES.SF<0
          jmp low1
  .else 
        jmp mid1
  .endif
mid1:      
  ;sf = 100�����
  add ebp, 25   ;�Ż��������ظ�����
  mov edx, ebp
  jmp over
low1:  
  ;sf<100�����
  add edi, 25
  mov edx, edi
  jmp over
high1:
  ;sf>100�����
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
  ;������ʱ
  invoke QueryPerformanceCounter, offset end_time

  mov    eax, end_time
  sub    eax,  start_time
  invoke printf, offset pFmt1, eax
  invoke ExitProcess, 0
main endp
end 