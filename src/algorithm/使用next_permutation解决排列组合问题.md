## 使用next_permutation解决排列组合问题

### next_permutation算法

next_permutation是C++中的一个函数，具体的作用是，对于一个给定的序列，对其重新排列，生成下一个排列，如何序列为最大序列，则将其重排为最小序列。还有一个对应的函数prev_permutation，这个函数是求前一个排列，如果是已经是最小的排列，则将其重排为最大的排列。next_permutation算法的具体步骤为：

1. 从后向前查找第一个相邻元素对(i,j)，并且满足A[i] < A[j]。
2. 从后往前，在[j,end)中寻找第一个满足A[i]<A[k]的数。
3. 将i与k交换。
4. 逆置[j,end)。

下面给出一个简单的实现。

```cpp
void nextPermutation(vector<int>& nums) {
    int i = nums.size() - 2;
    while (i >= 0 && nums[i] >= nums[i + 1]) {
        i--;
    }
    if (i >= 0) {
        int j = nums.size() - 1;
        while (j >= 0 && nums[i] >= nums[j]) {
            j--;
        }
        swap(nums[i], nums[j]);
    }
    reverse(nums.begin() + i + 1, nums.end());
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