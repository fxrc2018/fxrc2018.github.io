## Bomb Lab

这个实验室个人感觉十分有意思，大致的流程是，通过读汇编代码，找出对应的输入，使得这个这个程序不会执行explode_bomb函数。

下载实验所需的tar文件，然后解压，阅读实验手册，然后就可以开始实验了。我个人感觉这个实验，如果弄清楚了以下几点，就应该能做出来：

* 函数的参数及返回值。
* 栈上的局部变量的类型和值。
* 对条件语句和循环语句的汇编形式比较熟悉。
* 对gdb的使用比较熟悉。

下面正式开始说一说实验的流程。首先，看一看其中的bomb.c文件，看了之后发现就是一些流程的东西，大致是从输入流或文件读入输入，然后调用函数进行判断。c语言关键的代码如下：

```cpp
input = read_line(); /* Get input */
phase_1(input); /* Run the phase */
phase_defused(); /* Drat!  They figured it out! Let me know how they did it. */
printf("Phase 1 defused. How about the next one?\n");
```

后面还有5个类似的。看来，c文件的提示不是很多，还是要看汇编代码。使用命令

```shell
objdump -D bomb > bomb.s
```
得到汇编代码。在C代码中，我们知道了大致的流程，这里只需要逐个击破就可以了。

### phase_1
先看phase_1的汇编代码：

```cpp
0000000000400ee0 <phase_1>:
  400ee0:	48 83 ec 08          	sub    $0x8,%rsp
  400ee4:	be 00 24 40 00       	mov    $0x402400,%esi
  400ee9:	e8 4a 04 00 00       	callq  401338 <strings_not_equal>
  400eee:	85 c0                	test   %eax,%eax
  400ef0:	74 05                	je     400ef7 <phase_1+0x17>
  400ef2:	e8 43 05 00 00       	callq  40143a <explode_bomb>
  400ef7:	48 83 c4 08          	add    $0x8,%rsp
  400efb:	c3                   	retq   
```

通过阅读汇编代码可以知道，这里首先进行了一次入栈，然后调用了一个函数string_not_equal，然后比较返回值，如果为0，就返回；如果不为0，就调用explode_bomb。这里要回答第一个问题，string_not_equal的参数是哪个，很明显，第一个参数就是%rdi，这个参数由read_line函数传入，是我们写入的字符串的地址。第二个为%esi，也就是0x402400的值。这里看函数名，大致可以猜到是什么功能，所以，这里也没必要去看函数的功能，先试试输入0x402400的值，看看能不能出结果。

要知道这个值，必须要用到gdb调试，基本的思路是，先随便输入一个字符串，然后在调用string_not_equal之前打一个断点，看看能不能出结果。下面是操作的步骤：

```shell
gdb bomb
break * 0x400ee9
x/s 0x402400 /* 以字符串形式检查地址0x402400的值 */
```

我们发现，地址0x402400上确实有一个字符串，它的值为

```shell
Border relations with Canada have never been better.
```

我们尝试输入这个值，发现确实可以通过。说明这里函数的命名都是很规范的，一般情况下不用去阅读这种命名的函数的汇编代码。

### phase_2
先看phase_2的汇编代码：

```cpp
0000000000400efc <phase_2>:
  400efc:	55                   	push   %rbp
  400efd:	53                   	push   %rbx
  400efe:	48 83 ec 28          	sub    $0x28,%rsp
  400f02:	48 89 e6             	mov    %rsp,%rsi
  400f05:	e8 52 05 00 00       	callq  40145c <read_six_numbers>
  400f0a:	83 3c 24 01          	cmpl   $0x1,(%rsp)
  400f0e:	74 20                	je     400f30 <phase_2+0x34>
  400f10:	e8 25 05 00 00       	callq  40143a <explode_bomb>
  400f15:	eb 19                	jmp    400f30 <phase_2+0x34>
  400f17:	8b 43 fc             	mov    -0x4(%rbx),%eax
  400f1a:	01 c0                	add    %eax,%eax
  400f1c:	39 03                	cmp    %eax,(%rbx)
  400f1e:	74 05                	je     400f25 <phase_2+0x29>
  400f20:	e8 15 05 00 00       	callq  40143a <explode_bomb>
  400f25:	48 83 c3 04          	add    $0x4,%rbx
  400f29:	48 39 eb             	cmp    %rbp,%rbx
  400f2c:	75 e9                	jne    400f17 <phase_2+0x1b>
  400f2e:	eb 0c                	jmp    400f3c <phase_2+0x40>
  400f30:	48 8d 5c 24 04       	lea    0x4(%rsp),%rbx
  400f35:	48 8d 6c 24 18       	lea    0x18(%rsp),%rbp
  400f3a:	eb db                	jmp    400f17 <phase_2+0x1b>
  400f3c:	48 83 c4 28          	add    $0x28,%rsp
  400f40:	5b                   	pop    %rbx
  400f41:	5d                   	pop    %rbp
  400f42:	c3                   	retq 
```

这里有另外一个函数，read_six_numbers，这个函数也是一个规范命名的函数，从函数名称我们可以看出，是读入6个数据，但这6个数据读到哪里去，又从哪里读，我们并不知道，所以还是要读一读这个函数。这里，我们先明确一下参数，第一个参数，我们写入的字符串的地址，第二个参数，栈指针的地址。下面是read_six_numbers函数的汇编代码：

```cpp
000000000040145c <read_six_numbers>:
  40145c:	48 83 ec 18          	sub    $0x18,%rsp
  401460:	48 89 f2             	mov    %rsi,%rdx
  401463:	48 8d 4e 04          	lea    0x4(%rsi),%rcx
  401467:	48 8d 46 14          	lea    0x14(%rsi),%rax
  40146b:	48 89 44 24 08       	mov    %rax,0x8(%rsp)
  401470:	48 8d 46 10          	lea    0x10(%rsi),%rax
  401474:	48 89 04 24          	mov    %rax,(%rsp)
  401478:	4c 8d 4e 0c          	lea    0xc(%rsi),%r9
  40147c:	4c 8d 46 08          	lea    0x8(%rsi),%r8
  401480:	be c3 25 40 00       	mov    $0x4025c3,%esi
  401485:	b8 00 00 00 00       	mov    $0x0,%eax
  40148a:	e8 61 f7 ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  40148f:	83 f8 05             	cmp    $0x5,%eax
  401492:	7f 05                	jg     401499 <read_six_numbers+0x3d>
  401494:	e8 a1 ff ff ff       	callq  40143a <explode_bomb>
  401499:	48 83 c4 18          	add    $0x18,%rsp
  40149d:	c3                   	retq   
```

这里的汇编有点迷惑性，主要是关于%rax相关的操作我不是很明白，后来发现这个并不影响这个题目。这里主要是调用了fscanf这个函数，然后检查了这个函数的返回值，看看是不是大于5。这里弄清楚函数的参数就知道了。fscanf这个函数的原型为：

```cpp
int fscanf(FILE*stream,const char *format,...);
```

这里一对照就很容易得到结果，第一个参数是我们输入字符串得地址，第二个参数是地址0x4025c3的一个字符串，检查这个字符串可以知道这个字符串为：

```cpp
%d %d %d %d %d %d
```
后面的参数很容易想到了，就是6个地址，从参数从汇编代码可以看出，就是传入的sp指针依次开始。然后通过看汇编代码可知，先判断第一个值是不是1，然后就进入了一个循环，每次判断前一个的两倍是不是等于后一个，直到最后一个。所以，这个答案为：

```cpp
1 2 4 8 16 32
```

##3 phase_3

```cpp
0000000000400f43 <phase_3>:
  400f43:	48 83 ec 18          	sub    $0x18,%rsp
  400f47:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  400f4c:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  400f51:	be cf 25 40 00       	mov    $0x4025cf,%esi
  400f56:	b8 00 00 00 00       	mov    $0x0,%eax
  400f5b:	e8 90 fc ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  400f60:	83 f8 01             	cmp    $0x1,%eax
  400f63:	7f 05                	jg     400f6a <phase_3+0x27>
  400f65:	e8 d0 04 00 00       	callq  40143a <explode_bomb>
  400f6a:	83 7c 24 08 07       	cmpl   $0x7,0x8(%rsp)
  400f6f:	77 3c                	ja     400fad <phase_3+0x6a>
  400f71:	8b 44 24 08          	mov    0x8(%rsp),%eax
  400f75:	ff 24 c5 70 24 40 00 	jmpq   *0x402470(,%rax,8)
  400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax
  400f81:	eb 3b                	jmp    400fbe <phase_3+0x7b>
  400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax
  400f88:	eb 34                	jmp    400fbe <phase_3+0x7b>
  400f8a:	b8 00 01 00 00       	mov    $0x100,%eax
  400f8f:	eb 2d                	jmp    400fbe <phase_3+0x7b>
  400f91:	b8 85 01 00 00       	mov    $0x185,%eax
  400f96:	eb 26                	jmp    400fbe <phase_3+0x7b>
  400f98:	b8 ce 00 00 00       	mov    $0xce,%eax
  400f9d:	eb 1f                	jmp    400fbe <phase_3+0x7b>
  400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax
  400fa4:	eb 18                	jmp    400fbe <phase_3+0x7b>
  400fa6:	b8 47 01 00 00       	mov    $0x147,%eax
  400fab:	eb 11                	jmp    400fbe <phase_3+0x7b>
  400fad:	e8 88 04 00 00       	callq  40143a <explode_bomb>
  400fb2:	b8 00 00 00 00       	mov    $0x0,%eax
  400fb7:	eb 05                	jmp    400fbe <phase_3+0x7b>
  400fb9:	b8 37 01 00 00       	mov    $0x137,%eax
  400fbe:	3b 44 24 0c          	cmp    0xc(%rsp),%eax
  400fc2:	74 05                	je     400fc9 <phase_3+0x86>
  400fc4:	e8 71 04 00 00       	callq  40143a <explode_bomb>
  400fc9:	48 83 c4 18          	add    $0x18,%rsp
  400fcd:	c3                   	retq   
```

通过看汇编代码，首先看到fscanf的调用，首先弄清楚这个调用的参数和返回值，先通过检查地址0x4025cf字符串的值，知道需要传入的格式如下：

```cpp
%d %d
```

有了前面的经验，这两个值的存储位置也好确定，就是sp+8和sp+12的位置。然后就只能通过阅读汇编，确定这个函数的逻辑了。前面的内容比较好理解，就是判断读到的个数，如果大于1，再执行后面的内容。然后比较第一个数，看是不是大于7，大于7，就直接执行explode_bomb函数。有点不好理解的是下面这一句：

```cpp
jmpq   *0x402470(,%rax,8)
```

这一句的寻址规则是这样的，跳转到地址0x402470的值加上8 * %rax的位置。首先通过检查地址0x402470的值，得到跳转的地址表：

```cpp
(gdb) x/8x 0x402470
0x402470:       0x0000000000400f7c      0x0000000000400fb9
0x402480:       0x0000000000400f83      0x0000000000400f8a
0x402490:       0x0000000000400f91      0x0000000000400f98
0x4024a0:       0x0000000000400f9f      0x0000000000400fa6
```

这里以0为例，将会跳转到地址0x400f7c去执行，在去看这个位置的代码，发现还是比较简单的，就是比较一个特定值和第二个数是否相等。这里可以看出，这里存在多组解，以0为例，答案是：

```cpp
0 207
```

我开始做的时候，并没有想太多，看出了第一个应该小于7，我就随便输入了一个，用gdb执行的时候，使用stepi进行调试，发现直接就可以跳转到后面，然后就打印了%rax的值，然后就过了。

### phase_4

```cpp
000000000040100c <phase_4>:
  40100c:	48 83 ec 18          	sub    $0x18,%rsp
  401010:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  401015:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  40101a:	be cf 25 40 00       	mov    $0x4025cf,%esi
  40101f:	b8 00 00 00 00       	mov    $0x0,%eax
  401024:	e8 c7 fb ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  401029:	83 f8 02             	cmp    $0x2,%eax
  40102c:	75 07                	jne    401035 <phase_4+0x29>
  40102e:	83 7c 24 08 0e       	cmpl   $0xe,0x8(%rsp)
  401033:	76 05                	jbe    40103a <phase_4+0x2e>
  401035:	e8 00 04 00 00       	callq  40143a <explode_bomb>
  40103a:	ba 0e 00 00 00       	mov    $0xe,%edx
  40103f:	be 00 00 00 00       	mov    $0x0,%esi
  401044:	8b 7c 24 08          	mov    0x8(%rsp),%edi
  401048:	e8 81 ff ff ff       	callq  400fce <func4>
  40104d:	85 c0                	test   %eax,%eax
  40104f:	75 07                	jne    401058 <phase_4+0x4c>
  401051:	83 7c 24 0c 00       	cmpl   $0x0,0xc(%rsp)
  401056:	74 05                	je     40105d <phase_4+0x51>
  401058:	e8 dd 03 00 00       	callq  40143a <explode_bomb>
  40105d:	48 83 c4 18          	add    $0x18,%rsp
  401061:	c3                   	retq 
```

这里的汇编代码和前面很类似，就是读入两个数，再检查读入的数量是不是等于2，然后调用func4函数，最后检查func4的返回值是不是等于0，在检查第二个输入的数是不是等于0。可以看出，这里的关键是func4这个函数。

```cpp
0000000000400fce <func4>:
  400fce:	48 83 ec 08          	sub    $0x8,%rsp
  400fd2:	89 d0                	mov    %edx,%eax
  400fd4:	29 f0                	sub    %esi,%eax
  400fd6:	89 c1                	mov    %eax,%ecx
  400fd8:	c1 e9 1f             	shr    $0x1f,%ecx
  400fdb:	01 c8                	add    %ecx,%eax
  400fdd:	d1 f8                	sar    %eax
  400fdf:	8d 0c 30             	lea    (%rax,%rsi,1),%ecx
  400fe2:	39 f9                	cmp    %edi,%ecx
  400fe4:	7e 0c                	jle    400ff2 <func4+0x24>
  400fe6:	8d 51 ff             	lea    -0x1(%rcx),%edx
  400fe9:	e8 e0 ff ff ff       	callq  400fce <func4>
  400fee:	01 c0                	add    %eax,%eax
  400ff0:	eb 15                	jmp    401007 <func4+0x39>
  400ff2:	b8 00 00 00 00       	mov    $0x0,%eax
  400ff7:	39 f9                	cmp    %edi,%ecx
  400ff9:	7d 0c                	jge    401007 <func4+0x39>
  400ffb:	8d 71 01             	lea    0x1(%rcx),%esi
  400ffe:	e8 cb ff ff ff       	callq  400fce <func4>
  401003:	8d 44 00 01          	lea    0x1(%rax,%rax,1),%eax
  401007:	48 83 c4 08          	add    $0x8,%rsp
  40100b:	c3
```

func4函数使用了递归，但是，我感觉这里难的不是递归那里看不懂，而是前面的操作有点让人迷惑。下面写出这个汇编代码对应C语言代码：

```cpp
int func4(int x, int y, int z){
    int a = z;
    a -= y;
    int b = a;
    b >>= 31; /* logic */
    a += b;
    a >>= 1;

    b = a + y;
    if(b -x <= 0){
        if(b- x >= 0){
            return 0;
        }
        int ret = func4(x,b+1,z);
        return 2 * ret + 1;
    }else{
        z = b - 1;
        int ret = func4(x,y,z);
        return ret + ret;
    }
}
```

这里看上去执行了很多操作，但其实就相当于下面这一句话：

```cpp
int b = y + (z - y)/2;
```

这里不好理解的右移31的操作，因为有可能大于z，所以在除以二的时候，为了保证向0取整，如果z-y是负数的情况会+1。这里虽然是递归，但是题目本身并没有用到递归，通过传入的参数发现，y=0，z=14，要返回的值为0，所以就是递归出口，传入的第一个参数为7就可以了。所以答案为;

```cpp
7 0
```


### phase_5

```cpp
0000000000401062 <phase_5>:
  401062:	53                   	push   %rbx
  401063:	48 83 ec 20          	sub    $0x20,%rsp
  401067:	48 89 fb             	mov    %rdi,%rbx
  40106a:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax
  401071:	00 00 
  401073:	48 89 44 24 18       	mov    %rax,0x18(%rsp)
  401078:	31 c0                	xor    %eax,%eax
  40107a:	e8 9c 02 00 00       	callq  40131b <string_length>
  40107f:	83 f8 06             	cmp    $0x6,%eax
  401082:	74 4e                	je     4010d2 <phase_5+0x70>
  401084:	e8 b1 03 00 00       	callq  40143a <explode_bomb>
  401089:	eb 47                	jmp    4010d2 <phase_5+0x70>
  40108b:	0f b6 0c 03          	movzbl (%rbx,%rax,1),%ecx
  40108f:	88 0c 24             	mov    %cl,(%rsp)
  401092:	48 8b 14 24          	mov    (%rsp),%rdx
  401096:	83 e2 0f             	and    $0xf,%edx
  401099:	0f b6 92 b0 24 40 00 	movzbl 0x4024b0(%rdx),%edx
  4010a0:	88 54 04 10          	mov    %dl,0x10(%rsp,%rax,1)
  4010a4:	48 83 c0 01          	add    $0x1,%rax
  4010a8:	48 83 f8 06          	cmp    $0x6,%rax
  4010ac:	75 dd                	jne    40108b <phase_5+0x29>
  4010ae:	c6 44 24 16 00       	movb   $0x0,0x16(%rsp)
  4010b3:	be 5e 24 40 00       	mov    $0x40245e,%esi
  4010b8:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi
  4010bd:	e8 76 02 00 00       	callq  401338 <strings_not_equal>
  4010c2:	85 c0                	test   %eax,%eax
  4010c4:	74 13                	je     4010d9 <phase_5+0x77>
  4010c6:	e8 6f 03 00 00       	callq  40143a <explode_bomb>
  4010cb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
  4010d0:	eb 07                	jmp    4010d9 <phase_5+0x77>
  4010d2:	b8 00 00 00 00       	mov    $0x0,%eax
  4010d7:	eb b2                	jmp    40108b <phase_5+0x29>
  4010d9:	48 8b 44 24 18       	mov    0x18(%rsp),%rax
  4010de:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax
  4010e5:	00 00 
  4010e7:	74 05                	je     4010ee <phase_5+0x8c>
  4010e9:	e8 42 fa ff ff       	callq  400b30 <__stack_chk_fail@plt>
  4010ee:	48 83 c4 20          	add    $0x20,%rsp
  4010f2:	5b                   	pop    %rbx
  4010f3:	c3                   	retq   
```

这个题目，我先看了前面汇编代码，发现有个指令看不懂：

```cpp
mov    %fs:0x28,%rax
```

做完了才发现，这个命令其实不用了解，因为前面执行的是保存栈和寄存器的操作，从%rax开始才是正文。这里调用了string_length这个函数，这个函数的功能可以看出来，然后检查返回值，看是否为6。后面执行了一个循环语句，接着调用了strings_not_equal函数，可以看出，如果相等，函数就可以跳转出去了。所以，这里我用了一个取巧的方法，先输入一个6字符串的值，然后把strings_not_equal函数的参数打印一下，发现第二个参数就是：

```cpp
flyers
```

于是我输入这个字符串，发现并不对。打印发现，传入的第一个参数不是我的输入，而是另外一个字符。看来，这中间经过了一些转换。

通过阅读汇编代码，发现它把输入的字符串，使用低4位进行寻址，变为了地址0x4024b0中的字符，通过打印这个地址的字符串：

```cpp
maduiersnfotvbylSo you think you can stop the bomb with ctrl-c, do you
```

后面的事情就比较简单了，通过算出flyers对应的偏移，在使用ASCII码进行对应，可以得到答案如下：

```cpp
ionefg
```

### phase_6
由于phase_6的代码有点多，所以分为几个部分来说。

```cpp
00000000004010f4 <phase_6>:
  4010f4:	41 56                	push   %r14
  4010f6:	41 55                	push   %r13
  4010f8:	41 54                	push   %r12
  4010fa:	55                   	push   %rbp
  4010fb:	53                   	push   %rbx
  4010fc:	48 83 ec 50          	sub    $0x50,%rsp
  401100:	49 89 e5             	mov    %rsp,%r13
  401103:	48 89 e6             	mov    %rsp,%rsi
  401106:	e8 51 03 00 00       	callq  40145c <read_six_numbers>
  40110b:	49 89 e6             	mov    %rsp,%r14
  40110e:	41 bc 00 00 00 00    	mov    $0x0,%r12d
  401114:	4c 89 ed             	mov    %r13,%rbp
  401117:	41 8b 45 00          	mov    0x0(%r13),%eax
  40111b:	83 e8 01             	sub    $0x1,%eax
  40111e:	83 f8 05             	cmp    $0x5,%eax
  401121:	76 05                	jbe    401128 <phase_6+0x34>
  401123:	e8 12 03 00 00       	callq  40143a <explode_bomb>
  401128:	41 83 c4 01          	add    $0x1,%r12d
  40112c:	41 83 fc 06          	cmp    $0x6,%r12d
  401130:	74 21                	je     401153 <phase_6+0x5f>
  401132:	44 89 e3             	mov    %r12d,%ebx
  401135:	48 63 c3             	movslq %ebx,%rax
  401138:	8b 04 84             	mov    (%rsp,%rax,4),%eax
  40113b:	39 45 00             	cmp    %eax,0x0(%rbp)
  40113e:	75 05                	jne    401145 <phase_6+0x51>
  401140:	e8 f5 02 00 00       	callq  40143a <explode_bomb>
  401145:	83 c3 01             	add    $0x1,%ebx
  401148:	83 fb 05             	cmp    $0x5,%ebx
  40114b:	7e e8                	jle    401135 <phase_6+0x41>
  40114d:	49 83 c5 04          	add    $0x4,%r13
  401151:	eb c1                	jmp    401114 <phase_6+0x20>
```

这部分主要是读入6个数据，然后进行检查，不能相等，如果相等，就会调用explode_bomb函数。由于已经分析过read_six_numbers这个函数，这里就不分析了。这里是把数据读到了从栈顶开始的0x18个字节。

```cpp
  401153:	48 8d 74 24 18       	lea    0x18(%rsp),%rsi
  401158:	4c 89 f0             	mov    %r14,%rax
  40115b:	b9 07 00 00 00       	mov    $0x7,%ecx
  401160:	89 ca                	mov    %ecx,%edx
  401162:	2b 10                	sub    (%rax),%edx
  401164:	89 10                	mov    %edx,(%rax)
  401166:	48 83 c0 04          	add    $0x4,%rax
  40116a:	48 39 f0             	cmp    %rsi,%rax
  40116d:	75 f1                	jne    401160 <phase_6+0x6c>
```

这里把输入的数据进行一个转换，如果输入的数据为x，转换为7-x。对于循环来说，题目中的汇编普遍喜欢使用尾指针的形式，如同下面的C语言。

```cpp
int arr[6];
int *p ;
int *end = &arr[6];
for(p = arr;p!=end;p++){
  *p = 7 - *p;
}
```

```cpp
  40116f:	be 00 00 00 00       	mov    $0x0,%esi
  401174:	eb 21                	jmp    401197 <phase_6+0xa3>
  401176:	48 8b 52 08          	mov    0x8(%rdx),%rdx
  40117a:	83 c0 01             	add    $0x1,%eax
  40117d:	39 c8                	cmp    %ecx,%eax
  40117f:	75 f5                	jne    401176 <phase_6+0x82>
  401181:	eb 05                	jmp    401188 <phase_6+0x94>
  401183:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  401188:	48 89 54 74 20       	mov    %rdx,0x20(%rsp,%rsi,2)
  40118d:	48 83 c6 04          	add    $0x4,%rsi
  401191:	48 83 fe 18          	cmp    $0x18,%rsi
  401195:	74 14                	je     4011ab <phase_6+0xb7>
  401197:	8b 0c 34             	mov    (%rsp,%rsi,1),%ecx
  40119a:	83 f9 01             	cmp    $0x1,%ecx
  40119d:	7e e4                	jle    401183 <phase_6+0x8f>
  40119f:	b8 01 00 00 00       	mov    $0x1,%eax
  4011a4:	ba d0 32 60 00       	mov    $0x6032d0,%edx
  4011a9:	eb cb                	jmp    401176 <phase_6+0x82>
```

个人感觉从这里开始，就开始变得不好理解了，因为涉及到一个结构体，这个结构体也比较简单，就是一个链表。但问题是，在课本上对于这部分的讲解相当少，而且汇编代码会使人迷惑，让人搞不清楚它的意图。首先我们检查0x6032d0这个地址：

```cpp
(gdb) x/x 0x6032d0
0x6032d0 <node1>:       0x0000014c
```

这里我们可以发现这里有一个结构体。上面的一句汇编

```cpp
mov    0x8(%rdx),%rdx
```

我们发现它有一个偏移为8的成员，我们偏移8，然后在看看对应地址上存储的内容：

```cpp
(gdb) x/x 0x6032d8      
0x6032d8 <node1+8>:     0x006032e0
(gdb) x/x 0x6032e0
0x6032e0 <node2>:       0x000000a8
```

从这里大致可以想到，这是一个链表，第一个成员是value，第二个成员是next。弄清楚了这里的内容，上面的代码就好理解了，对应的C语言代码如下：

```cpp
int arr[6]; /* sp */
    node n1; /* node的首地址 */
    node p[6]; /* sp+0x20 - sp+0x50 */
    int i;
    for(i=0;i<6;i++){
        if(i <= 1){
            p[i] = n1;
        }else{
            int j;
            node temp = n1;
            for(j=1;j<i;j++){
                temp = temp->next;
            }
            p[i] = temp;
        }
    }
```

这里实现的功能就是根据输入的值，去读取链表中的第i个成员。

```cpp
  4011ab:	48 8b 5c 24 20       	mov    0x20(%rsp),%rbx
  4011b0:	48 8d 44 24 28       	lea    0x28(%rsp),%rax
  4011b5:	48 8d 74 24 50       	lea    0x50(%rsp),%rsi
  4011ba:	48 89 d9             	mov    %rbx,%rcx
  4011bd:	48 8b 10             	mov    (%rax),%rdx
  4011c0:	48 89 51 08          	mov    %rdx,0x8(%rcx)
  4011c4:	48 83 c0 08          	add    $0x8,%rax
  4011c8:	48 39 f0             	cmp    %rsi,%rax
  4011cb:	74 05                	je     4011d2 <phase_6+0xde>
  4011cd:	48 89 d1             	mov    %rdx,%rcx
  4011d0:	eb eb                	jmp    4011bd <phase_6+0xc9>
```

有了前面的分析，这里就比较容易了，就是调整链表，把链表从低地址到高地址连成一串。

```cpp
  4011d2:	48 c7 42 08 00 00 00 	movq   $0x0,0x8(%rdx)
  4011d9:	00 
  4011da:	bd 05 00 00 00       	mov    $0x5,%ebp
  4011df:	48 8b 43 08          	mov    0x8(%rbx),%rax
  4011e3:	8b 00                	mov    (%rax),%eax
  4011e5:	39 03                	cmp    %eax,(%rbx)
  4011e7:	7d 05                	jge    4011ee <phase_6+0xfa>
  4011e9:	e8 4c 02 00 00       	callq  40143a <explode_bomb>
  4011ee:	48 8b 5b 08          	mov    0x8(%rbx),%rbx
  4011f2:	83 ed 01             	sub    $0x1,%ebp
  4011f5:	75 e8                	jne    4011df <phase_6+0xeb>
  4011f7:	48 83 c4 50          	add    $0x50,%rsp
  4011fb:	5b                   	pop    %rbx
  4011fc:	5d                   	pop    %rbp
  4011fd:	41 5c                	pop    %r12
  4011ff:	41 5d                	pop    %r13
  401201:	41 5e                	pop    %r14
  401203:	c3                   	retq   
```

这里就是进行验证的阶段，它会验证链表的前一个结点的值，是不是大于链表后一个结点的值。所以到了这一步，我们基本上已经明白思路了，就是通过输入的值，把链表从大到小排序。所以这里我们先要知道链表的值。在这里呢，出题人问了简单，链表是放在一起的，在使用

```cpp
(gdb) x/24d 0x6032d0  
0x6032d0 <node1>:       332     1       6304480 0
0x6032e0 <node2>:       168     2       6304496 0
0x6032f0 <node3>:       924     3       6304512 0
0x603300 <node4>:       691     4       6304528 0
0x603310 <node5>:       477     5       6304544 0
0x603320 <node6>:       443     6       0       0
```

可以看到大小顺序，从大到小排序应该是3 4 5 6 1 2，但应该记得，我们输入的数据是经过转换的，所以，正确的答案为：

```cpp
4 3 2 1 6 5
```

### secret_phase
当6关都通过的时候，我们发现phase_defused这个函数并没有作用，因为在函数中如果触发了explode_bomb，就会直接退出。那么phase_defused这个函数是来干什么的呢，其实就是用来触发secret_phase的，通过阅读汇编代码（这里的代码省略了用于字节对齐的nop指令），我们就可以找到进入secret_phase的方法。

```cpp
00000000004015c4 <phase_defused>:
  4015c4:	48 83 ec 78          	sub    $0x78,%rsp
  4015c8:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax
  4015cf:	00 00 
  4015d1:	48 89 44 24 68       	mov    %rax,0x68(%rsp)
  4015d6:	31 c0                	xor    %eax,%eax
  4015d8:	83 3d 81 21 20 00 06 	cmpl   $0x6,0x202181(%rip)  # 603760 <num_input_strings>
  4015df:	75 5e                	jne    40163f <phase_defused+0x7b>
  4015e1:	4c 8d 44 24 10       	lea    0x10(%rsp),%r8
  4015e6:	48 8d 4c 24 0c       	lea    0xc(%rsp),%rcx
  4015eb:	48 8d 54 24 08       	lea    0x8(%rsp),%rdx
  4015f0:	be 19 26 40 00       	mov    $0x402619,%esi
  4015f5:	bf 70 38 60 00       	mov    $0x603870,%edi
  4015fa:	e8 f1 f5 ff ff       	callq  400bf0 <__isoc99_sscanf@plt>
  4015ff:	83 f8 03             	cmp    $0x3,%eax
  401602:	75 31                	jne    401635 <phase_defused+0x71>
  401604:	be 22 26 40 00       	mov    $0x402622,%esi
  401609:	48 8d 7c 24 10       	lea    0x10(%rsp),%rdi
  40160e:	e8 25 fd ff ff       	callq  401338 <strings_not_equal>
  401613:	85 c0                	test   %eax,%eax
  401615:	75 1e                	jne    401635 <phase_defused+0x71>
  401617:	bf f8 24 40 00       	mov    $0x4024f8,%edi
  40161c:	e8 ef f4 ff ff       	callq  400b10 <puts@plt>
  401621:	bf 20 25 40 00       	mov    $0x402520,%edi
  401626:	e8 e5 f4 ff ff       	callq  400b10 <puts@plt>
  40162b:	b8 00 00 00 00       	mov    $0x0,%eax
  401630:	e8 0d fc ff ff       	callq  401242 <secret_phase>
  401635:	bf 58 25 40 00       	mov    $0x402558,%edi
  40163a:	e8 d1 f4 ff ff       	callq  400b10 <puts@plt>
  40163f:	48 8b 44 24 68       	mov    0x68(%rsp),%rax
  401644:	64 48 33 04 25 28 00 	xor    %fs:0x28,%rax
  40164b:	00 00 
  40164d:	74 05                	je     401654 <phase_defused+0x90>
  40164f:	e8 dc f4 ff ff       	callq  400b30 <__stack_chk_fail@plt>
  401654:	48 83 c4 78          	add    $0x78,%rsp
  401658:	c3                   	retq   
```

可以看出，这里先检查输入的数据是不是6行，然后重新读入这个输入流，检查有没有一个特定字符串，如果有，就跳转到secret_phase。下面是操作的步骤。

```cpp
(gdb) break * 0x4015fa
(gdb) run < a.txt
(gdb) x/s 0x603870
0x603870 <input_strings+240>:   "7 0"
(gdb) x/s 0x402619
0x402619:       "%d %d %s"
```

其实，这里我并没有分析，因为我看出来7 0恰好是第4关的答案，看来是要在7 0后面加一个特定字符串才行，这个字符串也比较好得到：

```cpp
(gdb) x/s 0x402622
0x402622:       "DrEvil
```

然后我们试着7 0后面加入DrEvil字符串，发现顺利的进入了隐藏关，下面就看看隐藏关的内容吧：

```cpp
0000000000401242 <secret_phase>:
  401242:	53                   	push   %rbx
  401243:	e8 56 02 00 00       	callq  40149e <read_line>
  401248:	ba 0a 00 00 00       	mov    $0xa,%edx
  40124d:	be 00 00 00 00       	mov    $0x0,%esi
  401252:	48 89 c7             	mov    %rax,%rdi
  401255:	e8 76 f9 ff ff       	callq  400bd0 <strtol@plt>
  40125a:	48 89 c3             	mov    %rax,%rbx
  40125d:	8d 40 ff             	lea    -0x1(%rax),%eax
  401260:	3d e8 03 00 00       	cmp    $0x3e8,%eax
  401265:	76 05                	jbe    40126c <secret_phase+0x2a>
  401267:	e8 ce 01 00 00       	callq  40143a <explode_bomb>
  40126c:	89 de                	mov    %ebx,%esi
  40126e:	bf f0 30 60 00       	mov    $0x6030f0,%edi
  401273:	e8 8c ff ff ff       	callq  401204 <fun7>
  401278:	83 f8 02             	cmp    $0x2,%eax
  40127b:	74 05                	je     401282 <secret_phase+0x40>
  40127d:	e8 b8 01 00 00       	callq  40143a <explode_bomb>
  401282:	bf 38 24 40 00       	mov    $0x402438,%edi
  401287:	e8 84 f8 ff ff       	callq  400b10 <puts@plt>
  40128c:	e8 33 03 00 00       	callq  4015c4 <phase_defused>
  401291:	5b                   	pop    %rbx
  401292:	c3                   	retq   
```

这里首先读入了一行，然后调用strtol函数，这个函数的原型如下：

```cpp
long int strtol(const char *nptr,char **endptr,int base);
```

这里的主要作用是把char *转化为long类型的，base是进制。若参数endptr不为NULL，则会将遇到不合条件而终止的nptr中的字符指针由endptr返回；若参数endptr为NULL，则会不返回非法字符串。这里把转化后的值进行了比较，要小于等于0x3e8+1，然后就调用了fun7这个函数，然后检查fun7的返回值，如果返回值等于2，就成功了。看来，这里主要还是要fun7这个函数了。

```cpp
0000000000401204 <fun7>:
  401204:	48 83 ec 08          	sub    $0x8,%rsp
  401208:	48 85 ff             	test   %rdi,%rdi
  40120b:	74 2b                	je     401238 <fun7+0x34>
  40120d:	8b 17                	mov    (%rdi),%edx
  40120f:	39 f2                	cmp    %esi,%edx
  401211:	7e 0d                	jle    401220 <fun7+0x1c>
  401213:	48 8b 7f 08          	mov    0x8(%rdi),%rdi
  401217:	e8 e8 ff ff ff       	callq  401204 <fun7>
  40121c:	01 c0                	add    %eax,%eax
  40121e:	eb 1d                	jmp    40123d <fun7+0x39>
  401220:	b8 00 00 00 00       	mov    $0x0,%eax
  401225:	39 f2                	cmp    %esi,%edx
  401227:	74 14                	je     40123d <fun7+0x39>
  401229:	48 8b 7f 10          	mov    0x10(%rdi),%rdi
  40122d:	e8 d2 ff ff ff       	callq  401204 <fun7>
  401232:	8d 44 00 01          	lea    0x1(%rax,%rax,1),%eax
  401236:	eb 05                	jmp    40123d <fun7+0x39>
  401238:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  40123d:	48 83 c4 08          	add    $0x8,%rsp
  401241:	c3                   	retq   
```

fun7这个函数确实比较难懂，但是只要提醒一下是二叉树的查找，这个问题就基本可以解决了，因为对于二叉树的查找代码，我们还是比较熟悉的。下面给出fun7的C语言代码：

```cpp
struct tree{
    int val;
    struct tree *left;
    struct tree *right;
};

typedef struct tree *Tree;

int fun7(Tree tree,int val){
    if(tree == NULL){
        return 0xffffffff;
    }
    int ret;
    int temp = tree->val;
    if(temp - val <=0){
        ret = 0;
        if(val == temp){
            return ret;
        }else{
            return 2 * fun7(tree->right,val) + 1;
        }
    }else{
        return 2 * fun7(tree->left,val);
    }
    return ret;
}
```

可以看出，要返回2，情况可以是这样的：函数第一次返回0，第二次返回2x+1,最后返回2x就可以了。也就是说，我们所要查找的树的位置位于起始二叉树的左子树的右子树。所以，我们可以通过如下的命令来找到这个值。

```cpp
(gdb) x/x 0x6030f0+8
0x6030f8 <n1+8>:        0x00603110
(gdb) x/x 0x00603110+16
0x603120 <n21+16>:      0x00603150
(gdb) x/d 0x00603150
0x603150 <n32>: 22
```
所以，最后的答案就是

```cpp
22
```

至此，整个实验就结束了。
