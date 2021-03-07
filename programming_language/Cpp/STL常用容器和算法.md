## STL常用容器和算法

主要是方便查阅，每次都要去网上找感觉比较麻烦，作为笔记写在这里。这里是我在做算法题的时候，需要去查阅的STL中的容器和算法，所以，最常用的这里没有列举。

### string

```cpp
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

int main(int argc, char const *argv[])
{
    string s;
    // getline(cin,s); //读取一行，以换行作为分隔符
    // cin>>s; //读取一个字符串，以空格、换行作为分隔符
    int num = 12345678;
    s = to_string(num); //数字转字符串

    int p1 = s.find("3",0); //字符串查找
    int p2 = s.find('4',0);
    if(p1 != string::npos && p2 != string::npos){
        s = s.substr(p1,p2 - p1 + 1);
        cout<<s<<endl; //截取字符串
    }
    num = stod(s); //字符串转数字 num = 34

    /* 读取一个表达式中数字 */
    string expression = "7+3*4*5+2+4-3-1";
    istringstream iss(expression);
    ostringstream oss; 
    while(!iss.eof()){
        char c = iss.peek(); //查看第一个字符，但不读出
        // iss.putback(c); //可以将一个字符压回到输入流
        if(c >= '0' && c <= '9'){
            int tmp;
            iss>>tmp;
            oss<<tmp<<" ";
        }else{
            iss>>c;
        }
    }
    cout<<oss.str()<<endl;
    return 0;
}
```

#### 正则表达式

遇到字符串的问题，特别是匹配的问题，可以先考虑能否用正则表达式处理。个人感觉，正则表达式算是一个处理字符串神器。

使用正则表达式需要使用头文件regex，其中，有以下常用组件。

* regex 表示有一个正则表达式的类
* regex_match 将一个字符序列与一个正则表达式匹配
* regex_search 寻找第一个与正则表达式匹配的子序列
* regex_replace 使用给定格式替换一个正则表达式
* sregex_iterator 迭代器适配器，调用regex_search来遍历一个string中所匹配的子串
* smath 容器类，保存在string中搜索的结果

```cpp
#include <iostream>
#include <string>
#include <regex>
using namespace std;


int main(int argc, char const *argv[])
{
    /* 检查一个字符串是否为一个有效的ip地址 regex构造时可能会抛出异常*/
    regex ip("^((25[0-5]|2[0-4]\\d|[1]{1}\\d{1}\\d{1}|
        [1-9]{1}\\d{1}|\\d{1})($|(?!\\.$)\\.)){4}$");
    vector<string> v = {"127.0.0.1","127.0.1","256.0.0.0"};
    for(int i=0;i<v.size();i++){
        if(regex_match(v[i],ip)){
            cout<<v[i]<<" ";
        }
    }
    cout<<endl;

    /* 找出字符串中所有的数字 */
    string s = "abc123i2cd34jihle233";
    regex num("[0-9]+");
    sregex_iterator it(s.begin(),s.end(),num),end;
    for(;it != end;++it){
        cout<<it->str()<<" ";
    }
    cout<<endl;

     /* 找出字符串中的第一个数字 */
    smatch results;
    if(regex_search(s,results,num)){
        cout<<results.str()<<endl;
    }

    /* 去掉字符串中的数字 */
    string s1 = regex_replace(s,num,""); //这个函数还有更高级的用法
    cout<<s1<<endl;
    return 0;
}
```

### vector

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main(int argc, char const *argv[])
{
    vector<int> v;
    for(int i=0;i<10;i++){
        v.push_back(i);
        /* size:存储的数据的大小 capacity:分配空间的大小 */
        cout<<v.size()<<" "<<v.capacity()<<endl;
    }

    /* 可以利用vector的push_back和pop_back方法当做栈使用 */
    /* 在迭代中删除元素 */
    vector<int>::iterator it = v.begin();
    while(it != v.end()){
        if(*it % 2 == 1){
            it = v.erase(it);
        }else{
            ++it;
        }
    }

    for(int i=0;i<v.size();i++){
        cout<<v[i]<<" ";
    }
    cout<<endl;
    return 0;
}
```

### queue、dequeue和priority_queue

```cpp
#include <iostream>
#include <queue>
using namespace std;

struct cmp{
    bool operator()(int a,int b){
        return a < b;
    }
};

int main(int argc, char const *argv[])
{
    deque<int> deq;
    deq.push_front(2);
    deq.push_back(1);
    deq.pop_back();
    deq.pop_front();
    deq.front();
    deq.back();
    priority_queue<int,vector<int>,cmp> pq;
    pq.push(1);
    pq.pop();
    pq.top();
    return 0;
}
```

### set和map
set和map在刷题的过程中，主要实现动态地查找和更新。个人感觉需要掌握的就是插入、删除，查找和更新。

```cpp
#include <iostream>
#include <set>
#include <map>
using namespace std;

int main(int argc, char const *argv[])
{
    map<string,int> word;
    for(int i=0;i<10;i++){
        string s;
        char c = 'A' + i;
        s += c;
        word[s] = 1; //如果有，就赋值，没有，就插入新的记录
    }
    if(word.find("A") == word.end()){
        word.insert(make_pair("A",1)); // insert插入时，如果关键字在记录中，什么也不做
    }else{
        word["A"] ++;
    }
    word.erase("B");
    for(map<string,int>::iterator i = word.begin();i != word.end();++i){
        cout<<i->first<<" "<<i->second<<endl;
    }

    set<int> mark;
    for(int i=0;i<10;i++){
        mark.insert(i);
    }
    for(set<int>::iterator i = mark.begin();i != mark.end(); ){
        if(*i % 2 == 1){
            i = mark.erase(i);
        }else{
            i++;
        }
    }
    for(set<int>::iterator i = mark.begin();i != mark.end(); ++i){
        cout<<*i<<" ";
    }
    cout<<endl;
    return 0;
}
```



### 常用算法

#### lambd表达式
C++11支持lambd表达式，个人感觉写算法的时候，还是这个最好用。因为以前使用的是Java，对于不能创建一个匿名对象作为参数还是不习惯。现在C++也支持这种格式。

```cpp
[capture list](parameter list)->return type {function body}
```

capture list表示要捕捉的局部变量。

### 排序和二分查找

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main(int argc, char const *argv[])
{
    vector<int> v = {2,6,1,5,9,0,3,8,7,4};
    vector<int> v1 = {5,9};

    vector<int>::iterator i = find(v.begin(),v.end(),3); // 查找元素
    cout<< i - v.begin() <<endl;
    i = search(v.begin(),v.end(),v1.begin(),v1.end()); //查找子序列
    cout<< i - v.begin() <<endl;
    
    sort(v.begin(),v.end()); //还有一个稳定的排序stable_sort
    // lower_bound 返回第一个大于等于val的元素，不存在则返回end
    // upper_bound 返回第一个大于等于val的元素，不存在则返回end
    i = upper_bound(v.begin(),v.end(),30);
    cout<<v.end() - i<<endl;

    return 0;
}
```