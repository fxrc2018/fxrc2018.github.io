## C语言简单实现面向对象思想

C语言和C++都会经过汇编，生成汇编代码，在汇编代码的阶段，是分辨不出是C语言还是C++语言的。在早期C++还没有成熟的编译器的时候，C++通过先把程序翻译成C语言，然后使用C语言的编译器把程序编译成可执行文件。所以，面向对象只是一种思想，在不支持面向对象的语言中也可以使用这种思想。只不过，在C语言中，需要自己实现这些特性，而在C++中，是编译器在幕后做了一些工作。

### 数据抽象
数据抽象可以将类的接口和实现分开。C语言中有struct来定义新的数据类型，但没有实现数据类型和操作的绑定，要实现操作和数据类型的绑定，首先在传入参数的时候，需要把struct的地址传进去，然后把所有的操作都变成函数指针存储在结构体中。以一个简单的类为例：

```cpp
#ifndef _POINT_H_
#define _POINT_H_
typedef struct point* Point;
struct point{
    int x;
    int y;
    double (*distance)(Point p1);
    void (*print)(Point p);
};

double distance(Point p1);
Point create_point(int x,int y);
void print_point(Point p);

#endif
```

```cpp
#include "point.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

void print_point(Point p){
    printf("x = %d, y = %d.\n",p->x,p->y);
}

double distance(Point p1){
    return sqrt(pow(p1->x,2) + pow(p1->y,2));
}

Point create_point(int x,int y){
    Point p = (Point)malloc(sizeof(struct point));
    p->x = x;
    p->y = y;
    p->distance = distance;
    p->print= print_point;
    return p;
}

int main(int argc, char const *argv[])
{
    Point p = create_point(1,1);
    p->print(p);
    printf("p distance = %f\n",p->distance(p));
    free(p);
    return 0;
}
```
这样，对象的创建和方法的调用就有一点面向对象的感觉了。

### 继承
使用继承，可以定义相似地的类型并对其关系建模。继承就是对类的扩展，为了能够实现兼容，派生的类必须有基类的所有成员和函数。然后可以在其基础上增加一些功能，也可以改变原有的功能。这里我简单的把基类的函数指针和成员放在派生类的前面。

```cpp
#ifndef _CIRCLE_H_
#define _CIRCLE_H_
#include "point.h"
typedef struct circle* Circle;
struct circle{
    int x;
    int y;
    double (*distance)(Point p1);
    void (*print)(Circle c);
    int r;
    double (*area)(Circle c);    
};
Circle create_circle(int x,int y,int r);
double area(Circle c);

#endif
```

```cpp
#include "circle.h"

void print_circle(Circle c){
    printf("x = %d, y = %d, r = %d.\n",c->x,c->y,c->r);
}

Circle create_circle(int x,int y,int r){
    Circle c = (Point)malloc(sizeof(struct circle));
    c->x = x;
    c->y = y;
    c->distance = distance;
    c->print = print_circle;
    c->r = r;
    c->area = area;
    return c;
}

double area(Circle c){
    return 3.14 * c->r * c->r;
}

int main(int argc, char const *argv[])
{
    Circle c = create_circle(1,1,2);
    printf("c distance = %f\n",c->distance(c));
    c->print(c);
    printf("c area = %f\n",c->area(c));
    return 0;
}
```
这里可以发现，把point的成员放在前面，然后在初始化的时候，将函数指针赋值为point的函数地址，如果覆写了，就改为改过之后的函数地址。可以发现，能够调用原来的函数，也能够调用新增的函数。

### 动态绑定
动态绑定，可以在一定程度上忽略相似类型的区别，而以统一的方式使用它们的对象。看下面一个例子：

```cpp
int main(int argc, char const *argv[])
{
    Circle c = create_circle(1,1,2);
    Point p = (Point)c;
    p->print(p);
    printf("p distace = %f\n",p->distance(p));
    return 0;
}
```
可以发现，可以把Circle转化为Point，并且可以正常的调用Point的方法。这里其实很简单，强制装换之后，其实和没有装换是没太大区别的，除了不能访问其扩展的成员。其实动态绑定不在调用的时候，而在初始化的时候，print函数在初始化的时候就变成了新的地址，所以这里调用的时候，会使用Circle的函数。

### 总结
最近在看《深度探索C++对象模型》，一方面感觉C++编译器的实现确实很巧妙，一方面又感觉C语言确实小巧精悍，即使不用C++，C语言本身也可以实现面向对象的东西。所以写了这一篇笔记，加深一下自己对面向对象的理解。个人水平有限，举的例子也许有一些问题，望大家见谅。
