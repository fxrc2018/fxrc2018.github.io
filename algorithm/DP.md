## 枚举、递归和动态规划

动态规划我一直感觉有点难，主要原因是感觉每次都需要具体问题具体分析。听了《算法导论》的课程，感觉自己从中学到了一点套路，于是就想写下来，和大家分享一下。

动态规划通常用来求解最优化问题，并且，求解的问题需要满足两个要素：最优子结构和子问题重叠。

##动态规划的基本套路

课堂上老师对于动态规划的理解是：

* 动态规划 = 小心的暴力枚举。（DP = “careful brute force”）
* 动态规划 = 枚举 + 递归 + 记忆化。（DP = guessing + recursion + memorization）

解决动态规划问题主要有以下几个步骤：

1. 定义子问题。
2. 只考虑当前情况下，枚举所有的可能，然后选择一个最好的。
3. 解决相关的子问题。
4. 使用递归+记忆化的方式解决，或者使用动态规划表，自底向上的解决。这里需要检查子问题是不是有环的。
5. 解决基本情况。

时间复杂度 = 子问题的个数 * 解决子问题花费的时间。

这里最难的一点是定义子问题。如果输入是字符串或者数组的时候，子问题通常为下面三种情况之一：

* 后缀，从i开始到字符串结尾。
* 前缀，从0开始到i。
* 子串，从i到j。

对于子问题的定义会影响算法的复杂度。考虑下面这个问题。

给定一个整型数组arr，代表数值不同的纸牌排成一条线，玩家A和玩家B依次拿走每张纸牌，规定玩家A先拿，玩家B后拿，但是每个玩家每次只能拿走最左和最右的纸牌，玩家A和玩家B绝顶聪明。请返回最后的获胜者的分数。

上面这个题目时左程云书中的一个题目，他给出的解答是用了两个递归函数，也就是，子问题划分为两种情况，在$(i,j)$上先手最后获得分数，和在$(i,j)$上后手获得的分数。但后来我看到了另外一种解法，就是把子问题定义在$(i,j)$上先手比后手多获得的分数。这样，子问题的数量就减少了，但最后还需对结果进行处理，因为，这里求的是先手比后手多的值。

首先，在定义好子问题后，我们分析如何利用上面的套路来解决这个问题。

1. 对于状态$(i,j)$，都只有两种选择，选择前面的和后面的，如果选择前面的，状态变为了$(i+1,j)$，如果选择后面的，状态变为了$(i,j-1)$。当前状态的值为两个值中较大的。
2. 从上面的状态转移，可知，状态的转移是单向的，是无环的。
3. 如果只有一张牌了，那么当前状态的值就为这张牌的值。

从上面的分析，写出的代码如下：

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#define INF 0x07FFFFFFF

int guess(vector<int> &iterms, int left, int right, vector<vector<int> > &dp){
    if(left == right){ /* 只有一个，先手可以拿完 */
        dp[left][right] = iterms[left];
    }
    if(dp[left][right] == INF){
        /* 可以拿左边的，也可以拿右边的，取最大值 */
        dp[left][right] = max(iterms[left] - guess(iterms,left+1,right,dp),
            iterms[right] - guess(iterms,left,right-1,dp));
    }
    return dp[left][right];
}

int stoneGame(vector<int>& iterms) {
    if(iterms.size() == 0){
        return 0;
    }
    /* dp代表只考虑序列i,j的情况下，先手比后手多的值 */
    vector<vector<int> > dp(iterms.size(),vector<int>(iterms.size(),INF));
    guess(iterms,0,iterms.size()-1,dp);
    return dp[0][iterms.size()-1];
}

int main(int argc, char const *argv[])
{

    int n,num;
    cin>>n;
    vector<int> v;
    int sum = 0;
    for(int i=0;i<n;i++){
        cin>>num;
        v.push_back(num);
        sum += num;
    }
    int ret = stoneGame(v);
    if(ret < 0){ /* 先手可能会输 */
        ret = - ret; 
    }
    sum = (sum - ret) / 2 + ret;
    cout<<sum<<endl;
    system("pause");
    return 0;
}
```

在看下面一个问题。

给定两个字符串str1和str2，再给定三个整数ic，dc和rc，分别代表插入、删除和替换一个字符的代价，请输出将str1编辑成str2的最小代价。

这里就直接给出代码。


```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

#define INF 0x07FFFFFFF

int minCostGuess(string &s1, string &s2, int l1, int l2, int ic,int dc,int rc,vector<vector<int> > &dp){
    if(l1 == s1.size()){
        //只能插入
        dp[l1][l2] = (s2.size() - l2)*ic;
    }else if(l2 == s2.size()){
        //只能删除
        dp[l1][l2] = (s1.size() - l1) * dc;
    }
    if(dp[l1][l2] == INF){ //没有算出来
        int res = INF;
        res = min(res,ic + minCostGuess(s1,s2,l1,l2+1,ic,dc,rc,dp)); // 插入
        res = min(res,dc + minCostGuess(s1,s2,l1+1,l2,ic,dc,rc,dp)); // 删除
        if(s1[l1] == s2[l2]){
            rc = 0; //如果相同，替换的代价为0
        }
        res = min(res,rc + minCostGuess(s1,s2,l1+1,l2+1,ic,dc,rc,dp)); // 替换
        dp[l1][l2] = res;
    }
    return dp[l1][l2];
}

int minCost(string &s1, string &s2, int ic,int dc,int rc){
    vector<vector<int> > dp(s1.size()+1,vector<int>(s2.size()+1,INF));
    minCostGuess(s1,s2,0,0,ic,dc,rc,dp);
    return dp[0][0];
}


int main(int argc, char const *argv[])
{
    string s1,s2;
    int ic,dc,rc;
    cin>>s1>>s2>>ic>>dc>>rc;
    cout<<minCost(s1,s2,ic,dc,rc)<<endl;    
    system("pause");
    return 0;
}
```

上面的代码去牛客网测试，发现时间超了（但我多试了几次，发现有时候能够过），所以，有时候写出这种记忆化递归版本并不能通过，还需要对其进行优化。

##动态规划的优化——使用动态规划表、空间优化

```cpp
int minCost(string s1,string s2,int ic,int dc,int rc){
    int n = s1.size();
    int m = s2.size();
    vector<vector<int>> dp(n +1 ,vector<int>(m+1,0));
    for(int i=1;i<=m;i++){
        dp[0][i] = ic * i;
    }
    for(int i=1;i<=n;i++){
        dp[i][0] = dc * i;
    } 
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            dp[i][j] = min(dp[i][j-1] + ic,dp[i-1][j] + dc);
            int tmp = s1[i-1] == s2[j-1]?dp[i-1][j-1]:dp[i-1][j-1] + rc;
            dp[i][j] = min(dp[i][j],tmp);
        }
    }
    return dp[n][m];
}
```


未完待续。
