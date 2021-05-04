## Splay树

Splay树是一种BST树，它的查找、插入、删除、分割、合并等操作，复杂度都是$$O(\log n)$$的。它的最大特点是可以把某个结点往上旋转到指定位置，特别是可以旋转到根的位置，成为新的根节点。它有这样一种应用背景：如果需要经常查找和使用一个数，那么把它旋转到根结点后，这样下次访问它，只需要查一次就找到了。

Splay主要有以下几个操作：

- 把结点 x 旋转到根
    - 如果父节点就是根，只需要旋转一次结点 x 。
    - 如果 x 的父节点不是根，且 x 、 x 的父节点、x 父节点的父节点，三点共线，则需要先旋转一次 x 的父节点，再旋转 x 。
    - 如果 x 的父节点不是根，且 x 、 x 的父节点、x 父节点的父节点，三点不共线，则旋转两次 x 。
- 插入
    - 插入和普通二叉搜索树的方法一样。在插入之后，可以根据需要对新插入的结点左Splay操作。
- 分裂
    - 以第 k 小的数为界，把树分成两个部分。先把第 k 小的元素旋转到树根，然后把它与右子树断开，就得到了两棵树。
- 合并
    - 可合并的两棵树，其中一棵树 left 的所有元素应该小于另一个数 right 的所有元素。合并过程是先把 left 的最大元素 x 旋转到根，此时树根 x 没有右子树，把 x 的右子树接到 right 的根，就完成了合并。
- 删除
    - 把待删除的结点旋转到根，然后删除它，再合并左、右子树。

### 例题

给定一个长度为 n 的整数序列，初始时序列为 {1,2,…,n−1,n}。
序列中的位置从左到右依次标号为 1∼n。
我们用 [l,r] 来表示从位置 l 到位置 r 之间（包括两端点）的所有数字构成的子序列。
现在要对该序列进行 m 次操作，每次操作选定一个子序列 [l,r]，并将该子序列中的所有数字进行翻转。
例如，对于现有序列 1 3 2 4 6 5 7，如果某次操作选定翻转子序列为 [3,6]，那么经过这次操作后序列变为 1 3 5 6 4 2 7。
请你求出经过 m 次操作后的序列。

```cpp
#include <iostream>
using namespace std;

const int N = 1e5 + 10;
struct Node{
    int s[2]; // 左右子树
    int p; // 父结点
    int v; //值
    int flag; //旋转标记
    int size; //大小
    
    void init(int _v, int _p)
    {
        v = _v, p = _p;
        size = 1;
    }
};

Node tr[N];
int idx;
int root;
int n,m;

void pushup(int x){
    tr[x].size = tr[tr[x].s[0]].size + tr[tr[x].s[1]].size + 1;
}


void pushdown(int x)
{
    if (tr[x].flag)
    {
        swap(tr[x].s[0], tr[x].s[1]);
        tr[tr[x].s[0]].flag ^= 1;
        tr[tr[x].s[1]].flag ^= 1;
        tr[x].flag = 0;
    }
}

void rotate(int x){
    int y = tr[x].p, z = tr[y].p;
    int k = tr[y].s[1] == x;  // k=0表示x是y的左儿子；k=1表示x是y的右儿子
    tr[z].s[tr[z].s[1] == y] = x, tr[x].p = z;
    tr[y].s[k] = tr[x].s[k ^ 1], tr[tr[x].s[k ^ 1]].p = y;
    tr[x].s[k ^ 1] = y, tr[y].p = x;
    pushup(y), pushup(x);
}


//将结点x旋转为k的儿子，如果k=0，代表旋转到根
void splay(int x, int k){
    while(tr[x].p != k){
        int y = tr[x].p;
        int z = tr[y].p;
        if(z != k){ //要转到k下面，相当于y就是要到的父节点
            if((tr[y].s[1] == x) ^ (tr[z].s[1] == y)){
                rotate(y);
            }else{
                rotate(x);
            }
        }
        rotate(x);
    }
    if(k == 0){
        root = x;
    }
}

void insert(int v){
    int u = root;
    int p = 0;
    while(u != 0){
        p = u;
        if(v > tr[u].v){
            u = tr[u].s[1];
        }else{
            u = tr[u].s[0];
        }
        
    }
    u = ++idx;
    if(p != 0){
        tr[p].s[v > tr[p].v] = u;
    }
    tr[u].init(v, p);
    splay(u,0);
}



int getK(int k){
    int u = root;
    while(u != 0){
        pushdown(u);
        int left = tr[u].s[0];
        int right = tr[u].s[1];
        if(tr[left].size >= k){
            u = left;
        }else{
            if(tr[left].size + 1 == k){
                return u;
            }else{
                u = right;
                k -= tr[left].size + 1;
            }
        }
    }
    return -1;
}

void output(int u)
{
    pushdown(u);
    if (tr[u].s[0]) output(tr[u].s[0]);
    if (tr[u].v >= 1 && tr[u].v <= n) printf("%d ", tr[u].v);
    if (tr[u].s[1]) output(tr[u].s[1]);
}

int main(){
    scanf("%d%d",&n,&m);
    for(int i=0;i<=n+1;i++){
        insert(i);
    }
    int l,r;
    for(int i=0;i<m;i++){
        scanf("%d%d",&l,&r);
        l = getK(l);
        r = getK(r+2);
        splay(l,0);
        splay(r,l);
        tr[tr[r].s[0]].flag ^= 1;
    }
    output(root);
    return 0;
}
```