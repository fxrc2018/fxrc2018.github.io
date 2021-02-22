## Cache Lab

### part A
Cache Lab分为两部分，第一部分是写一个缓存的模拟器，这个其实还比较简单，只要弄明白缓冲的工作原理就行。写的时候，对于LRU的实现，开始我打算使用使用标志位，每次缓存+1的策略实现，但这样可能会溢出。后来看了别人的代码，可以使用一个大数初始化，然后把其他缓存减一的策略，这样可以有效克服溢出的情况。我写代码的时候，没有认真读实验手册，导致没有使用原来定义好的结构体，但还是可以通过测试。

```cpp
#define MAX 100000

struct line{
    int *flags;/* 是否有效 */
    long long *heads;/* 缓存头部 */
    int *times;/* 记录每组的时间 */
    int E;/* 有多少组 */
};

struct cache{
    int b;/* 记录偏移的长度 */
    struct line *lines;/* 缓存的地址 */
    int S;/* 缓存的组数 */
    long long mask_index;/* 索引的掩码 */
    long long mask_mark;/* 标记的掩码 */
    int hits;/* 命中 */
    int misses;/* 未命中 */
    int evictions;/* 驱逐 */
};

/* 对函数进行声明 */
void init_cache(struct cache *cache,int s,int E,int b);/* 初始化缓存 */
void init_line(struct line *line,int E);/* 初始化缓存行 */
void get_cache(struct cache *cache,long long addr);/* 得到缓存 */

void init_cache(struct cache *cache,int s,int E,int b){
    cache->S = 1<<s;
    cache->lines = malloc(cache->S * sizeof(struct line));
    int i;
    for(i=0;i<cache->S;i++){
        init_line(cache->lines+i,E);
    }
    cache->b = b;

    long long mask_index = -1ll;
    long long mask_mark = -1ll;
    mask_mark = mask_mark<<(s + b);
    mask_index = ~( mask_mark | ~(mask_index << b));
    cache->mask_index = mask_index;
    cache->mask_mark = mask_mark;
    //printf("%p %p\n",mask_mark,mask_index);

    cache->misses = 0;
    cache->hits = 0;
    cache->evictions = 0;
}

void init_line(struct line *line,int E){
    /*为其他的内容分配内存*/
    line->heads = (long long *)malloc(E * sizeof(long long));
    line->flags = (int *)malloc(E * sizeof(int));
    line->times = (int *)malloc(E * sizeof(int));
    line->E = E;
    int i;
    for(i =0;i<E;i++){
        line->flags[i] = 0;
        line->times[i] = 0;
    }
}

void get_cache(struct cache *cache,long long addr){
    /* 根据索引找到组 */
    int index = (int)(addr & cache->mask_index)>>cache->b;
    printf("index = %d\n",index);
    /* 在组内找标记 */
    struct line *line= cache->lines + index;
    int i;
    for(i = 0;i<line->E;i++){
        long long content = line->heads[i];
        int flag = line->flags[i];
        if((addr & cache->mask_mark) == (content & cache->mask_mark) && flag == 1){
           printf("hits\n");
		   /* 更新使用时间 */
           int j;
           for(j=0;j<line->E;j++){
               if(j == i){
                   line->times[j] = MAX;
               }else{
                   line->times[j]--;
               }
           }
           cache->hits++;
           return;
       }
    }
    cache->misses++;
    /* 如果没满，进行缓存 */
    for(i = 0;i<line->E;i++){
        if(line->flags[i] == 0){
            line->heads[i] = addr;
            line->flags[i] = 1;
            printf("flag = %d\n",line->flags[i]);
			/* 更新使用时间 */
           int j;
           for(j=0;j<line->E;j++){
               if(j == i){
                   line->times[j] = MAX;
               }else{
                   line->times[j]--;
               }
           }
            printf("miss\n");
            return;
        }
    }
    /* 驱逐 */
    cache->evictions++;
    printf("eviction\n");
	int j,pos=0,min = MAX;
    for(j = 0;j<line->E;j++){
        if(line->times[j] < min){
            min = line->times[j];
            pos = j; 
        }
    }
    line->heads[pos] = addr;
	/* 驱逐之后仍然需要更新时间 */
	for(i=0;i<line->E;i++){
		if(i == pos){
			line->times[i] = MAX;
		}else{
			line->times[i]--;
		}
	}
}

int main(int argc,char *argv[])
{
	int s = 1;
	int E = 1;
	int b = 1;
	char *file = NULL;

	int opt;
	while( (opt = getopt(argc,argv,"s:E:b:t:")) != -1){
		switch(opt){
			case 's':
				s = atoi(optarg);
				break;
			case 'E':
				E = atoi(optarg);
				break;
			case 'b':
				b = atoi(optarg);
				break;
			case 't':
				file = optarg;
				break;
			default:
				break;
		}
	}
	printf("s = %d E = %d b = %d file = %s\n",s,E,b,file);
	/* 初始化缓存 */
	struct cache cache;
	init_cache(&cache,s,E,b);
	FILE *fp = fopen(file,"r");
	if(fp == NULL){
		printf("open file fail\n");
		return 1;
	}
	char c;
	long long x,y;
	while(fscanf(fp,"%c%llx%llx",&c,&x,&y) != EOF){
			switch(c){
				case 'L':
				case 'S':
					get_cache(&cache,x);
					break;
				case 'M':
					get_cache(&cache,x);
					get_cache(&cache,x);
					break;
				default:
					break;
			}
	}
    printSummary(cache.hits, cache.misses, cache.evictions);
    return 0;
}
```

### part B

个人感觉part B很难，感觉不好分析，代码实现也略有难度。先说说32 * 32的情况，因为使用的缓存大小为s=5，E=1，b=5。我们假设B[0][0]映射到组1，因为缓存的大小为32字节，所以可以放8个int型数据。先看看直接转置的问题。因为矩阵是连续存储的，所以B[1][0]映射到组5，B[2][0]映射到组9，以此类推。我们发现，B[8][0]映射到组1。所以，如果我们直接转置，写入8个int后，下次就会发生驱逐。所以，我们应该先填满缓存，然后在读入下一组。对应的代码如下。

```cpp
void transpose_submit(int M, int N, int A[N][M], int B[M][N]){
    if(M == 32){
        int i,j,tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7;
        for(j=0;j< 32;j+=8){
            for(i=0;i<32;i++){
                tmp0 = A[i][j+0];
                tmp1 = A[i][j+1];
                tmp2 = A[i][j+2];
                tmp3 = A[i][j+3];
                tmp4 = A[i][j+4];
                tmp5 = A[i][j+5];
                tmp6 = A[i][j+6];
                tmp7 = A[i][j+7];

                B[j+0][i] = tmp0;
                B[j+1][i] = tmp1;
                B[j+2][i] = tmp2;
                B[j+3][i] = tmp3;
                B[j+4][i] = tmp4;
                B[j+5][i] = tmp5;
                B[j+6][i] = tmp6;
                B[j+7][i] = tmp7;

            }
        }
        return;
    }
    
    if(M == 64){
        int i,j,tmp0,tmp1,tmp2,tmp3;
        for(j=0;j<M;j+=4){
            for(i=0;i<M;i++){
                tmp0 = A[i][j];
                tmp1 = A[i][j+1];
                tmp2 = A[i][j+2];
                tmp3 = A[i][j+3];
                B[j][i] = tmp0;
                B[j+1][i] = tmp1;
                B[j+2][i] = tmp2;
                B[j+3][i] = tmp3;
            }
        }
        return;
    }

    if(M == 61){
        int ii,jj,i,j,tmp;
        for(ii=0;ii<M;ii+=8){
            for(jj=0;jj<N;jj+=8){
                for(i=jj;i<jj+8&&i<N;i++){
                    for(j=ii;j<ii+8&&j<M;j++){
                        tmp = A[i][j];
                        B[j][i] = tmp;
                   }
               }
           }
       }
    }
}
```
64 * 64的分析差不多，但是这样做得不了满分，可以拿到8分中的4分。看了别人的解答，感觉还是很复杂。至于61 * 67，直接使用8 * 8的分块就过了。

个人对于Part B的理解确实不多，就写到这里吧。
