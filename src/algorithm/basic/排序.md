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
### 选择排序
选择排序非常简单，每次选择最大的一个数，放到指定的位置即可。

```cpp
void selectSort(vector<int> &arr){
    for(int i=arr.size()-1;i>0;i--){
        int maxPos = 0;
        for(int j=1;j<=i;j++){
            if(arr[j] > arr[maxPos]){
                maxPos = j;
            }
        }
        swap(arr[i],arr[maxPos]);
    }
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

### 堆排序

可以利用堆的性质来进行排序。代码如下：

```cpp
//堆的下沉操作
void heapSink(vector<int> &arr, int pos, int len){
    int parent = pos;
    int child = parent * 2 + 1;
    int num = arr[pos];
    while(child < len){
        if(child + 1 < len && arr[child+1] > arr[child]){
            child++;
        }
        if(num > arr[child]){
            break;
        }
        arr[parent] = arr[child];
        parent = child;
        child = parent * 2 + 1;
    }
    arr[parent] = num;
}

void heapSort(vector<int> &arr){
    for(int i=arr.size()/2;i>=0;i--){
        heapSink(arr,i,arr.size()); //通过有规律的下沉操作，迅速构造堆
    }
    for(int i=arr.size()-1;i>0;i--){
        swap(arr[0],arr[i]); //每次弹出最大的一个数
        heapSink(arr,0,i); //执行下沉操作
    }
}
```

### 桶排序

以上的排序都是基于比较，但如果在取值比较少的情况下，可以用桶排序实现线性时间的排序。桶排序主要思想是，先按照值将数据放在桶里，然后按照值的大小依次统计每个桶的数量，实现排序。这种排序方式只适用于值的取值范围比较小的情况。

```cpp
void bucketSort(vector<int> &arr){
    //假设arr[i]的取值范围为0 - 100
    vector<int> bucket(101,0);
    for(int num : arr){
        bucket[num]++;
    }
    int k = 0;
    for(int i=0;i<=100;i++){
        for(int j=0;j<bucket[i];j++){
            arr[k++] = i;
        }
    }
}
```