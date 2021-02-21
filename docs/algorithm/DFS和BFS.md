## DFS和BFS

```cpp
#include <iostream>
#include <string>
using namespace std;

const int N = 10;
int arr[N];
int n;
int cnt;

void dfs(int x){
    if(x == n){
        for(int i=0;i<n;i++){
            string str(n,'.');
            str[arr[i]] = 'Q';
            cout<<str<<endl;
        }
        cout<<endl;
        cnt++;
        return;
    }
    for(int i=0;i<n;i++){
        bool flag = false;
        for(int j=1;x-j>=0;j++){
            if(arr[x-j] == i || arr[x-j] == i - j || arr[x-j] == i+j){
                flag = true;
                break;
            }
        }
        // cout<<i<<endl;
        if(!flag){
            arr[x] = i;
            
            dfs(x+1);   
        }
    }
}

int main(){
    scanf("%d",&n);
    dfs(0);
    // printf("%d",cnt);
    return 0;
}
```

```cpp
#include <iostream>
#include <string>
#include <queue>
#include <algorithm>
#include <unordered_map>
using namespace std;

int bfs(string st){
    string ed = "12345678x";
    unordered_map<string,int> d;
    d[st] = 0;
    queue<string> q;
    q.push(st);
    while(q.size() > 0){
        string top = q.front();
        q.pop();
        if(top == ed){
            break;
        }
        int move[] = {-1,1,-3,3};
        int pos = -1;
        for(int i=0;i<9;i++){
            if(top[i] == 'x'){
                pos = i;
                break;
            }
        }
        for(int i=0;i<4;i++){
            if((i == 0 && pos % 3 == 0) || (i == 1 && pos % 3 == 2)){
                continue;
            }
            int pos2 = pos + move[i];
            string str = top;
            if(pos2 >= 0 && pos2 < 9){
                swap(str[pos],str[pos2]);
                if(d.count(str) == 0){
                    q.push(str);
                    d[str] = d[top] + 1;
                }
            }
            
        }
        
    }
    return d.count(ed) == 0?-1: d[ed];
}

int main(){
    string s;
    string st;
    int count = 0;
    for(int i=0;i<9;i++){
        cin>>s;
        st+=s;
    }
    printf("%d",bfs(st));
    return 0;
}
```

