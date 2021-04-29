#include<iostream>
#include<stdio.h>
#include<string.h>
#include<string>
#include<vector>
#include<queue>
using namespace std;

#define CHARACTER_SET_SIZE 256

void writeBit(FILE *file, char bit,bool reset = false){
    static int index = 8;
    static char c = 0x00;
    // static FILE *pre = NULL;
    // if(file != pre){
    //     index = 0;
    //     c = 0x00;
    //     pre = file;
    // }
    if(reset == true){
        index = 8;
        c = 0x00;
        return;
    }
    if(index == 8){
        index = 0;
        c = 0x00;
        fwrite(&c,1,1,file); /* write a blank */
    }
    if(bit == '1'){
        c = c | (0x01 << (7-index));
    }
    index++;
    fseek(file,-1,SEEK_END);
    fwrite(&c,1,1,file);
}

bool readBit(FILE *file,bool reset = false){
    static int index = 8;
    static char c = 0x00;
    // static FILE *pre = NULL;
    // if(pre != file){
    //     index = 8;
    //     c = 0x00;
    //     pre = file;
    //     cout<<"read file changed"<<endl;
    // }
    if(reset == true){
        index = 8;
        c = 0x00;
        return false;
    }
    if(index == 8){
        fread(&c,1,1,file);
        index = 0;
    }
    if(((c<<index)&0x80) != 0){
        index++;
        return true;
    }else{
        index++;
        return false;
    }
}

struct Node 
{
    char ch;
    int freq;
    Node *left;
    Node *right;

    Node(char ch,int freq,Node *left,Node *right)
        :ch(ch),freq(freq),left(left),right(right){
    }        
};

struct cmp{
    bool operator()(Node *n1,Node *n2){
        return n1->freq > n2->freq; /* n1's priority is lower than n2 if retrun true */
    }
};

void releaseTrie(Node *root){
    if(root == NULL){
        return;
    }
    releaseTrie(root->left);
    releaseTrie(root->right);
    delete root;
}

Node* buildTrie(vector<int> &freq){
    int len = freq.size();
    priority_queue<Node*,vector<Node*>,cmp> q;
    for(int i=0;i<len;i++){
        if(freq[i] > 0){
            cout<<(char)i << " "<<freq[i]<<endl;
            q.push(new Node(i,freq[i],NULL,NULL));
        }
    }
    while(q.size() > 1){
        Node *x = q.top();
        q.pop();
        Node *y = q.top();
        q.pop();
        q.push(new Node('\0',x->freq + y->freq,x,y));
    }
    return q.top();
}

void buildCode(vector<string> &v,Node *node,string s){
    if(node->left == NULL && node->right == NULL){
        v[node->ch] = s;
        return;
    }
    buildCode(v,node->left,s+'0');
    buildCode(v,node->right,s+'1');
}



void writeTrie(FILE *file,Node *root){
    if(root->left == NULL && root->right == NULL){
        writeBit(file,'1');
        char c = root->ch;
        for(int i=0;i<8;i++){
            if(((c<<i) & 0x80) != 0){
                 writeBit(file,'1');
            }else{
                writeBit(file,'0');
            }
        }
        return;
    }
    writeBit(file,'0');
    writeTrie(file,root->left);
    writeTrie(file,root->right);
}

void encode(const char *input,const char *output){
    readBit(NULL,true);
    FILE *in = fopen(input,"rb");
    vector<int> freq(CHARACTER_SET_SIZE,0);
    char c;
    int len = 0;
    while((fread(&c,1,1,in))>0){
        freq[c]++;
        len ++ ;
    }
    Node *root = buildTrie(freq);
    vector<string> code(CHARACTER_SET_SIZE,"");
    buildCode(code,root,"");
    for(int i=0;i<code.size();i++){
        if(code[i] != ""){
            cout<<(char)i<<" "<<code[i]<<endl;
        }
    }

    writeBit(NULL,NULL,true);
    FILE *out = fopen(output,"wb+");
    writeTrie(out,root);
    for(int i=0;i<32;i++){
        if((len & 0x80000000) != 0){
            writeBit(out,'1');
        }else{
            writeBit(out,'0');
        }
        len = len<<1;
    }
    fseek(in,0,SEEK_SET);
    while((fread(&c,1,1,in))>0){
        for(int i=0;i<code[c].size();i++){
            writeBit(out,code[c][i]);
        }
    }
    releaseTrie(root);
    fclose(in);
    fclose(out);
}

Node *readTrie(FILE *file){
    if(readBit(file) == true){
        char c = 0x00;
        for(int i=0;i<8;i++){
            if(readBit(file) == true){
                c = c | (0x1 << (7 - i));
            }
        }
        return new Node(c,0,NULL,NULL);
    }
    return new Node('\0',0,readTrie(file),readTrie(file));
}

void decode(const char *input,const char *output){
    readBit(NULL,true);
    FILE *in = fopen(input,"rb");
    Node *root = readTrie(in);
    vector<string> code(CHARACTER_SET_SIZE,"");
    buildCode(code,root,"");
    for(int i=0;i<code.size();i++){
        if(code[i] != ""){
            cout<<(char)i<<" "<<code[i]<<endl;
        }
    }
    unsigned int len = 0;
    for(int i=0;i<32;i++){
        if(readBit(in) == true){
            len = len | (0x1 << (31 - i));
        }
    }
    
    writeBit(NULL,NULL,true);
    FILE *out = fopen(output,"wb+");
    for(int i=0;i<len;i++){
        Node *node = root;
        while(node->left != NULL || node->right != NULL){
            if(readBit(in) == true){
                node = node->right;
            }else{
                node = node->left;
            }
        }
        char c = node->ch;
        for(int j=0;j<8;j++){
            if((c & 0x80) != 0){
                writeBit(out,'1');
            }else{
                writeBit(out,'0');
            }
            c = c << 1;
        }     
    } 
    
    releaseTrie(root);
    fclose(in);
    fclose(out);
}

int main(int argc, char const *argv[])
{
    encode("input.txt","huffman.data");
    decode("huffman.data","output.txt");
    if(system("fc input.txt output.txt") != 0){
        cout<<"decode failed"<<endl;
    }else{
        //计算一下压缩效率
        FILE *fin = fopen("input.txt","r");
        fseek(fin,0,SEEK_END);
        double lenin = ftell(fin);
        fclose(fin);
        FILE *fout = fopen("huffman.data","r");
        fseek(fout,0,SEEK_END);
        double lenout = ftell(fout);
        fclose(fout);
        cout<<"decode successful"<<endl;
        cout<<"compression ratio = "<<(lenout/lenin)<<endl;
    }
    system("pause");
    return 0;
}