## 使用next_permutation解决排列组合问题

### next_permutation算法

next_permutation是C++中的一个函数，具体的作用是，对于一个给定的序列，对其重新排列，生成下一个排列，如何序列为最大序列，则将其重排为最小序列。还有一个对应的函数prev_permutation，这个函数是求前一个排列，如果是已经是最小的排列，则将其重排为最大的排列。next_permutation算法的具体步骤为：

1. 从后向前查找第一个相邻元素对(i,j)，并且满足A[i] < A[j]。
2. 在[j,end)中寻找一个最小的k使其满足A[i]<A[k]。
3. 将i与k交换。
4. 逆置[j,end)。

下面给出一个简单的实现。

```cpp
#include <iostream>
#include <algorithm>
using namespace std;

bool nextPermutation(int *begin, int *end){
    int *i,*j,*k,*x;
    for(x = end - 1;x > begin;--x){
        if(*(x-1) < *x ){
            i = x -1;
            j = x;
            break;
        }
    }
    if(x == begin){
        reverse(begin,end);
        return false;
    }   
    for(x=j,k=j;x<end;++x){
        if(*x < *k && *x > *i){
            k = x;
        }
    }
    iter_swap(i,k);
    reverse(j,end);
    return true;
}

int main(int argc, char const *argv[])
{
    int arr[] = {1,2,3,4};
    do{
        for_each(arr,arr+4,[](int a)->void{
            cout<<a<<" ";
        });
        cout<<endl;
    }while (nextPermutation(arr,arr + 4));
    return 0;
}

```

### 使用next_permutation求全排列

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main(int argc, char const *argv[])
{
    vector<int> v = {1,2,3,4};
    sort(v.begin(),v.end());
    do{
        for_each(v.begin(),v.end(),[](int a)->void{
            cout<<a<<" ";
        });
        cout<<endl;
    }while (next_permutation(v.begin(),v.end()));
    return 0;
}
```

### 使用next_permutation解决组合问题

这里有一个技巧，就是next_permutation函数支持重复元素，如果要求从n个元素中选择m个，可以对n-m个0和m个1进行全排列，其中，1代表选中，0代表不选中。对应的代码如下。

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main(int argc, char const *argv[])
{
    vector<int> v = {1,2,3,4,5};
    int m = 2,n = 5;
    vector<int> choose(n-m,0);
    for(int i=0;i<m;i++){
        choose.push_back(1);
    }
    do{
        for(int i=0;i<choose.size();i++){
            if(choose[i] == 1){
                cout<<v[i]<<" ";
            }
        }
        cout<<endl;
    }while (next_permutation(choose.begin(),choose.end()));
    return 0;
}
```

### 使用深度优先搜索解决全排列问题

在没有接触到next_permutation这个函数之前，遇到排列问题我都是使用DFS解决的。

给定一个由不同的小写字母组成的字符串，输出这个字符串的所有全排列。输入的字符串是排好序的。如输入abc，输出为：abc、acb、bac、bca、cab、cba。这个题目在一开始的时候，我不知道该怎么做，后来看了一个深搜的题目，发现和这个题目，很类似，然后就想到了解法。下面先给出代码：

```cpp
#include<iostream>
#include <cstring>
using namespace std;

int len;
char array[10];
int visited[10];
char result[10];

void Dfs(int n){
    //如果是终点，直接返回 
    if(n==len+1){
        result[n] = '\0';
        cout<<result<<endl;
        return;
    }
    //找可以踩的点 
    for(int i=0;i<len;i++){
        if(visited[i] == 0){
            visited[i] = 1;
            result[n-1] = array[i];
            Dfs(n+1);//计算后面的全排列 
            visited[i] = 0;//计算完了，算下一个，这里要释放 
        }
    }
}

int main(){
    cin>>array;
    len = strlen(array);
    memset(visited,0,sizeof(visited));
    Dfs(1);
    return 0;
}
```
