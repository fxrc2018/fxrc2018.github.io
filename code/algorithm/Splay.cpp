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

