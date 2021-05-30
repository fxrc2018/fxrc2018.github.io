## AC自动机

AC自动机是KMP的升级版。KMP是单模匹配算法，处理在一个文本串中查找一个模式串的问题；AC自动机是多模匹配算法，能在一个文本串中同时查找多个不同的模式串。

KMP是通过查找 P 对应的 Next 数组实现快速匹配的。如果把所有的 P 做成一个字典树，然后在匹配的时候查找这个 P 对应的 Next 数组，不就实现了快速匹配的效果吗？

```cpp
#include <iostream>
#include <string>
#include <queue>
#include <cstring>
using namespace std;

const int N = 10000 + 10;
const int S = 50;
const int M = 26;

int tr[N * S][M];
int cnt[N * S];
int ne[N * S];
int idx;

void insert(const string &p){
    int cur = 0;
    for(int i=0;i<p.size();i++){
        int pos = p[i] - 'a';
        if(tr[cur][pos] == 0){
            tr[cur][pos] = ++idx;
        }
        cur = tr[cur][pos];
    }
    cnt[cur]++;
}

void build(){
    queue<int> q;
    for(int i=0;i<M;i++){
        if(tr[0][i] > 0){
            q.push(tr[0][i]);
        }
    }
    while(q.size() > 0){
        int top = q.front();
        q.pop();
        for(int i=0;i<M;i++){
            int p = tr[top][i];
            if(p > 0){
                q.push(p);
                ne[p] = tr[ne[top]][i];
            }else{
                tr[top][i] = tr[ne[top]][i]; // 隐含的递推操作
            }
        }
    }
}

int main(){
    int t;
    scanf("%d",&t);
    string p,str;
    while(t--){
        int n;
        memset(tr,0,sizeof(tr));
        memset(cnt,0,sizeof(cnt));
        memset(ne,0,sizeof(ne));
        idx = 0;

        scanf("%d",&n);
        for(int i=0;i<n;i++){
            cin>>p;
            insert(p);
        }
        cin>>str;
        build();
        int res = 0;
        for(int i=0,j=0;i<str.size();i++){
            int pos = str[i] - 'a';
            j = tr[j][pos];
            int p = j;
            while (p)
            {
                res += cnt[p];
                cnt[p] = 0;
                p = ne[p];
            }
        }
        printf("%d\n",res);
    }
    system("pause");
    return 0;
}
```