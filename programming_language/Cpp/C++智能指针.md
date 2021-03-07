## C++智能指针

### 智能指针的基本原理

RAII（Resource Acquisition Is Initialization）,也称为“资源获取就是初始化”，是C++语言的一种管理资源、避免泄漏的惯用法。C++标准保证任何情况下，已构造的对象最终会销毁，即它的析构函数最终会被调用。简单的说，RAII 的做法是使用一个对象，在其构造时获取资源，在对象生命期控制对资源的访问使之始终保持有效，最后在对象析构的时候释放资源。

```cpp
#include <iostream>
using namespace std;

template<typename T>
class AutoPtr{
public:
    AutoPtr(T *ptr = NULL):ptr_(ptr){}
    ~AutoPtr(){
        delete ptr_;
    }
    /* 构造和指针一样的语法 */
    T* operator->() const{
        return ptr_;
    }
    T& operator*() const{
        return *ptr_;
    }
    /* 实现拷贝构造函数和等号运算符，避免重复释放和空悬指针 */
    AutoPtr(AutoPtr& other):ptr_(other.release()){}
    AutoPtr& operator=(AutoPtr& other){
        reset(other.release());
        return *this;
    }
    T* release(){
        T* temp = ptr_;
        ptr_ = NULL;
        return temp;
    }

    void reset(T* ptr){
        if(ptr_ != ptr){
            delete ptr_;
        }
        ptr_ = ptr;
    }
private:
    T *ptr_;
};

class A{
public:
    A(int val):val_(val){
        cout<<"A::A("<<val_<<")"<<endl;
    }
    ~A(){
        cout<<"A::~A("<<val_<<")"<<endl;
    }
    void func(){
        cout<<"A::func()"<<endl;
    }
    int val_;
};


int main(int argc, char const *argv[])
{
    A *a = new A(1);
    AutoPtr<A> ptr(a);
    ptr->func();
    cout<<"A::val_ = "<<ptr->val_<<endl;
    AutoPtr<A> ptr2 = ptr;
    AutoPtr<A> ptr3(new A(2));
    ptr3 = ptr2;
    return 0;
}
```

### STL中的智能指针

