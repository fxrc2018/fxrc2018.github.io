## 从C到C++

C++语言是以C语言为基础，对C语言进行了加强，如类型加强，函数加强和异常处理，最重要的是，C++加入了面向对象支持。

### “实用性”增强

C++中所有的变量都可以在使用时定义，这一点最大的改变就是使用for循环的时候：

```cpp
for(int i=0;i<10;i++){
	/* i的作用域在for循环内，C语言不能这样写 */
}
```

### register关键字增加

C语言中不能对register关键字声明的变量取地址，C++可以。这个关键字一般不使用，因为编译器会自动优化。

### 函数类型检测增强

C++把弱符号（未初始化的全部变量）也当做了强符号（函数和已初始化的全局变量），所以，在C语言中能通过编译的，在C++是无法通过编译的。

```cpp
#include<stdio.h>

int a;
int a = 10;
int main(int argc, char const *argv[])
{
    printf("%d\n",a); /* 强符号屏蔽弱符号 */
    return 0;
}
```	

### struct类型加强
C语言中struct定义一组变量的集合，C编译器并不认为这是一种新的类型；而C++中的struct是一个新类型的定义声明。如：

```cpp
#include <stdio.h>

struct Student
{
    char name[20];
    int age;
};

int main(int argc, char const *argv[])
{
    Student s; /* 在C语言中需要声明成 struct Student s; */
    s.age = 16;
    return 0;
}
```

此外，struct和class完成的功能是一样的。

### 三目运算符的功能增强
C语言中，三目运算符返回的是变量的值，而C++语言返回的是变量本身。所以，C语言中三目运算符的结果不可以作为左值使用，而C++中可以。下面是一个例子：

```cpp
#include <stdio.h>

int main(int argc, char const *argv[])
{
    int a = 20;
    int b = 10;
    /* 下面的写法在C语言中可以这样来实现 *(a>b?&a:&b) = 30; */
    (a > b ? a : b) = 30;
    printf("%d\n", a);
    return 0;
}
```

### const关键字增强
先看一个例子：

```cpp
#include<stdio.h>

int main(int argc, char const *argv[])
{
    /* 如果a是全局的，会导致对只读区域的写操作，导致程序出错 */
    const int a = 10;
    int *p = (int *)&a;
    *p = 20; /* 在C编译器可以修改，C++不能 */
    printf("a = %d *p = %d\n",a,*p);
    return 0;
}
```

对于const修饰的局部变量是由编译器维护的，当要去修改它的值时，编译器会察觉出来，然后报错。而不是说这块内存本身是不能修改的。在C语言中，const变量是只读变量，有自己的存储空间。在C++的实现中，有所不同，C++维护了一个符号表，当碰见常量时，在符号表中放入常量。当编译过程中若发现常量则直接以符号表中的值替换。编译过程中若发现对const使用了extern或者&操作符，则给对应的常量分配存储空间（兼容C）。所以在上面的程序中，打印a的值并不会变，变的是*p的值。
C++中的const常量类似于宏定义，但const常量是由编译器处理，提供类型检查和作用域检查，宏定义由预处理器处理，只是单纯的文本替换。

### 命名空间

命名空间为防止名字冲突提供了更加可控的机制。命名空间分割了全局命名空间，其中每个命名空间是一个作用域。通过在某个命名空间中定义库的名字，库的作者（以及用户）可以避免全局名字固有的限制。全局作用域也叫默认命名空间。
命名空间的定义格式如下：

```cpp
namespace name{ ... }
```

只要能出现在全局作用域中的声明就能置于命名空间内，主要包括：类，变量（及其初始化操作），函数（及其定义），模板和其他命名空间。命名空间可以是不连续的，声明的时候，如果不存在，就创建；如果存在，就在命名空间中添加声明。

命名空间的简单使用如下：

* 使用整个命名空间：using namespace name;
* 使用命名空间中的变量：using name::variable;
* 使用默认命名空间中的变量：直接使用。

知道了这个，对于`#include<iostream>`就可以更好理解了，这里面只是包含了命名空间的声明，所以还需要自己显示的使用`using namespace std`来使用命名空间。

### 新增bool类型关键字

C++在C语言的基本类型系统上增加了bool，C++中的bool的取值只有true和false。理论上bool只占用一个字节，如果多个bool变量定义在一起，可能会各占一个bit，这取决与编译器的实现。C++编译器会在赋值的时候将非0值转化为true，0值转化为false。

### 引用
引用为对象起一个名字，引用类型引用另一种类型。定义引用时，程序把引用和它的初值绑定在一起，而不是将初始值拷贝给引用。一旦初始化完成，引用将和它的初始值对象一直绑定在一起。因为无法令引用重新绑定到另外一个对象，因此引用必须初始化因为引用本身不是一个对象，所以不能定义引用的引用；但是引用可以赋值给引用，它们所引用的都是同一个对象。引用的语法格式如下：

```cpp
类型标识符 &引用名=目标变量名；
```

引用作为其他变量的别名而存在，因此在一些场合可以代替指针。引用相对于指针来说，具有更好的可读性和实用性。

引用在C++中的内部实现是一个常指针，但从使用角度来说，引用会让人误以为只是一个别名，没有自己的存储空间。当实参传给形参引用的时候，相当于C++编译器自动给实参取了一个地址，当我们使用指针的时候，C++编译器又会自动进行解引用。当我们使用时，并不关心编译器引用是怎么做的，但当我们分析奇怪的语法现象时，我们才会考虑C++编译器会怎么做。下面是一个例子：

```cpp
#include<iostream>
#include<string.h>
using namespace std;

struct Student{
    char name[20];
    int age;
};

ostream& operator<<(ostream &out,Student &s){
    out<<s.name<<" "<<s.age;
    return out;
}

void reference(Student &s){
    strcpy(s.name,"reference");
    s.age = 20;
}

void pointer(Student *s){
    strcpy((*s).name,"pointer");
    (*s).age = 20;
}

int main(int argc, char const *argv[])
{
    Student s1,s2;
    Student *ps = &s1;
    Student &rs = s1;
    /* 两个值是相等的 */
    printf("ps = %p rs = %p\n",ps,&rs);
    reference(s1);
    pointer(&s2);
    cout<<s1<<"\n"<<s2<<endl;
    return 0;
}
```

函数返回值是引用的情况：

* 栈变量：不能成为其他引用的初始值；也不能作为左值使用，可以作为右值。
* 静态变量或全局变量：可以作为其他引用的初始值；也可以作为左值使用，也可以作为右值使用。

这里结合函数的调用规则，和C++引用的实现，就很容易理解这一点。
引用也可以引用指针对象，这是里所当然的。只不过引用指针的格式看起来有点怪，可能有点不好理解，下面是一个例子：

```cpp
#include<iostream>
#include<string.h>
using namespace std;

struct Student{
    char name[20];
    int age;
};

ostream& operator<<(ostream &out,Student &s){
    out<<s.name<<" "<<s.age;
    return out;
}

void reference(Student &s){
    strcpy(s.name,"reference");
    s.age = 20;
}

void pointer(Student *s){
    strcpy((*s).name,"pointer");
    (*s).age = 20;
}

int main(int argc, char const *argv[])
{
    Student s1,s2;
    Student *ps = &s1;
    Student &rs = s1;
    /* 两个值是相等的 */
    printf("ps = %p rs = %p\n",ps,&rs);
    reference(s1);
    pointer(&s2);
    cout<<s1<<"\n"<<s2<<endl;
    return 0;
}
```

如果想要被调用函数不修改引用的值，可以将其声明为常引用，即用const修饰的引用。普通引用和常引用的区别可以很方便地使用对应的指针模型说明。下面是具体的声明：

```cpp
int &a =b;/* 相当于int * const a = &b; */
const int &a = b;/* 相当于 const int * const a = &b; */
```

值得说明的是，可以使用常量对const引用进行初始化，C++编译器会为常量值分配空间，并把引用名作为这段空间的别名，这时候，会生成一个只读变量，如：

```cpp
const int &a = 40; /*  如果去掉const将无法通过编译 */
```

### inline内联函数
在C语言中可以使用宏代码片段来实现一个函数，来避免函数调用的开销。C++中增加了inline关键字，用于声明函数，C++编译器会直接将函数体插入在函数调用的地方。如：

```cpp
inline void fun(){...}
```

和C语言的宏代码片段比起来，C++的内联函数有一下的一些优点：

* 内联函数是对编译器的一种请求，因此编译器可能拒绝这种请求。
* 内联函数由编译器处理，直接将编译后的函数体插入调用的地方。而宏代码片段由预处理器处理，进行简单的文本替换，没有任何编译过程。


### 函数默认参数和占位参数
C++可以在函数声明时为参数提供一个默认值，当函数调用时没有指定这个参数的值，编译器会自动用默认值代替。函数默认参数有如下的规则：

* 只有参数列表后面部分的参数才可以提供默认参数。
* 一旦在一个函数调用中开始使用默认参数，那么这个参数后面必须使用默认参数后面的值。

占位参数只有参数类型声明，而没有参数名声明。一般情况下，在函数内部无法使用占位参数。

### 函数重载

函数重载：用同一个函数名定义不同的函数，当函数名和不同的参数搭配时函数的含义不同。函数重载至少满足下面的一个条件：

* 参数个数不同。
* 参数类型不同。
* 参数顺序不同。

值得注意的是，函数的返回值不是函数重载的判断标准。

函数重载的调用准则：

* 将所有同名函数作为候选者，尝试寻找可行的候选函数。
* 精确匹配实参。
* 通过默认参数能够匹配实参。
* 通过默认类型转换匹配实参。
* 匹配失败，导致编译失败。
* 最终寻找到的可行候选函数不唯一，则出现二义性，编译失败。

函数重载的注意事项：

* 重载函数在本质上是相互独立的同步函数（静态链编）。
* 重载函数的函数类型是不同的。
* 函数重载是发生在一个类里面的。

当把重载函数赋给函数指针时，会严格与函数参数匹配。下面是一个例子：

```cpp
#include<iostream>
using namespace std;

typedef void (*Fun)(int a);

void func(int a){
    cout<<"call func(int a)"<<endl;
}

void func(int a,int b = 1){
    cout<<"call func(int a,int b =1)"<<endl;
}

void func(double a,double b,char c = 'a'){
    cout<<"call func(double a,double b,char c = 'a')"<<endl;
}

int main(int argc, char const *argv[])
{
    Fun fun = func;
    fun(1);/* 严格匹配，能通过编译 */
    //func(1);/* 无法通过，和默认参数的重载重复 */
    func(1,2);/* 匹配默认参数 */
    func(1.2,3.4);/* 匹配double类型 */
    func(1,2,'a');/* 类型转化进行匹配 */
    system("pause");
    return 0;
}
```

### external "C"

函数重载的实质其实是名字改编，也就是在编译的时候，系统会把函数的名字进行改编，变成一个独一无二的名字。请看下面一个程序。

```cpp
#include<iostream>

void func(int a);
void func(int a,int b){}

int main(){
	func(1);
    return 0;
}
```
这个程序在VS中编译，会提示这样的错误。

```cpp
无法解析的外部符号 "void __cdecl func(int)" (?func@@YAXH@Z)，该符号在函数 main 中被引用
```

如果在第一个func函数的前面加上extern "C"，错误提示会变成下面这样.

```cpp
无法解析的外部符号 func，该符号在函数 main 中被引用
```

这说明了两个问题，第一，C++编译器会对函数名进行改编。第二，extern "C"会阻止这种改编，这一点主要是为了和C语言兼容，因为如果C++使用了函数名改编，那么C语言就无法找到对应的函数，链接就会出错。为了兼容C和C++，一般的头文件会这样写。

```cpp
#ifdef __cplusplus
extern "C"{
#endif

/* 函数声明 */

#ifdef __cplusplus
}
#endif
```

### 新的类型转换运算符

C++中新增了4个类型转换符,它们的参数和作用如下：

* const_cast<T> (expr) ：用于指针或引用，用于去除const限定，通常不是为了修改它所指向的内容，而是为了函数能够接受实参。
* static_cast<T>(expr) ：用于算术类型较大的转化为较小的，以及基类指针转化为派生类指针。
* reinterpret_cast<T>(expr) ：用于在任意指针或引用类型之间的转换，以及指针与足够大的整数类型之间的转换。
* dynamic_cast<T>(expr) ：用于基类指针或引用安全地（能够判断是否能够转化成功）转化为派生类的指针或引用。

下面是一个例子。

```cpp
#include <iostream>
using namespace std;

void print(char *name){
    cout<<name<<endl;
}

int main(int argc, char const *argv[])
{
    const char *name = "LiMing";
    print(const_cast<char *>(name));
    double a = 123.456;
    int b = static_cast<int>(a);
    cout<<b<<endl;
    long long c = reinterpret_cast<long long>(&a);
    cout<<c<<endl;
    return 0;
}
```
