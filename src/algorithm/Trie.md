## Trie

```cpp
#include <iostream>
#include <string>
using namespace std;

const int N = 2e4 + 10;

int trie[N][26];
int cnt[N];
int idx;

void insert(string s){
    int p = 0;
    for(int i=0;i<s.size();i++){
        if(trie[p][s[i] - 'a'] == 0){
            trie[p][s[i] - 'a'] = ++idx;
        }
        p = trie[p][s[i] - 'a'];
    }
    cnt[p]++;
}

int query(string s){
    int p = 0;
    for(int i=0;i<s.size();i++){
        if(trie[p][s[i] - 'a'] == 0){
            return 0;
        }
        p = trie[p][s[i] - 'a'];
    }
    return cnt[p];
}

int main(){
    int n;
    scanf("%d",&n);
    string op,s;
    for(int i=0;i<n;i++){
        cin>>op>>s;
        if(op == "I"){
            insert(s);
        }else if(op == "Q"){
            printf("%d\n",query(s));
        }
    }
    return 0;
}
```