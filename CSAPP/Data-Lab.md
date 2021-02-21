## Data Lab

这个实验就是完成15个函数，这些函数在我看来更像是智力测试，因为如果不限定操作符的话，这些这些函数根本不用自己实现。但是这个实验还是很有意义的，因为它会使你对数据的表示方法有更深的理解，此外还涉及一些奇怪的算法，如求log中，手动的二分查找；在求1的个数中的手动分组求解的方法；这些是很值得学习的。下面是题目和解析，有一些题目参考了很多网上的资源。

```cpp
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
    return ~(~x|~y);
}
```

这个题考的是布尔代数中的一个公式，如果不知道公式，也可以凭感觉想，也是可以想出来的。下面使用公式的推导过程：

$$
A\&B = \overline{\overline{A\&B}} = \overline{\overline A | \overline B}
$$

```cpp
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
    return (x>>(n<<3))&0xff;
}
```

要得到低8位，肯定是把一个数和0xff进行相与。主要是要左移n*8位，刚开始的是时候我写的是x左右n位8次，但这样不行，看了别人的才反应过来可以这样写，主要考的是，乘以一个常数可以用位移来实现。

```cpp
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
  /* x为0的时候，进行两次位移 */
  int mask = ~(~0<<(32 + ~n)<<1);
  return (x>>n)&mask;
}
```
这个题目的主要思路是先进行算术右移，然后把位移的部分置为0。这里有两个注意点，第一是不能使用减号，要实现减法只能使用加法实现，变成负数的操作就是取反加1。第二是要注意n为0的情况，会导致移位的位数等于数据长度，这个操作时没有定义的，只能使用两次位移来避免这个事情。

```cpp
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
  int mask1 = (0x55)|(0x55<<8);
  int mask2 = (0x33)|(0x33<<8);
  int mask3 = (0x0f)|(0x0f<<8);
  int mask4 = 0xff;
  int mask5 = ~(~0<<16);
  mask1 = mask1 | (mask1<<16);
  mask2 = mask2 | (mask2<<16);
  mask3 = mask3 | (mask3<<16);
  mask4 = mask4 | (mask4<<16);
  
  x = (x&mask1) + ((x>>1)&mask1);
  x = (x&mask2) + ((x>>2)&mask2);
  x = (x&mask3) + ((x>>4)&mask3);
  x = (x&mask4) + ((x>>8)&mask4);
  x = (x&mask5) + ((x>>16)&mask5);
  return x;
}
```

这个题目确实很难，个人感觉没有提示是做不出来的，或者说，要想出来，需要花的时间很长。其实我想的下一题的解法也有这种思想在里面：

```cpp
int bang(int x){
  x = x<<16 | x;
  x = x<<8 | x;
  x = x<<4 | x;
  x = x<<2 | x;
  x = x<<1 | x;
  return (x^0x1)&0x1;
}
```

这个主要的思想就是通过或运算，把含有1的位移到最右边。上面的思想是，对含有1的位进行二分相加，这里使用掩码来达到一些技巧：掩码和位移把整数的加法，变成并行的，几位之间的加法。如第一次，就是16个加法，分别是相邻位之间的加法，并且，掩码的使用可以达到加法可以有进位，不溢出的功能。

```cpp
/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
  int y = (~x+1)^x;
  return (~((y>>31) | (x>>31)))&0x1;
}
```

这个题利用了一个重要的特性，0的相反数（取反加1）是其本身，相同的数异或结果为0。但是，这里有一个比较烦的数字，就是最小的整数，它的相反数也是其本身。这里，就要区分这两种情况。注意到，如果不是0或最小的整数，y的首位肯定是1（一正一负相与），如果是最小数，x的首位是1，所以，通过这两个条件，就可以选择出0。

```cpp
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return (~0)<<31;
}
```

这个题目比较简单，返回最小的整数，也就是0x80000000。

```cpp
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
  int m = 33 + ~n;
  int y = (x<<m)>>m;
  return !(x^y);
}
```

这个题的基本思路，就是将数据截断后，和原来的表示方法仍然一致。但是要注意的是，如果是正数，可能出现没有符号位的情况，为了判别这种情况，需要把截断的数进行符号扩展，然后和原来的数进行比较即可。

```cpp
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
  int add = x>>31 & ~(~0<<n);
  return (x + add)>>n;
}
```

这个参看教材的74页，这里比教材增加的地方是需要对正数和负数进行判断。

```cpp
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x+1;
}
```

这个比较简单，就是取反加1。

```cpp
/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
  int y = ((~x+1)^x)>>31;  /* 0最低位0，其他，最低位1 */
  return (~(x>>31)&y)&0x1; /* ~(x>>31)负数最低位0，其他最低位1，然后与上y，得到结果 */
}
```
这道题的难度是需要对0进行特殊处理，因为有符号位，可以把一个数左移31位，这样就可以判断是否是负数。但是，如果不是负数，还有可能是0和正数，需要把0的情况去掉。0有一个特殊的属性，0的相反数还是0,。利用这个属性，可以区别出0和其他数。

```cpp
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */

int isLessOrEqual(int x, int y) {
  int signx = x>>31;
  int signy = y>>31;
  /* 符号相同 */
  int sign_equal = (!(signx^signy)) & ((x + (~y))>>31);
  int sign_not = signx & (!signy);
  return sign_equal | sign_not;
}
```

```cpp
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
  /* 基本思路，手动二分法,找到第一个为1的位置 */
  /* 到中间,偏移16位 */
  int first = (!!(x>>16))<<4;
  /* 偏移8位 */
  first = first + ((!!(x>>(first + 8)))<<3);
  first = first + ((!!(x>>(first + 4)))<<2);
  first = first + ((!!(x>>(first + 2)))<<1);
  first = first + (!!(x>>(first + 1)));
  return first;
}
```

```cpp
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
  unsigned int neg = 0x80000000;
  unsigned int mask = 0x7f800000;
  if((mask&uf) == mask && (uf&0x007fffff)!=0){
    return uf;
  }else{
    
    return uf^neg;
  };
}
```

```cpp
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
  unsigned int after_shift = x;
  unsigned int shift_number = 0;
  unsigned int temp;
  unsigned int sign = 0;
  unsigned int flag;
  if(x == 0){
    return 0;
  }
  /* 确定符号 */
  if(x < 0){
    sign = 0x80000000;
    x = -x;
    after_shift = x;
  }
  /* 确定阶数 */
  while(1){
     temp = after_shift;
     after_shift <<= 1;
     shift_number++;
     if((temp&0x80000000)){
       break;
     }
  }
  shift_number = 32 - shift_number;
  /* 确定舍入条件，这里写得相当好，可惜是别人写的 */
  if((after_shift&0x1ff)>0x100){
    flag = 1;
  }else if((after_shift&0x3ff)==0x300){
    flag = 1;
  }else{
    flag = 0;
  }

  return (sign) + ((127+shift_number)<<23) + (after_shift>>9) + flag;
}
```

```cpp
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
 /* 分类讨论,规格化的数，和非规格化的数 */
 unsigned int frac = 0x007fffff;
 unsigned int exp = 0x7f800000;
 unsigned int result = uf;

/* 非规格化的数 */
 if((uf&exp) == 0){
    /* 把小数部分乘以2,需要考虑进位的情况 */
    result = (result&frac)<<1;
    result = (uf&(~frac)) | (result);
    /* 规格化的数 */
 }else if((uf&exp)!=exp){
   /* 把指数部分加1 */
   result = result + 0x00800000;
 }
 return result;
}
```
