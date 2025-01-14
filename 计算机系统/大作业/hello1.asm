
hello.o：     文件格式 elf64-x86-64


Disassembly of section .text:

0000000000000000 <main>:
   0:	f3 0f 1e fa          	endbr64 
   4:	55                   	push   %rbp
   5:	48 89 e5             	mov    %rsp,%rbp
   8:	48 83 ec 20          	sub    $0x20,%rsp
   c:	89 7d ec             	mov    %edi,-0x14(%rbp)
   f:	48 89 75 e0          	mov    %rsi,-0x20(%rbp)
  13:	83 7d ec 05          	cmpl   $0x5,-0x14(%rbp)
  17:	74 19                	je     32 <main+0x32>
  19:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax        # 20 <main+0x20>
  20:	48 89 c7             	mov    %rax,%rdi
  23:	e8 00 00 00 00       	call   28 <main+0x28>
  28:	bf 01 00 00 00       	mov    $0x1,%edi
  2d:	e8 00 00 00 00       	call   32 <main+0x32>
  32:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%rbp)
  39:	eb 56                	jmp    91 <main+0x91>
  3b:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
  3f:	48 83 c0 18          	add    $0x18,%rax
  43:	48 8b 08             	mov    (%rax),%rcx
  46:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
  4a:	48 83 c0 10          	add    $0x10,%rax
  4e:	48 8b 10             	mov    (%rax),%rdx
  51:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
  55:	48 83 c0 08          	add    $0x8,%rax
  59:	48 8b 00             	mov    (%rax),%rax
  5c:	48 89 c6             	mov    %rax,%rsi
  5f:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax        # 66 <main+0x66>
  66:	48 89 c7             	mov    %rax,%rdi
  69:	b8 00 00 00 00       	mov    $0x0,%eax
  6e:	e8 00 00 00 00       	call   73 <main+0x73>
  73:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
  77:	48 83 c0 20          	add    $0x20,%rax
  7b:	48 8b 00             	mov    (%rax),%rax
  7e:	48 89 c7             	mov    %rax,%rdi
  81:	e8 00 00 00 00       	call   86 <main+0x86>
  86:	89 c7                	mov    %eax,%edi
  88:	e8 00 00 00 00       	call   8d <main+0x8d>
  8d:	83 45 fc 01          	addl   $0x1,-0x4(%rbp)
  91:	83 7d fc 09          	cmpl   $0x9,-0x4(%rbp)
  95:	7e a4                	jle    3b <main+0x3b>
  97:	e8 00 00 00 00       	call   9c <main+0x9c>
  9c:	b8 00 00 00 00       	mov    $0x0,%eax
  a1:	c9                   	leave  
  a2:	c3                   	ret    
