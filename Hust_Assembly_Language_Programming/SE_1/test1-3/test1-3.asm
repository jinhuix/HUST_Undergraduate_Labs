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
  err	db	"Incorrect Password！",0
  ans   db  10 dup(0)   ; 10个字节的空间，初值均为 0 
.stack 200
.code
start:
  invoke printf, offset  prtFmt, addr tip	;提示输入密码串
  invoke scanf, offset lpFmt,addr ans	;输入不超过10位的密码串
  invoke strlen, offset  ans			;计算密码串的长度
  ;比较密码串长度是否一致
  cmp eax, 10
  jnz error
  ;字符串长度一致，比较密码串字符是否一致
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

