##类加载机制
先看这样一段程序。
```
class A{
    public static A a = new A();
    static {
        System.out.println("A的静态块");
    }
    {
        System.out.println("A的构造块");
    }
    A(){
        System.out.println("A的构造函数");
    }
}

class B extends A{
	public static B b = new B();
    static {
        System.out.println("B的静态块");
    }
    {
        System.out.println("B的构造块");
    }
    B(){
        System.out.println("B的构造函数");
    }
}

public class Init {
	public static void main(String[] args) {
		B b = new B();
	}
}
```
输出的结果为
```
/* public static A a = new A()触发 */
A的构造块 
A的构造函数

/* public static B b = new B()触发 */
A的静态块
A的构造块
A的构造函数
B的构造块
B的构造函数

/* B b = new B() */
B的静态块
A的构造块
A的构造函数
B的构造块
B的构造函数
```

jvm把class文件加载到内存，并对数据进行校验、解析和初始化，最终形成jvm可以直接使用的java类型的过程，有三个步骤，加载，链接和初始化。
1. 加载
将class文件字节码内容加载到内存中，并将这些静态数据转换成方法区中的运行时数据结构，在堆中生成一个代表这个类的java.lang.Class对象，作为方法区类数据的访问入口。
2. 链接
链接 将java类的二进制代码合并到jvm的运行状态之中的过程。
 验证：确保加载的类信息符合jvm规范，没有安全方面的问题。
 准备：正式为类变量（static变量）分配内存并设置类变量初始值的阶段，这些内存都将在方法区中进行分配。
 解析：虚拟机常量池内的符号引用替换为直接引用的过程。（比如String s ="aaa",转化为 s的地址指向“aaa”的地址）。
3. 初始化
初始化阶段是执行类构造器方法的过程。类构造器方法是由编译器自动收集类中的所有类变量的赋值动作和静态语句块（static块）中的语句合并产生的。
当初始化一个类的时候，如果发现其父类还没有进行过初始化，则需要先初始化其父类的初始化。
虚拟机会保证一个类的构造器方法在多线程环境中被正确加锁和同步。
当访问一个java类的静态域时，只有真正声明这个静态变量的类才会被初始化。

对于上面的结果，我的理解是，如果去掉A类和B类中的静态变量时，会得到以下的结果：
```
A的静态块 //只触发一次
B的静态块
A的构造块 //每次构造都触发
A的构造函数
B的构造块
B的构造函数
```
这就是正常的构造顺序，那么，为什么构造A的静态变量时没有触发A的静态方法呢，我个人的想法是，在A类初始化自己，不算访问一个Java类的静态域，因为这是在分配内存的同时初始化内容。如果我们在A中加入一个int型的静态变量，初始化为3，我们在静态块中打印，会发现，打印的时候，该变量已经正确初始化了。

这些只能算我对于这种语法现象的个人理解吧，个人观点，仅供参考。

##在构造函数中使用虚函数的情况

我先学了C语言，然后学了Java，后来做算法题的时候，学了C++，现在又复习Java。在我看来，Java和C++在很多地方都很相似，但也有很多地方不同，今天要说的就是在构造函数中调用虚函数的情况，也就是子类覆写了一个会在构造函数中调用的情况。在C++中，这种情况是没有意义的，因为C++只会调用本类的方法，而不会调用子类的方法。但Java有一点不同，它可以调用子类的方法，但是，由于初始化顺序的关系，会有一些比较奇怪的现象。先看下面一段程序。

```
class A{
	private String name = "class A";
	
	public A(){
		func();
	}
	
	void func() {
		System.out.println("A:"+name);
	}
}

class B extends A{
	
	private String name = "class B";
	
	@Override
	void func() {
		System.out.println("B:"+name);
	}
}

public class Test {  
	
	public static void main(String[] args) {
		new B();
	}
	
}  
```
这里的输出为：
```
B:null
```
从所以会出现这个结果，是因为初始化B的时候，要先初始化A，而初始化A，需要调用B的func函数，而此时，B还没有完全初始化，所以name的值为null。
