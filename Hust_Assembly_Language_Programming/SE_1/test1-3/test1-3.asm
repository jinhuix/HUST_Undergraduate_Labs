.686     
.model flat, c
 ExitProcess proto stdcall :dword
 includelib  kernel32.lib  
 printf      proto c	:vararg
 scanf		 proto c	:vararg
 strlen      proto c	:ptr byte
 includelib  ucrt.lib
 includelib  libcmt.lib
 includelib  legacy_stdio_definitions.lib
.data
 lpFmt	db	"%s", 0
 prtFmt db "%s",0dh,0ah,0
  pass	db	"U202011675",0
  tip	db	"Please Enter The Password:",0
  rig	db	"OK!",0
  err	db	"Incorrect Password��",0
  ans   db  10 dup(0)   ; 10���ֽڵĿռ䣬��ֵ��Ϊ 0 
.stack 200
.code
start:
  invoke printf, offset  prtFmt, addr tip	;��ʾ�������봮
  invoke scanf, offset lpFmt,addr ans	;���벻����10λ�����봮
  invoke strlen, offset  ans			;�������봮�ĳ���
  ;�Ƚ����봮�����Ƿ�һ��
  cmp eax, 10
  jnz error
  ;�ַ�������һ�£��Ƚ����봮�ַ��Ƿ�һ��
  mov ecx, 10
  mov ebx, -1
isequal:
  inc ebx
  mov ah, byte ptr pass[ebx]
  cmp ah, byte ptr ans[ebx]
  loope isequal
  jz right
error:
  invoke printf, offset  prtFmt, offset err
  jmp exit
right:
  invoke printf, offset  prtFmt, offset rig
exit:invoke ExitProcess, 0
end start

