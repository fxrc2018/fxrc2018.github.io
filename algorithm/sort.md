## 排序

这里讨论的排序还有如下两个特点：
- 只讨论基于比较的排序（> = < 有定义）
- 只讨论内部排序（排序工作能够在主存中完成） 

大多数情况下，为简单起见，讨论从小大的整数排序。这里使用的函数模型是：

```cpp
void xSort(vector<int> &arr)
```

除了面试外，在日常的工作中，我们是不可能去手写排序算法了，但是了解排序算法的工作原理还是很有意义的，里面使用的一些技巧很值得借鉴。

在排序算法中，有一个稳定性的概念，即任意两个相等的数据， 排序前后的相对位置不发生改变。

先给出我用于测试排序函数正确性的测试函数，本文所有的排序算法都只通过了这个测试函数，所有有可能有一些BUG。

```cpp
void testSortFunc(void(*sortFunc)(vector<int>&)){
    int TIMES = 10000;
    int SIZE = 100;
    for(int i=0;i<TIMES;i++){
        vector<int> arr;
        for(int j=0;j<SIZE;j++){
            arr.push_back(rand());
        }
        vector<int> arr1 = arr, arr2 = arr;
        sort(arr1.begin(),arr1.end());
        sortFunc(arr2);
        for(int j=0;j<SIZE;j++){
            if(arr1[j] != arr2[j]){
                cout<<"error"<<endl;
                return;
            }
        }
    }
    cout<<"pass all test"<<endl;
}
```

### 冒泡排序
冒泡排序的思想十分简单，就是每次找到最大/最小的数，然后重复N次。所以，写这种程序还是比较简单的，下面直接给出冒泡排序的代码：

```cpp
void bubbleSort(vector<int> &arr){
    for(int i = arr.size()-1;i>0;i--){
        //通过交换，每次将剩余的最大数调整到i
        for(int j=0;j<i;j++){
            if(arr[j] > arr[j+1]){
                swap(arr[j],arr[j+1]);
            }
        }
    }
}
```

### 插入排序
插入排序的算法就好像打牌的时候，摸牌之后的操作。这个算法个人感觉比冒泡排序要难一点。

```cpp
void insertSort(vector<int> &arr){
    for(int i = 1;i<arr.size();i++){
        int num = arr[i];
        int j;
        //如果前一个数大于当前数，将前一个数后移一位，然后继续和下一个数比较
        for(j = i;j>0 && arr[j-1] > num;j--){
            arr[j] = arr[j-1];
        }
        //停止的时候就到了正确的位置
        arr[j] = num;
    }
}
```

### 希尔排序
希尔排序其实和简单排序差不多，只不过是每次是个n个进行排序。这个我已经空了很久了，在K&R那本书里面看到了这个算法，然后就搬到了这里。下面是具体的代码：

```cpp
void shellSort(vector<int> &arr){
    for(int gap = arr.size() / 2; gap > 0; gap /= 2){
        for(int i = gap;i < arr.size(); i++){
            for(int j = i - gap;j >= 0 && arr[j] > arr[j + gap];j -= gap){
                swap(arr[j],arr[j+gap]);
            }
        }
    }
}
```

### 快速排序

快速排序是现在常用的算法，其核心是在线性时间内将一个数组以pivot(自己选择)为中点，使得比它大的都在其右边，比它小的都在其左边，然后进行递归调用，直到只剩下一个数字。

```cpp
void quickSort(vector<int> &arr, int left, int right){
    if(left >= right){ //递归出口
        return;
    }
    //随机选一个数作为参考数
    int len = right - left + 1;
    int randPos = rand() % len + left;
    swap(arr[left],arr[randPos]);
    //partition操作，执行之后，左边的数小于参考数，右边的数大于参考数
    int pos = left; //pos表示在pos右边的数都要大于arr[left]
    for(int i=left+1;i<=right;i++){
        if(arr[i] < arr[left]){
            swap(arr[i],arr[++pos]);
        }
    } 
    swap(arr[left],arr[pos]);
    
    quickSort(arr,left,pos-1);
    quickSort(arr,pos+1,right);
}

void quickSort(vector<int> &arr){
    quickSort(arr,0,arr.size()-1);
}
```

### 归并排序

归并排序和快速排序的复杂度都是O(nlogn)  ，它的基本思想是，在线性时间内，对已经排序的数组合在一起。为了这样做，它需要额外的空间。它的代码如下：

```cpp
void mergeSort(vector<int> &arr1, vector<int> &arr2, int left, int right){
    if(left >= right){
        return;
    }
    int mid = left + (right - left) / 2;
    mergeSort(arr1,arr2,left,mid);
    mergeSort(arr1,arr2,mid+1,right);

    //到这里的时候可以认为，两个数组都排好序了，执行merge操作
    int k = left;
    int i = left;
    int j = mid + 1;
    while(i <= mid && j <= right){
        if(arr1[i] <= arr1[j]){
            arr2[k++] = arr1[i++];
        }else{
            arr2[k++] = arr1[j++];
        }
    }
    while(i <= mid){
        arr2[k++] = arr1[i++];
    }
    while(j <= right){
        arr2[k++] = arr1[j++];
    }
    for(i=left;i<=right;i++){
        arr1[i] = arr2[i];
    }
}

void mergeSort(vector<int> &arr){
    vector<int> arr2(arr.size(),0);
    mergeSort(arr,arr2,0,arr.size()-1);
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





