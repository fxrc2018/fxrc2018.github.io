——主要参考了中国大学MOOC[数据结构](https://www.icourse163.org/course/ZJU-93001?tid=1002654021)课程的内容

这个代码我一直想写，但是一直都没有写，原因大概是自己对算法不熟，还有没有测试平台，并不知道自己写得对不对。但是我感觉主要原因还是自己比较懒，不愿意去写。其实以前学MOOC的写过一次，但是参考别人的代码较多，没有自己的理解，所以很快就忘了。写这个程序我最大的感受是，递归真的相当重要，这里有一个很重要的技巧，沿着函数返回的路，一路检查是不是不满足平衡的条件，经过调整，最后就得到了一颗平衡二叉树。写这个程序的时候，发现我在搜索二叉树中忘记写删除的方法了，其实删除也比较简单，找到左子树的最大值或右子树的最小值，替换，然后删除后面掉该叶子节点。这个算法本身并不好，但现在就知道这些吧。下面是具体的代码，可能有一点问题。
```
#include<stdio.h>
#include<stdlib.h>

#define BALANCE_FACTOR 1
typedef struct avltree* AVLTree;

struct avltree{
    int val;
    struct avltree *left;
    struct avltree *right;
    int height;
};

int height(AVLTree tree){
    return tree==NULL?-1:tree->height;
}

void update_height(AVLTree root){
    if(height(root->left) > height(root->right)){
        root->height = height(root->left) +1;
    }else{
        root->height = height(root->right)+1;
    }
}

AVLTree create_tree(int val){
    AVLTree tree = (AVLTree)malloc(sizeof(struct avltree));
    tree->height = 0;
    tree->left = NULL;
    tree->right = NULL;
    tree->val = val;
    return tree;
}

AVLTree singleLeft(AVLTree k2){
    AVLTree k1 = k2->left;
    AVLTree temp = k1->right;
    k1->right = k2;
    k2->left = temp;
    /* 更新变更了的树的高度，需要注意顺序 */
    update_height(k2);
    update_height(k1);
    return k1;
}

AVLTree doubleLeft(AVLTree k3){
    AVLTree k1 = k3->left;
    AVLTree k2 = k1->right;
    k1->right = k2->left;
    k3->left = k2->right;
    k2->left = k1;
    k2->right = k3;
    /* 更新变更了的树的高度，需要注意顺序 */
    update_height(k1);
    update_height(k3);
    update_height(k2);
    return k2;
}

AVLTree singleRight(AVLTree k2){
    AVLTree k1 = k2->right;
    k2->right = k1->left;
    k1->left = k2;
    update_height(k2);
    update_height(k1);
    return k1;
}

AVLTree doubleRight(AVLTree k3){
    AVLTree k1 = k3->right;
    AVLTree k2 = k1->left;
    k1->left = k2->left;
    k3->right = k2->right;
    k2->left = k3;
    k2->right = k1;
    update_height(k1);
    update_height(k3);
    update_height(k2);
    return k2;
}

AVLTree balance(AVLTree tree){
    if(tree == NULL){
        return;
    }
    /* 到这里的时候，两颗子树的高度已经更新，但是自己的高度没有更新 */
    if(height(tree->left) - height(tree->right) > BALANCE_FACTOR){
        if(height(tree->left->left) > height(tree->left->right)){
            tree = singleLeft(tree);
        }else{
            tree = doubleLeft(tree);
        }
    }

    if(height(tree->right) - height(tree->left) > BALANCE_FACTOR){
        if(height(tree->right->right) > height(tree->right->left)){
            tree = singleRight(tree);
        }else{
            tree = doubleRight(tree);
        }
    }
    /* 更新树的高度 */
    update_height(tree);
    return tree;

}

AVLTree insert(AVLTree root,int val){
    if(root == NULL){
        root = create_tree(val);
    }
    if(val > root->val){
        root->right = insert(root->right,val);
    }else if(val < root->val){
        root->left = insert(root->left,val);
    }else{
        ;/* 不处理有重复元素的情况 */
    }
    return balance(root); 
}

int findMin(AVLTree root){
    while(root->left){
        root = root->left;
    }
    return root->val;
}

AVLTree remove_tree(AVLTree root,int val){
    if(root == NULL){
        return root;
    }
    /* 先查找 */
    if(val > root->val){
        root->right = remove_tree(root->right,val);
    }else if(val < root->val){
        root->left = remove_tree(root->left,val);
    }else if(root->left && root->right){
        root->val = findMin(root->right);
        root->right = remove_tree(root->right,val);
    }else{
        root = root->left?root->left:root->right;
    }
    return balance(root);
}

int main(int argc, char const *argv[])
{
    int n;
    scanf("%d",&n);
    int i;
    AVLTree root = NULL;
    for(i=0;i<n;i++){
        int temp;
        scanf("%d",&temp);
        root = insert(root,temp);
    }
    printf("root = %d\n",root->val);
    root = remove_tree(root,root->val);
    printf("root = %d\n",root->val);
    system("pause");
    return 0;
}
```
