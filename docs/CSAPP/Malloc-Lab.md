## Malloc Lab

这个实验要求我们实现一个小型的内存分配器，需要实现以下几个函数。

```cpp
int mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t size);
```

其实，书上已经实现了这几个函数，我们先去跑一跑。这里需要注意一点，trace文件在官方的文件中没有，需要自己去下载。另外，为了方便，我修改了config.h文件，把trace的默认文件路径改为了当前文件路径。

```cpp
 ./mdriver -v  
Results for mm malloc:
trace  valid  util     ops      secs  Kops
 0       yes   99%    5694  0.009728   585
 1       yes   99%    5848  0.009026   648
 2       yes   99%    6648  0.015117   440
 3       yes  100%    5380  0.011085   485
 4       yes   66%   14400  0.000160 89944
 5       yes   92%    4800  0.008537   562
 6       yes   92%    4800  0.007930   605
 7       yes   55%   12000  0.286405    42
 8       yes   51%   24000  0.437601    55
 9       yes   27%   14401  0.136688   105
10       yes   34%   14401  0.003792  3797
Total          74%  112372  0.926069   121

Perf index = 44 (util) + 8 (thru) = 53/100
```
util代表Space utilization空间利用率，而thru代表Throughput，吞吐量。空间利用率占60分，而吞吐量占40分。首先分析书上的实现的缺陷。

首先，肯定是要优化吞吐量，因为是使用的链表，每一次查找都需要花费O(n)，这里书上给出的建议是使用显式链表，也就是遍历的时候，可以不用遍历非空闲块，这样可以加快速度。进一步的优化是使用分离链表，也就是把空闲块按照大小分为几种情况，分别存储。

然后还需要优化空间利用率，这里我把首次适配修改为了最佳适配，发现有几个例子的空间利用率还是不高。这肯定和请求的序列有关。首先是7和8两个请求序列，这两个请求序列的特点是，首先是多次交替请求小块和大块，然后释放所有的大块，然后再请求一个更大的块。这完全就是要搞事情，因为大块中间夹着小块，所以无法释放，造成空间利用率很低。然后9和10两个请求序列，这两个请求序列的特点是有realloc，而书上的实现比较暴力，所以需要优化。

感觉很简单，写的时候，还是发现了很多问题。第一是代码的思路简单，但实现相对来说比较困难，需要注意的地方比较多。第二是调试困难，个人感觉就没办法调试。这个实验搞了半天，没有做出来，有点遗憾，先这样吧，有时间再弄。