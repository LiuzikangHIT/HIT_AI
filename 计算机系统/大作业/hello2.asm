
hello：     文件格式 elf64-x86-64


Disassembly of section .init:

0000000000401000 <_init>:
  401000:	f3 0f 1e fa          	endbr64 
  401004:	48 83 ec 08          	sub    $0x8,%rsp
  401008:	48 8b 05 e9 2f 00 00 	mov    0x2fe9(%rip),%rax        # 403ff8 <__gmon_start__@Base>
  40100f:	48 85 c0             	test   %rax,%rax
  401012:	74 02                	je     401016 <_init+0x16>
  401014:	ff d0                	call   *%rax
  401016:	48 83 c4 08          	add    $0x8,%rsp
  40101a:	c3                   	ret    

Disassembly of section .plt:

0000000000401020 <.plt>:
  401020:	ff 35 e2 2f 00 00    	push   0x2fe2(%rip)        # 404008 <_GLOBAL_OFFSET_TABLE_+0x8>
  401026:	f2 ff 25 e3 2f 00 00 	bnd jmp *0x2fe3(%rip)        # 404010 <_GLOBAL_OFFSET_TABLE_+0x10>
  40102d:	0f 1f 00             	nopl   (%rax)
  401030:	f3 0f 1e fa          	endbr64 
  401034:	68 00 00 00 00       	push   $0x0
  401039:	f2 e9 e1 ff ff ff    	bnd jmp 401020 <_init+0x20>
  40103f:	90                   	nop
  401040:	f3 0f 1e fa          	endbr64 
  401044:	68 01 00 00 00       	push   $0x1
  401049:	f2 e9 d1 ff ff ff    	bnd jmp 401020 <_init+0x20>
  40104f:	90                   	nop
  401050:	f3 0f 1e fa          	endbr64 
  401054:	68 02 00 00 00       	push   $0x2
  401059:	f2 e9 c1 ff ff ff    	bnd jmp 401020 <_init+0x20>
  40105f:	90                   	nop
  401060:	f3 0f 1e fa          	endbr64 
  401064:	68 03 00 00 00       	push   $0x3
  401069:	f2 e9 b1 ff ff ff    	bnd jmp 401020 <_init+0x20>
  40106f:	90                   	nop
  401070:	f3 0f 1e fa          	endbr64 
  401074:	68 04 00 00 00       	push   $0x4
  401079:	f2 e9 a1 ff ff ff    	bnd jmp 401020 <_init+0x20>
  40107f:	90                   	nop
  401080:	f3 0f 1e fa          	endbr64 
  401084:	68 05 00 00 00       	push   $0x5
  401089:	f2 e9 91 ff ff ff    	bnd jmp 401020 <_init+0x20>
  40108f:	90                   	nop

Disassembly of section .plt.sec:

0000000000401090 <puts@plt>:
  401090:	f3 0f 1e fa          	endbr64 
  401094:	f2 ff 25 7d 2f 00 00 	bnd jmp *0x2f7d(%rip)        # 404018 <puts@GLIBC_2.2.5>
  40109b:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

00000000004010a0 <printf@plt>:
  4010a0:	f3 0f 1e fa          	endbr64 
  4010a4:	f2 ff 25 75 2f 00 00 	bnd jmp *0x2f75(%rip)        # 404020 <printf@GLIBC_2.2.5>
  4010ab:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

00000000004010b0 <getchar@plt>:
  4010b0:	f3 0f 1e fa          	endbr64 
  4010b4:	f2 ff 25 6d 2f 00 00 	bnd jmp *0x2f6d(%rip)        # 404028 <getchar@GLIBC_2.2.5>
  4010bb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

00000000004010c0 <atoi@plt>:
  4010c0:	f3 0f 1e fa          	endbr64 
  4010c4:	f2 ff 25 65 2f 00 00 	bnd jmp *0x2f65(%rip)        # 404030 <atoi@GLIBC_2.2.5>
  4010cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

00000000004010d0 <exit@plt>:
  4010d0:	f3 0f 1e fa          	endbr64 
  4010d4:	f2 ff 25 5d 2f 00 00 	bnd jmp *0x2f5d(%rip)        # 404038 <exit@GLIBC_2.2.5>
  4010db:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

00000000004010e0 <sleep@plt>:
  4010e0:	f3 0f 1e fa          	endbr64 
  4010e4:	f2 ff 25 55 2f 00 00 	bnd jmp *0x2f55(%rip)        # 404040 <sleep@GLIBC_2.2.5>
  4010eb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)

Disassembly of section .text:

00000000004010f0 <_start>:
  4010f0:	f3 0f 1e fa          	endbr64 
  4010f4:	31 ed                	xor    %ebp,%ebp
  4010f6:	49 89 d1             	mov    %rdx,%r9
  4010f9:	5e                   	pop    %rsi
  4010fa:	48 89 e2             	mov    %rsp,%rdx
  4010fd:	48 83 e4 f0          	and    $0xfffffffffffffff0,%rsp
  401101:	50                   	push   %rax
  401102:	54                   	push   %rsp
  401103:	45 31 c0             	xor    %r8d,%r8d
  401106:	31 c9                	xor    %ecx,%ecx
  401108:	48 c7 c7 25 11 40 00 	mov    $0x401125,%rdi
  40110f:	ff 15 db 2e 00 00    	call   *0x2edb(%rip)        # 403ff0 <__libc_start_main@GLIBC_2.34>
  401115:	f4                   	hlt    
  401116:	66 2e 0f 1f 84 00 00 	cs nopw 0x0(%rax,%rax,1)
  40111d:	00 00 00 

0000000000401120 <_dl_relocate_static_pie>:
  401120:	f3 0f 1e fa          	endbr64 
  401124:	c3                   	ret    

0000000000401125 <main>:
  401125:	f3 0f 1e fa          	endbr64 
  401129:	55                   	push   %rbp
  40112a:	48 89 e5             	mov    %rsp,%rbp
  40112d:	48 83 ec 20          	sub    $0x20,%rsp
  401131:	89 7d ec             	mov    %edi,-0x14(%rbp)
  401134:	48 89 75 e0          	mov    %rsi,-0x20(%rbp)
  401138:	83 7d ec 05          	cmpl   $0x5,-0x14(%rbp)
  40113c:	74 19                	je     401157 <main+0x32>
  40113e:	48 8d 05 c3 0e 00 00 	lea    0xec3(%rip),%rax        # 402008 <_IO_stdin_used+0x8>
  401145:	48 89 c7             	mov    %rax,%rdi
  401148:	e8 43 ff ff ff       	call   401090 <puts@plt>
  40114d:	bf 01 00 00 00       	mov    $0x1,%edi
  401152:	e8 79 ff ff ff       	call   4010d0 <exit@plt>
  401157:	c7 45 fc 00 00 00 00 	movl   $0x0,-0x4(%rbp)
  40115e:	eb 56                	jmp    4011b6 <main+0x91>
  401160:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
  401164:	48 83 c0 18          	add    $0x18,%rax
  401168:	48 8b 08             	mov    (%rax),%rcx
  40116b:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
  40116f:	48 83 c0 10          	add    $0x10,%rax
  401173:	48 8b 10             	mov    (%rax),%rdx
  401176:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
  40117a:	48 83 c0 08          	add    $0x8,%rax
  40117e:	48 8b 00             	mov    (%rax),%rax
  401181:	48 89 c6             	mov    %rax,%rsi
  401184:	48 8d 05 ad 0e 00 00 	lea    0xead(%rip),%rax        # 402038 <_IO_stdin_used+0x38>
  40118b:	48 89 c7             	mov    %rax,%rdi
  40118e:	b8 00 00 00 00       	mov    $0x0,%eax
  401193:	e8 08 ff ff ff       	call   4010a0 <printf@plt>
  401198:	48 8b 45 e0          	mov    -0x20(%rbp),%rax
  40119c:	48 83 c0 20          	add    $0x20,%rax
  4011a0:	48 8b 00             	mov    (%rax),%rax
  4011a3:	48 89 c7             	mov    %rax,%rdi
  4011a6:	e8 15 ff ff ff       	call   4010c0 <atoi@plt>
  4011ab:	89 c7                	mov    %eax,%edi
  4011ad:	e8 2e ff ff ff       	call   4010e0 <sleep@plt>
  4011b2:	83 45 fc 01          	addl   $0x1,-0x4(%rbp)
  4011b6:	83 7d fc 09          	cmpl   $0x9,-0x4(%rbp)
  4011ba:	7e a4                	jle    401160 <main+0x3b>
  4011bc:	e8 ef fe ff ff       	call   4010b0 <getchar@plt>
  4011c1:	b8 00 00 00 00       	mov    $0x0,%eax
  4011c6:	c9                   	leave  
  4011c7:	c3                   	ret    

Disassembly of section .fini:

00000000004011c8 <_fini>:
  4011c8:	f3 0f 1e fa          	endbr64 
  4011cc:	48 83 ec 08          	sub    $0x8,%rsp
  4011d0:	48 83 c4 08          	add    $0x8,%rsp
  4011d4:	c3                   	ret    
