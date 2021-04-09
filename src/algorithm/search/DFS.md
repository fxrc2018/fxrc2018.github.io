## DFS

在图中进行深搜，一般可以用递归函数实现。如果要判断从某个点V出发能否走到终点，程序框架如下（一开始V是新点）：

```cpp
bool Dfs(V) {
    if(V是终点) {
        return true;
    }
    if(V是旧点) {
        return false; 
    }
    将V标记为旧点; 
    对和V相邻的每一个结点U{
        if(Dfs(U) == true){
            return true;
        }
    } 
    return false;
}
```

 如果要记录从起点到终点的路径，则需要一个数组path来记录一路走过的每一个结点，还需要一个变量depth来记录当前的深度（从出发起走到当前结点的步数）。深度可以不作为全局变量而写成Dfs()的参数。假设结点的类型是Node，则程序的框架如下：

```cpp
Node path[MAX_LEN]
int depth;

bool Dfs(V) {
    if(V是终点) {
        path[depth] = V;
        return true;
    }
    if(V是旧点) {
        return false; 
    }
    path[depth] = V;
    depth++; 
    将V标记为旧点; 
    对和V相邻的每一个结点U{
        if(Dfs(U) == true){
            return true;
        }
    } 
    //如果需要从这里返回false，证明了从这个点到不了终点
    //要删除这个点 
    depth--; 
    return false;
}

int main(){
    将所有点都标记为新点；
    depth = 0;
    if(Dfs(起点)){
        for(int i = 0;i <= depth;depth++){
            cout<<path[i]<<endl;
        }
    } 
    return 0;
} 
```
把图中的所有结点都走一遍，这叫做遍历一个图。可以用深度优先搜索的方法遍历一个图，遍历图的代码框架如下：

```cpp
Dfs(V){
    if(V是旧点){
        return;
    }
    将V标记为旧点;
    对和V相邻的每一个点U{
        Dfs(U); 
    } 
}

int main(){
    将所有的点都标记为新点;
    while(在图中能找到新点k){
        Dfs(k);
    } 
    return 0;
}
```