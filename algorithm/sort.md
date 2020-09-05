——主要参考了中国大学MOOC[数据结构](https://www.icourse163.org/course/ZJU-93001?tid=1002654021)课程的内容
这里讨论排序算法，它的函数模型是：

```c
void xSort ( ElementType A[], int N )
```

大多数情况下，为简单起见，讨论从小大的整数排序。这里讨论的排序还有如下两个特点：
* 只讨论基于比较的排序（> = < 有定义）
* 只讨论内部排序（排序工作能够在主存中完成） 

在排序算法中，有一个稳定性的概念，即任意两个相等的数据， 排序前后的相对位置不发生改变。这里之所以要讨论多种排序，是因为，没有一种排序是任何情况下都表现最好的。如果有的话最好，因为就只用学一种就行了。下面给出排序算法的一些比较：
|排序方法|平均时间复杂度|最坏情况下时间复杂度|额外空间复杂度|稳定性|
|:--:|:--:|:--:|:--:|:--:|
简单选择排序|$O(N^2)$| $O(N^2)$|	$O(1)$|	不稳定
 |冒泡排序|$O(N^2)$|$O(N^2)$|$O(1)$|稳定|
|直接插入排序|$O(N^2)$|$O(N^2)$|$O(1)$|	稳定|
 |希尔排序|$O(N^d)$|$O(N^2)$|$O(1)$|	不稳定|
 |堆排序|$O(NlogN)$|$O(NlogN)$|$O(1)$|不稳定|
 |快速排序|$O(NlogN)$|$O(N^2)$|$O(logN)$|不稳定|
 |归并排序|$O(NlogN)$|$O(NlogN)$|$O(N)$|稳定|
 |基数排序|$O(P(N+B))$|	$O(P(N+B))$|$O(N+B)$|	稳定|
##简单排序
 冒泡排序的思想十分简单，就是每次找到最大/最小的数，然后重复N次。所以，写这种程序还是比较简单的，下面直接给出冒泡排序的代码：

```c
#include<stdio.h>

void swap(int* a,int* b){
	int temp = *a;
	*a = *b;
	*b = temp;
}

void bubbleSort(int array[],int N) {
	int i,j;
	for(i=N-1;i>=0;i--){
		int flag = 0;//记录一轮是否发生了交换 
		for(j=0;j<i;j++){
			if(array[j] > array[j+1]){
				swap(&array[j],&array[j+1]);
				flag = 1;//发生了改变 
			}
		}
		if(flag == 0){
			break;//如果没发生交换，说明已经排好序了 
		}
	}
}

void insertionSort(int array[],int N){
	int i,j;
	for(i=1;i<N;i++){
		int temp = array[i];
		for(j=i;j>0&&temp<array[j-1];j--){//如果前一个比较大
			array[j] = array[j-1];//往后挪一个位置 
		}
		array[j] = temp;
	}
}

int main() {
	int array[] = {7,1,3,8,12,11,2,9};
	int size = sizeof(array)/sizeof(int);
//	bubbleSort(array,size);
	insertionSort(array,size);
	int i = 0;
	for(i=0; i<size; i++) {
		printf("%d ",array[i]);
	}
	return 0;
}
```

插入排序的算法就好像打牌的时候，摸牌之后的操作。这个算法个人感觉比冒泡排序要难一点。
##希尔排序
希尔排序其实和简单排序差不多，只不过是每次是个n个进行排序。这个我已经空了很久了，在K&R那本书里面看到了这个算法，然后就搬到了这里。下面是具体的代码：

```c
#include<stdio.h>

/* 使用希尔排序算法进行排序 */
void shellsort(int v[],int n){
	int gap,i,j,temp;
	/* 每次隔gap进行排序 */
	for(gap=n/2;gap>0;gap/=2){
	    /* 每次都是插入排序 */
	    for(i=gap;i<n;i++){
	        for(j=i-gap;j>=0&&v[j]>v[j+gap];j-=gap){
	            temp = v[j];
	            v[j] = v[j+gap];
	            v[j+gap] = temp;
	        }
	    }
	}
}

int main()
{
      int array[] = {7,1,3,8,12,11,2,9};
	int size = sizeof(array)/sizeof(int);
	shellsort(array,size);
	int i;
	for(i=0;i<size;i++){
		printf("%d ",array[i]);
	}
      return 0;
}
```
##分治排序
 快速排序是现在常用的算法，其核心是在线性时间内将一个数组以pivot(自己选择)为中点，使得比它大的都在其右边，比它小的都在其左边，然后进行递归调用，直到只剩下一个数字。我想的方法需要额外的空间，就是比pivot大的从右边开始放，比它小的从左边开始放，最后一个放在剩下的位置。它使用的算法要好一点，无需额外的空间。它和冒泡排序的不同之处在于，它一次把数组近似地分成了两份，而一次冒泡分成了1和n-1；所以，当运气不好的时候，它的复杂度也是 O(n^2 ),和冒泡排序一样，一般的复杂度是$O(nlogn)$。所以，计算pivot都是使用的随机化的方法，此处为了简单起见，pivot取数组的第一个数字。快速排序的简单代码如下：

```c
#include<stdio.h>

void swap(int* a,int* b){
	int temp = *a;
	*a = *b;
	*b = temp;
}

void quickSort(int array[],int left,int right){
	if(left>=right){
		return;
	}
	int i = left;
	int j = right;
	while(i!=j){
		while(j>i&&array[j]>=array[i]){
			j--;
		}
		swap(&array[i],&array[j]);
		while(i<j&&array[i]<array[j]){
			i++;
		}
		swap(&array[i],&array[j]);
	}
	quickSort(array,left,i-1);
	quickSort(array,i+1,right);
}

int main(){
	int array[] = {7,1,3,8,12,11,2,9};
	int size = sizeof(array)/sizeof(int);
	quickSort(array,0,size-1);
	int i;
	for(i=0;i<size;i++){
		printf("%d ",array[i]);
	}
	return 0;
} 
```

归并排序和快速排序的复杂度都是O(nlogn)  ，它的基本思想是，在线性时间内，对已经排序的数组合在一起。为了这样做，它需要额外的空间。它的代码如下：

```c
#include<stdio.h>

void merge(int array[],int left,int mid,int right,int temp[]){
	int i = left;
	int j = mid;
	int index = 0;
	while(i<mid&&j<right+1){
		if(array[i]<array[j]){
			temp[index++] = array[i++]; 
		}else{
			temp[index++] = array[j++];
		}
	}
	while(i<mid){
		temp[index++] = array[i++];
	}
	while(j<right+1){
		temp[index++] = array[j++];
	}
	for(i = 0,j = left;i<index;i++,j++){
		array[j] = temp[i];
	}
}

void mergeSort(int array[],int left,int right,int temp[]){
	if(left<right){
		int mid = left + (right - left)/2;
		mergeSort(array,left,mid,temp);
		mergeSort(array,mid+1,right,temp);
		merge(array,left,mid+1,right,temp);
	}
}

int main(){
	
	int array[] = {7,1,3,8,12,11,2,9};
	int size = sizeof(array)/sizeof(int);
	int temp[size];
	mergeSort(array,0,size,temp);
	int i = 0;
	for(i=0;i<size;i++){
		printf("%d ",array[i]);
	}
	return 0;
}
```

##优先队列和堆排序
优先队列（Priority Queue）：特殊的“队列”，取出元素的顺序是依照元素的优先权（关键字）大小，而不是元素进入队列的先后顺序。优先队列的实现一般是用堆。

类型名称：最大堆（MaxHeap）
数据对象集：完全二叉树，每个结点的元素值不小于其子结点的元素值。
操作集：最大堆H ∈ MaxHeap，元素item  ∈ ElementType，主要操作有：
1. MaxHeap Create( int MaxSize )：创建一个空的最大堆。
2. Boolean IsFull( MaxHeap H )：判断最大堆H是否已满。
3. Insert( MaxHeap H, ElementType item )：将元素item插入最大堆H。
4. Boolean IsEmpty( MaxHeap H )：判断最大堆H是否为空。
5. ElementType DeleteMax( MaxHeap H )：返回H中最大元素(高优先级)。

下面给出它的实现：

```c
#include<stdio.h>
#include<stdlib.h>

typedef struct heap* Heap;

struct heap{
    int *array;
    int size;/* 记录当前值 */
    int max; /* 记录最大值 */
};

Heap createHeap(int max){
    Heap heap = (Heap)malloc(sizeof(struct heap));
    heap->size = 0;
    heap->array = (int *)malloc(sizeof(int) * (max + 1));
    heap->max = max;
    return heap;
}

void swap(Heap heap,int i,int j){
    int temp = heap->array[i];
    heap->array[i] = heap->array[j];
    heap->array[j] = temp;
}

void insert(Heap heap,int val){
    /* 先判断是否满了 */
    if(heap->size+1 > heap->max){
        printf("堆满\n");
        return;
    }
    /* 先将val插入到数组最后 */
    heap->array[++heap->size] = val;
    /* 进行上浮 */
    int k = heap->size;
    printf("k = %d val = %d\n",k,heap->array[heap->size]);
    while(k>1 && heap->array[k] > heap->array[k/2]){
        swap(heap,k,k/2);
        k = k/2;
    }
}

int delMax(Heap heap){
    /* 先判断是否空了 */
    if(heap->size < 1){
        printf("堆空\n");
        return -1;
    }
    /* 先保存最大值 */
    int ret = heap->array[1];
    /* 把最后一个交换到第一个，然后进行下沉 */
    swap(heap,1,heap->size);
    heap->size--;
    int k = 1;
    while(2*k <= heap->size){/* 如果没有孩子不能交换 */
        /* 找到两个孩子中较大的那个 */
        int j = k*2;
        if(j<heap->size && heap->array[j] < heap->array[j+1]){
            j++;
        }
        /* 如果k大于两个，结束 */
        if(heap->array[k] >= heap->array[j]){
            break;
        }
        swap(heap,k,j);
        k = j;
    }
    return ret;
}

void freeHeap(Heap heap){
    if(heap != NULL && heap->array != NULL){
        free(heap->array);
    }
    if(heap != NULL){
        free(heap);
    }
}

int main(int argc, char const *argv[])
{
    Heap heap = createHeap(100);
    int i;
    for(i=0;i<20;i++){
        insert(heap,rand()%100);
    }
     for(i=0;i<20;i++){
        printf("%d ",delMax(heap));
    }
    freeHeap(heap);
    return 0;
}
```

这个的实现个人看来还是相当简单的。值得注意的是，这里插入之后，就不能改变元素的属性了。当然，这里的示例代码是没办法改，但是对于指针类型的，就应当注意了。对于堆排序，我开始以为就是创建一个堆，然后插入之后再弹出。而事实上却不是这样的。因为堆的内部就是一个数组实现，而且在初始化堆的时候用了一些技巧，还是很值得学习的，下面给出代码。

```c
#include <stdio.h>
#include <stdlib.h>

void swap(int *arr, int x, int y){
    int temp = arr[x];
    arr[x] = arr[y];
    arr[y] = temp;
}

void sink(int *arr, int k, int n){
    while (2 * k <= n)
    {
        int j = k * 2;
        if (j < n && arr[j] < arr[j + 1]){
            j++;
        }
        if (arr[k] >= arr[j]){
            break;
        }
        swap(arr, k, j);
        k = j;
    }
}

void heapSort(int *arr, int n){
    /* 先构造堆，从第一个父节点开始，执行下沉操作 */
    int k;
    for (k = n / 2; k >= 1; k--){
        sink(arr, k, n);
    }
    k = n;
    /* 一次弹出最大的点，然后执行下沉操作 */
    while (n > 1){
        swap(arr, 1, n);
        sink(arr, 1, --n);
    }
    /* 排列第一个 */
    for(n=0;n<k;n++){
        if(arr[n] > arr[n+1]){
            swap(arr,n,n+1);
        }
    }
}

int main(int argc, char const *argv[]){
    int arr[20];
    int i;
    for (i = 0; i < 20; i++){
        arr[i] = rand() % 100;
    }
    heapSort(arr, 19);
    for (i = 0; i < 20; i++){
        printf("%d ", arr[i]);
    }
    return 0;
}
```
这个代码我写出来没花多少时间，但是调试却是花了不少时间，因为我一个地方的细节写错了，其实就是不小心，或者说没有注意到，把index搞成了数组在index的值。
##线性时间排序
 一般的排序算法在最坏的情况下需要$O(NlogN)$时间，但是在某些特殊情况下以线性时间排序仍然是可能的。先看一个桶排序的例子：

```c
#include<stdio.h>

void countingSort(int array[],int size){
	int countingArray[100];
	int i = 0;
	for(i=0;i<100;i++){
		countingArray[i] = 0;
	}
	for(i=0;i<size;i++){
		countingArray[array[i]]++;
	}
	for(i=1;i<100;i++){
		countingArray[i] += countingArray[i-1]; 
		
	}
	int temp[size];
	for(i=size-1;i>=0;i--){
		temp[countingArray[array[i]]-1] = array[i];
		countingArray[array[i]]--;
	}
	for(i=0;i<size;i++){
		array[i] = temp[i];
	}
}

int main(){
	int array[] = {1,7,5,89,42,32,32};
	int size = sizeof(array)/sizeof(int);
	countingSort(array,size);
	int i;
	for(i=0;i<size;i++){
		printf("%d ",array[i]);
	}
	return 0;
}
```
在这种算法中，有一个额外的数组，且比较的数必须落在数组的长度以内。它的思想很简单，先将这个数组初始化为0，然后遇到一个数字，索引对应的数字加1。可以看出，这种方法有局限，且需要的额外空间较大。为了节省空间，有一种新的方法，基数排序。由于这个需要变长的数组，就用Java实现，而不使用C语言。

```c
public class RadixSort {
	public static void radixSort(String[] strings,int stringLen) {
		//初始化桶
		final int BUCKETS = 256;
		ArrayList<String>[] buckets = new ArrayList[BUCKETS];
		for(int i=0;i<BUCKETS;i++) {
			buckets[i] = new ArrayList<>();
		}
		//从最后一个字符开始，一共要stringLen轮
		for(int i=stringLen-1;i>=0;i--) {
			//将string放入对应的桶中
			for(int j=0;j<strings.length;j++) {
				char c = strings[j].charAt(i);
				buckets[c].add(strings[j]);
			}
			int index = 0;
			//将桶中的字符串拷贝回去，进行下一轮排序
			for (ArrayList<String> arrayList : buckets) {
				for (String string : arrayList) {
					strings[index++] = string;
				}
				arrayList.clear();
			}
		}
	}
	public static void main(String[] args) {
		String[] strings = {"cab","bac","abc","bca","acb","cba"};
		radixSort(strings, 3);
		for (String string : strings) {
			System.out.println(string);
		}
	}
}
```
这种方法和桶排序的思想是一样的，只不过用了时间换空间。





