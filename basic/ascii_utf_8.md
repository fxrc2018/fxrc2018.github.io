## ASCII和UTF-8

大多数的字符串算法都是以ASCII码为例的，但在实际开发中，常常要用到UTF-8编码，这时候，需要了解一点两种编码的基础知识，才能正确的运用已有的字符串算法。

### 标准ASCII码

标准ASCII 码也叫基础ASCII码，使用7 位二进制数（剩下的1位二进制为0）来表示所有的大写和小写字母，数字0 到9、标点符号，以及在美式英语中使用的特殊控制字符。

ASCII码只是用了7位二进制数，用一个字节表示时，它第一位总是为0。UTF-8就是利用了这一特点，在兼容ASCII码的情况下对ASCII码进行了扩展。

### UTF-8码

UTF-8编码规则：如果只有一个字节则其最高二进制位为0；如果是多字节，其第一个字节从最高位开始，连续的二进制位值为1的个数决定了其编码的字节数，其余各字节均以10开头。

从UTF-8的编码规则可以看出，在UTF-8的字符串中，搜索一个英文字符串和一个中文字符串是没有问题的，因为没有重复的部分。

知道了UTF-8的编码规则，可以写一个计算UTF-8的字符数的函数。

```cpp
#include <bits/stdc++.h>
using namespace std;

int utf8Len(const string &s){
    int i = 0;
    int res = 0;
    while(i < s.size()){
        unsigned char c = s[i];
        int j = 0;
        while(j < 8 && (c & 0x80) != 0){ //查找开始的1，连续几个1表示一个字符的编码长度
            j++;
            c <<= 1;
        }
        if(j == 0){ //特殊，如果开始就是0，长度为1个字节
            j = 1;
        }
        if(j > 6){
            cout<<"eror: not a utf-8 code string"<<endl;
            return -1;
        }
        i += j;
        res++;
    }
    return res;
}

int main(int argc, char const *argv[])
{
    string s = "你好，世界，hello world";
    cout<<utf8Len(s)<<endl; //17
    //29 占用了29个字节，因为一个汉字占3位 
    //这是utf-8的一个缺点，无法直观的看出占用了多少字节
    cout<<s.size()<<endl; 
    system("pause");
    return 0;
}
```