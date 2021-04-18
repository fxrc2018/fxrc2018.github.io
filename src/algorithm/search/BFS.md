## BFS

树与图的广度优先遍历需要使用一个队列来实现。起初，队列中仅包含一个起点。在广度优先遍历过程中，我们不断从队头取出一个节点 x ，对于 x 面对的多条分支，把沿着每条分支到达的下一个节点（如果尚未访问过）插入队尾。重复执行上述过程直到队列为空。

广度优先遍历是一种按照层次顺序访问的方法，它具有如下两个重要性质：

1. 在访问完所有的第 i 层节点后，才会开始访问第 i+1 层节点。

2. 任意时刻，队列中至多有两个层次的节点。若其中一部分节点属于第 i 层，则另一部分节点属于第 i+1 层，并且所有第 i 层节点排在第 i+1 层节点之前。


也就是说，广度优先遍历队列中的元素关于层次满足两段性和单调性。

### Flood-Fill

给定一个 N \* M 的 01 矩阵和一个起点（ 0 表示可通行，1 表示禁止点），每一步可以向上、下、左、右四个方向之一移动 1 单位距离，求从出发点能够到达哪些位置，以及达到每个位置的最小步数。我们也称 BFS 解决该问题的算法为 Flood-Fill，就好像在起点倒水，看能覆盖多大的一片区域。


### 最小步数模型

给定一个矩形地图，控制一个物体在地图中按要求移动，求最小步数。

对于上述问题，很适合用广度优先搜索解决。因为广度优先搜索是逐层遍历搜索树的算法，所有状态按照入队的先后顺序具有层次单调性（也就是步数单调性）。**如果每一次扩展恰好对应一步，那么当一个状态第一次被访问（入队）时，就得到了从起始状态到达该状态的最小步数。**

### 双端队列BFS

在一张边权要么是 0 、要么是 1 的图上，我们可以用双端队列广搜来计算。算法的整体框架和一般的广搜类似，只是在每个节点上沿分支扩展的时候稍作改变。如果这条分支的边权为 0 ，那么把沿该分支到达的新节点从队头入队；否则，从队尾入队。这样一来，我们仍然能保证，任意时刻广搜队列中的节点对应的距离值都具有“两段性”和“单调性”，每个节点虽然可能被更新（入队）多次，**但是它第一次被扩展（出队）时，就能得到从源出发到达该点的最短距离**，之后再被取出可以直接忽略。

### 优先队列BFS

对于更加普适性的情况，也就是每次扩展都有各自不同的代价时，求出起始状态到每个状态的最小代价，就相当于在一张带权图上求出从起点到每个节点的最短路。我们通常使用优先队列进行广搜。

我们可以每次从队列中取出当前代价最小的状态进行扩展（该状态一定已经是最优的，因为队列中其他状态的代价都不小于它，所以以后就不可能再更新它了），沿着各条分支到达的新状态加入优先队列。不断执行搜索，直到队列为空。

在优先队列BFS中，每个状态也会被多次更新，多次进出队列，一个状态也可能以不同的代价在队列中同时存在。不过，**每个状态第一次被取出时，就得到了从起始状态到该状态的最小代价。**之后若再次被取出，则可以直接忽略，不进行扩展。所以，优先队列BFS中每个状态只扩展一次，时间复杂度只多了维护二叉堆的代价。若一般广搜的复杂度为 O(N) ，则优先队列BFS的复杂度为 O(N logN)。

### 双向广搜

从起始状态、目标状态分别开始，两边轮流进行（也可以从两个队列中大小较小的一个进行扩展），每次各扩展一整层（把队列中的节点都扩展一次）。当两边各自有一个状态在记录数组中发生重复时，就说明这两个搜索过程相遇了，可以合并得出起点到终点的最少步数。

### A*

如果给定一个目标状态，需要求出从初态到目标状态的最小代价，那么优先队列BFS的这个优先策略显然是不完善的。一个状态的当前代价最小，只能说明从起始状态到该状态的代价很小，而在未来的搜索中，从该状态到目标状态可能会花费很大的代价。另外一些状态虽然当前代价略大，但是未来到目标状态的代价可能会很小，于是从起始状态到目标状态的总代价反而更优。优先队列BFS会优先选择前者的分支，导致求出最优解的搜索量增大。

为了提高搜索效率，我们很自然地想到，可以对未来可能产生的代价进行预估。详细地讲，我们设计一个估价函数，以任意状态为输入，计算出从该状态到目标状态所需代价的最小估计值。在搜索中，仍然维护一个堆，不断从堆中取出当”当前代价+未来估价“最小的状态进行扩展。

为了满足第一次从堆中取出目标状态时得到的就是最优解，我们设计的估价函数需要满足一个基本准则：估价函数的估值不能大于未来实际代价，估价比实际代价更优。

这种带有估价函数的优先队列BFS就称为 A\* 算法。只要保证对于任意状态 state，都有 f(state)<=g(state)，A\*算法就一定能在目标状态第一次从堆中取出时得到最优解，并且在搜索过程中每个状态只需要被扩展一次（之后再被取出可以直接忽略）。 估价 f(state) 越准确，越接近 g(state) ，A\* 算法的效率就越高。如果估价始终为 0，就等价于普通的队列优先BFS。

A\* 算法提高搜索效率的关键，就在于能否设计出一个优秀的估价函数。估价函数在满足上述设计准则的前提下，还应该尽可能反映未来实际代价的变化趋势和相对大小关系，这样搜索才会较快地逼近最优解。

在寻路算法中，通常使用的估计函数有（以二维平面为例）：

* 曼哈顿距离

$$
D=|x_1 - x_2| + |y_1 - y_2|
$$

* 欧氏几何平面距离

$$
D=\sqrt{(x_1 - x _2)^2 + (y_1-y_2)^2}
$$

* 切比雪夫距离

$$
D=max(|x_1-x_2|,|y_1-y_2|)
$$

#### 八数码

把除空格之外的所有数字排成一个序列，求出该序列的逆序对数。如果初态和终态的逆序对数奇偶性相同，那么这两个状态互相可达，否则一定不可达。

```cpp
#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <vector>
#include <algorithm>
using namespace std;

int dx[4] = {-1, 0, 1, 0}, dy[4] = {0, 1, 0, -1};
char op[4] = {'u', 'r', 'd', 'l'};

string bfs(string s){
    string e = "12345678x";
    unordered_map<string,pair<string,char>> prev;
    prev[s] = {"",' '};
    queue<string> q;
    q.push(s);
    bool flag = true;
    while(q.size() > 0 && flag){
        string top = q.front();
        q.pop();
        int x,y;
        for(int i=0;i<9;i++){
            if(top[i] == 'x'){
                x =  i / 3;
                y = i % 3;
                break;
            }
        }
        for(int i=0;i<4;i++){
            int x1 = x + dx[i];
            int y1 = y + dy[i];
            if(x1>=0&&x1<3&&y1>=0&&y1<3){
                string nes = top;
                swap(nes[x1*3+y1],nes[x*3+y]);
                
                if(nes == e){
                    prev[nes] = {top,op[i]};
                    flag = false;
                    break;
                }
                if(prev.count(nes) == 0){
                    prev[nes] = {top,op[i]};
                    q.push(nes);
                }
            }
        }
    }
    string res = "";
    string tmp = e;
    while(tmp != s){
        res += prev[tmp].second;
        tmp = prev[tmp].first;
    }
    reverse(res.begin(),res.end());
    return res;
}

int f(const string &s){
   int res = 0;
   for(int i=0;i<9;i++){
       if(s[i] != 'x'){
           int pos = s[i] - '1';
           res += abs(i/3 - pos /3) + abs((i%3)-(pos%3));
       }
   }
   return res;
}

string astar(string s){
    string e = "12345678x";
    priority_queue<pair<int,string>,vector<pair<int, string>>, 
    	greater<pair<int, string>>> q;
    unordered_map<string,pair<string,char>> prev;
    unordered_map<string,int> dist;
    q.push({f(s),s});
    dist[s] = 0;
    while(q.size() >0 ){
        pair<int,string> top = q.top();
        q.pop();
        if(top.second == e){
            break;
        }
        int x,y;
        for(int i=0;i<9;i++){
            if(top.second[i] == 'x'){
                x = i / 3;
                y = i % 3;
                break;
            }
        }
        string state = top.second;
        int step = dist[top.second];
        for(int i=0;i<4;i++){
            int x1 = x + dx[i];
            int y1= y + dy[i];
            if(x1>=0&&x1<3&&y1>=0&&y1<3){
                string ns = state;
                swap(ns[x1*3+y1],ns[x*3+y]);
                // 可能会被更新多次
                if(dist.count(ns)==0 || dist[ns] > step + 1){
                    dist[ns] = step + 1;
                    q.push({step+1+f(ns),ns});
                    prev[ns] = {state,op[i]};
                }
            }
        }
    }    
    string res = "";
    string tmp = e;
    while(tmp != s){
        res += prev[tmp].second;
        tmp = prev[tmp].first;
    }
    reverse(res.begin(),res.end());
    return res;
}

int main(){
    char c;
    string s;
    for(int i=0;i<9;i++){
        cin>>c;
        s += c;
    }
    int cnt = 0;
    for(int i=1;i<9;i++){
        for(int j=0;j<i;j++){
            if(s[j] != 'x' && s[i] != 'x' && s[j] > s[i]){
                cnt ++;
            }
        }
    }
    if(cnt % 2 == 1){
        cout<<"unsolvable"<<endl;
    }else{
        cout<<astar(s)<<endl;
    }
    
    return 0;
}
```