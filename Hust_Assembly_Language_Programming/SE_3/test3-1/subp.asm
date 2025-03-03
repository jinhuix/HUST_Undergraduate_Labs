.686P 
.model flat,c
 printf proto c :ptr sbyte, :vararg
 extern str20:sbyte
 N = 5

.code
;display: 显示数据子程序
;buff: 缓冲区的首地址
display proc buff:dword
  
  local num:dword   ;定义局部变量，表示存储循环的次数
  mov ecx, N
  mov num, ecx
  mov ebx, buff

  .while(num > 0)
    invoke printf, offset str8,ebx,dword ptr [ebx+9],dword ptr[ebx+13],dword ptr[ebx+17],dword ptr[ebx+21]    
    add ebx, 25
    dec num
  .endw
  ret
display endp
end