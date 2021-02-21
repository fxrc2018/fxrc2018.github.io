### 协程的概念

协程又可以称为用户线程,微线程，可以将其理解为单个进程或线程中的多个用户态线程，这些微线程在用户态进程控制和调度。

进程,线程以及协程的设计都是为了并发任务可以更好的利用CPU资源，他们之间最大的区别在于CPU资源的使用上:

- 进程和线程的任务调度是由内核控制的，是抢占式的；
- 协程的任务调度是在用户态完成,需要代码里显式地将CPU交给其他协程,是协作式的

由于我们可以在用户态调度协程任务，所以我们可以把一组相互依赖的任务设计为协程。这样,当一个协程任务完成之后,可以手动的进行任务切换，把当前任务挂起(yield),切换到另一个协程区工作.由于我们可以控制程序主动让出资源，很多情况下将不需要对资源进行加锁。

## Unity中的协程
Unity中的协程提供了轻型的并行概念，可以同步并行逻辑，同时可以避免多线程带来的逻辑麻烦。

协程可以实现在某些点的同步，可以将复杂逻辑分帧，可以在主线程内非阻塞的运行一个持续性逻辑。

Unity中协程的实现借助了C#的迭代器，每次yeild都会离开本次执行，迭代器会记住这个位置和上下文，下一次update到这个协程上次离开的位置开始执行。

### Unity协程的几种常用操作

Unity中的协程有以下几种常见的返回类型。

```csharp
yeild return null; //等到下一帧继续执行
yeild return new WaitForSeconds(1.0f); //等待1秒后执行
yeild return StartCoroutine(); //等待另一个协程结束
yeild return WWW(); //开启异步下载
```

#### 延迟执行一段代码

```csharp
IEnumberator Work(){
    yeild return new WaitForSeconds(3.0f);
    //do Work
}
```

#### 将复杂的操作分散到多个帧

```csharp
IEnumberator Work(){
    for(int i=0;i<10000;i++){
        res += i;
        yeild return null;
    }
}
```

#### 串联几个连续性的逻辑

```csharp
IEnumberator Work1(){
    Debug.log("Work1");
}

IEnumberator Work2(){
    yeild rturn StartCoroutine(Work1());
    Debug.log("Work2");
}

```

