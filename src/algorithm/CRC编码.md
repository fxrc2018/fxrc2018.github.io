## CRC的基本原理

CRC编码也称为多项式编码，因为该编码能够将要发送的比特串看做系数是0和1一个多项式，对比特串的操作被解释为多项式算术。

CRC编码操作如下。考虑d比特的数据D，发送结点要将它发送给接收结点。发送方和接收方首先必须协商一个r+1比特模式，称为生成多项式，将其表示为G。生成多项式G的最高有效位的比特必须为1。对于一个给定的数据段D，发送方要选择r个附加比特R，并将它们附加到D上，使得得到的d+r比特模式用模2算术恰好能被G整除。用CRC进行差错检测的过程也很简单，重新计算附加比特R，如果和接到到的附加比特R相同，则认为数据被正确接收了；否则认为数据出现了差错。

所有CRC计算采用模2算术来做，在加法中不进位，在减法中不借位。这意味着加法和减法是相同的，而且这两种操作等价于按位异或。R的计算公式为：

$$
R = remainder\frac{D\cdot 2^r}{D}
$$

后面的例子都已CRC32为例，CRC32的生成多项式为：

$$
x^{32}+ x^{28}+ x^{27}+ x^{26}+ x^{25}+ x^{23}+ x^{22}+ x^{20}+ x^{19}+ x^{18}+ x^{14}+ x^{13}+ x^{11}+ x^{10}+ x^9+ x^8+ x^6+ x^0
$$

因为CRC的生成多项式的最高位都为1，并且系数都为0或1，所以可以用一串比特来表示这个多项式，并且最高位的1在表示中可以省略的，为了方便，一般写成16进制，所以CRC32的生成多项式的表示形式为100011110110111000110111101000001，对应的16进制表示为0x11EDC6F41，省略最高位表示为0x1EDC6F41。

计算的结果都使用下面这个在线的CRC计算器来检验
[http://www.ghsi.de/pages/subpages/Online%20CRC%20Calculation/](http://www.ghsi.de/pages/subpages/Online%20CRC%20Calculation/)。

### 直接计算法

直接计算法就是模拟多项式除法。先看看计算一个字节的CRC码怎么办。

```cpp
static uint32_t POLYNOMIAL = 0x1EDC6F41;
/* 计算数据左移8位之后的crc的值 */
uint32_t crc32_byte(uint32_t data){
    uint32_t crc = data << 24; //先左移r-8位
    for(int i=0;i<8;i++){ /* 每次左移相当于末尾补0，一共补了8个0 */
        if((crc & 0x80000000) != 0){ /* 商x的n次方，需要做一次减法 */
            crc <<= 1; //因为生成多项式省略了最高位1，所以需要左移移位，相当于减去了最高位
            crc ^= POLYNOMIAL;
        }else{ /* 商0 */
            crc <<= 1;
        }
    }
    return crc;
}
```

相信上面的代码还是很好理解的，但问题在于，如果想要计算多个字节的CRC值，上面这种方法是不好操作的，因为要进行左移一位后面，要根据后面的值，决定补0还是1，所以不好操作。这里，我们可以利用CRC计算的一个性质，也就是说，如果D为0x0102，那么R可以等于0x0100的R加上0x02的R。并且，如果我们知道了0x01的R，要求0x0100的R，其实可以根据0x01的R来求，并且计算也很简单，因为后面补0，就和前面的操作类似。

多个字节的操作也是类似的，也就是说，我们找到了一种可以扩展字节的方法，也就是说，我们知道数据D的R，然后需要求D后面加一个字节R，我们找到了这样一个规律。根据这个规律，实际上我们就可以写出代码了。

```cpp
#define INIT_VALUE 0x0
#define FINAL_VALUE 0x0

uint32_t crc32_byte2(uint32_t data){
    uint32_t crc = data;
    for(int i=0;i<8;i++){ /* 每次左移相当于末尾补0，一共补了8个0 */
        if((crc & 0x80000000) != 0){ /* 商x的n次方，需要做一次减法 */
            crc <<= 1; //因为生成多项式省略了最高位1，所以需要左移移位，相当于减去了最高位
            crc ^= POLYNOMIAL;
        }else{ /* 商0 */
            crc <<= 1;
        }
    }
    return crc;
}

uint32_t crc32_naive(uint8_t *p_buf, uint8_t *p_end){
    uint32_t crc = INIT_VALUE;
    uint32_t data;
    for (;p_buf < p_end;p_buf++){
        data = ((uint32_t)(*p_buf)) << 24;
        /* 原来的CRC和现在的CRC之间的关系是 原来CRC左移8位之后的余项 + 新的数据的CRC */
        crc = crc32_byte2(crc) ^ crc32_byte2(data);
    }
    return crc ^ FINAL_VALUE;
}
```

### 查表法

上面的代码其实效率不是很快，我们发现我们做了很多重复计算，因为我们每次都需要去计算一个字节的R值，我们很自然的想到，我们可以用一个表来存储这个结果，这样就可以减少很多重复计算。所以，可以改为下面的代码。

```cpp
static int isTableInit = 0;
static uint32_t table[256];

static void initTable(){
    isTableInit = 1;
    uint32_t crc;
    for (int i = 0; i < 256; i++){
        crc = i<<24;
        for (int j = 0; j < 8; j++){
            if ((crc & 0x80000000) != 0){
                crc = POLYNOMIAL ^ (crc << 1);
            }else{
                crc = crc << 1;
            }
        }
        table[i] = crc;
    }
}

uint32_t crc32_table(uint8_t *p_buf, uint8_t *p_end){
    if (isTableInit == 0){
        initTable();
    }
    uint32_t crc = INIT_VALUE;
    while (p_buf < p_end){
        /* 和直接求解类似，只不过把crc拆成了两部分，
           变为了FF000000和00FFFFFF的形式，其中，后面一个的CRC值就是其本身，而前面的值可以查表得到 */
        uint32_t data = (uint32_t)*p_buf++;
        crc = (crc << 8) ^ table[(crc>>24) & 0x000000FF] ^ table[data];
        // 根据计算的性质，上面的语句可以改写为下面的语句
        // crc = (crc << 8) ^ table[( (crc>>24) ^ data) & 0x000000FF];
    }
    return crc ^ FINAL_VALUE;
}
```

### 正向校验和反向校验

有时候一个字节的表示是相反的，如果还是按照原来的算法来做的话，比较复杂，因为需要先把每个比特翻转过来，所以就出现了一种反向的校验的方式，这种方式避免了对每个比特的翻转。这里可能不好理解，我个人认为需要使用递推的思想去理解这个代码，就很好理解了。

```cpp
static uint32_t POLYNOMIAL_REVERSE = 0x82F63B78; //crc32生成多项式的翻转
static int isTableInit2 = 0;
static uint32_t table2[256];

static void initTable2(){
    isTableInit2 = 1;
    uint32_t crc;
    for (int i = 0; i < 256; i++){
        crc = i;/* 和前面的类似，只是方向相反 */ 
        for (int j = 0; j < 8; j++){
            if ((crc & 0x01) != 0){
                crc = POLYNOMIAL_REVERSE ^ (crc >> 1);
            }else{
                crc = crc >> 1;
            }
        }
        table2[i] = crc;
    }
}

static uint64_t bitsReverse(uint64_t value,int len){
    if(len <= 0 || len >= 64){
        return value;
    }
    uint64_t high = ((uint64_t)0x1) << (len - 1);
    uint64_t low = (uint64_t)0x1;
    uint64_t res = 0;
    for(int i=0;i<len;i++){
        if( (value & high) != 0 ){
            res |= low;
        }
        high >>= 1;
        low <<= 1;
         
    } 
    return res;
}

uint32_t crc32_table2(uint8_t *p_buf, uint8_t *p_end){
    if (isTableInit2 == 0){
        initTable2();
    }
    uint32_t crc = INIT_VALUE;
    while (p_buf < p_end){
        /* 可以看出，还是把左边的数据看做高位，只不过每个字节的比特是相反的 */
        uint32_t data = (uint32_t)*p_buf++;
        crc = (crc >> 8) ^ table2[crc & 0x000000FF] ^ table2[data];
        // 根据计算的性质，上面的语句可以改写为下面的语句
        // crc = (crc >> 8) ^ table2[( crc ^ data) & 0x000000FF];
    }
    crc = (uint32_t)bitsReverse(crc,32);
    return crc ^ FINAL_VALUE;
}
```
这个算法的主要思想是，翻转多项式，而不是翻转数据，这样可以提高效率，因为数据可能会比较多。

### 简单的测试代码

```cpp
int main(int argc, char const *argv[])
{
    /* 测试数据0001020304050607 b4ffce3e*/
    const int n = 8;
    uint8_t buf[n];
    for (int i = 0; i < n; i++){
        buf[i] = (uint8_t)i;
    }
    printf("%x\n",crc32_naive(buf,buf+n));
    printf("%x\n",crc32_table(buf,buf+n));

    for (int i = 0; i < n; i++){
        buf[i] = (uint8_t)bitsReverse(i,8);
    }    
    printf("%x\n",crc32_table2(buf,buf+n));
    system("pause");
    return 0;
}
```
