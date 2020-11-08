## ASCII、Unicode和UTF-8

大多数的字符串算法都是以ASCII码为例的，但在实际开发中，常常要用到UTF-8编码，这时候，需要了解一点两种编码的基础知识，才能正确的运用已有的字符串算法。

### 标准ASCII码

标准ASCII 码也叫基础ASCII码，使用7 位二进制数（剩下的1位二进制为0）来表示所有的大写和小写字母，数字0 到9、标点符号，以及在美式英语中使用的特殊控制字符。

ASCII码只是用了7位二进制数，用一个字节表示时，它第一位总是为0。UTF-8就是利用了这一特点，在兼容ASCII码的情况下对ASCII码进行了扩展。

### Unicode

如果只表示英文，一个字节是够用的，但要表示世界上所有的文字，必须使用多个字节才行。Unicode就是为了能在计算机中表示所有的文字。它为每种语言中的每个字符设定了统一并且唯一的二进制编码，以满足跨语言、跨平台进行文本转换、处理的要求。

Unicode可以看做是是一本很厚的字典，记录着世界上所有字符对应的一个数字。这个字符的划分通常遵循了一些规律，利用这些规律可以用来匹配特定的字符。如，如果要匹配中文，可以通过中文的开始和结束的编号来匹配。

### UTF-8码

UTF-8编码规则：

- 对于单个字节的字符，第一位设为 0，后面的 7 位对应这个字符的 Unicode 码点。因此，对于英文中的 0 - 127 号字符，与 ASCII 码完全相同。这意味着 ASCII 码那个年代的文档用 UTF-8 编码打开完全没有问题。

- 对于需要使用 N 个字节来表示的字符（N > 1），第一个字节的前 N 位都设为 1，第 N + 1 位设为0，剩余的 N - 1 个字节的前两位都设位 10，剩下的二进制位则使用这个字符的 Unicode 码点来填充。

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

### 匹配中文
要匹配中文，只需要看一个字符是否在中文的Unicode码内。但这里最好使用宽字符，直接用Unicode表示，如果用UTF-8有问题。
```cpp
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char const *argv[])
{
    //不这样初始化wcout无法输出中文字符，具体原理还不清楚
    ios::sync_with_stdio(false);
    locale::global(locale(""));
    setlocale(LC_CTYPE, "");
    wcout.imbue(locale(""));

    wstring s1 = L"你好，世界";
    wregex p1(L"[\u4e00-\u9fa5]+"); //中文的起止编号
    wsmatch r1;
    wstring::const_iterator is = s1.begin();
    wstring::const_iterator ie = s1.end();
    while(regex_search(is,ie,r1,p1)){
        wcout<<r1.str()<<endl;
        is = r1[0].second;
    }
    wstring wstr = regex_replace(s1,p1,L"");
    wcout <<L"替换之后的字符串："<<wstr<<endl;
    system("pause");
    return 0;
}
```
如果直接使用UTF-8进行匹配，会出现一些奇怪的现象。
```cpp
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char const *argv[])
{
    system("chcp 65001"); //改变命令行的编码
    string s = "你好，世界";
    regex p("[\u4e00-\u9fa5]+");
    smatch r;
    string::const_iterator is = s.begin();
    string::const_iterator ie = s.end();
    while(regex_search(is,ie,r,p)){
        cout<<r.str()<<endl;
        is = r[0].second;
    }
    //执行到这里流出错了 输出不了
    string s1 = regex_replace(s,p,"");
    cout<<"替换之后的字符串："<<s1<<endl; //替换的结果为空，错了
    system("pause");
    return 0;
}
```
按我的理解，这种写法是行不通的，因为UTF-8需要解码才能得到Unicode，直接匹配会出错。不清楚正则匹配中\u的实现，但是直接用确实是有问题。要匹配中文，最好还是先处理成Unicode宽字符比较好。