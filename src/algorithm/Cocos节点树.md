## Cocos节点树

为什么要在算法里面说Cocos的节点树呢，因为我觉得Cocos的节点树设计得不错，使用了二叉树的思想，但写出来的代码却和二叉树的代码看上去很不同。

```cpp
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

class Node{
public:
    string name;
    int localZOrder;
    vector<Node*> children;
    Node* parent;

    Node(string name, int localZOrder):
        name(name),
        localZOrder(localZOrder),
        children(),
        parent(NULL){}

    void sortChildren(){
        sort(children.begin(), children.end(), [](const Node* n1 , const Node* n2)->bool{
            return n1->localZOrder < n2->localZOrder;
        });
    }

    void addChild(Node* node){
        children.push_back(node);
        node->parent = this;
    }

    void _inOrder(){
        int i;
        //和中序遍历相似，这里隐含的条件是，localZOrder小于0在左子树，大于0在右子树
        for(i=0;i<children.size();i++){
            if(children[i]->localZOrder >= 0){
                break;
            }
            children[i]->_inOrder();
        }
        cout<<name<<" ";
        for(i;i<children.size();i++){
            children[i]->_inOrder();
        }
    }

    void inOrder(){
        sortChildren();
        _inOrder();
        cout<<endl; //换行
    }

    void setLocalZOrder(int newZOrder){
        localZOrder = newZOrder;
    }

};

int main(int argc, char const *argv[])
{
    Node *root = new Node("A",0);
    int p = 1;
    for(int i=-2;i<=1;i++){
        root->addChild(new Node(string(1,'A' + p),i));
        p++;
    }
    for(int i=-2;i<=1;i++){
        root->children[0]->addChild(new Node(string(1,'A' + p),i));
        p++;
    }
    root->inOrder();
    root->children[0]->setLocalZOrder(3);
    root->inOrder();
    system("pause");
    return 0;
}
```

我个人感觉Cocos的节点树写得确实不错，代码可以写得比较简单，但支持这种动态的调整各个层级的`localZOrder`，并且能够处理好本层之间的相对大小关系。我觉得这个代码值得学习，所以写了一个简单的版本在这里。