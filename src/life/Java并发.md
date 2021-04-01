##使用线程
有三种使用线程的方法：

* 实现 Runnable 接口；
* 实现 Callable 接口；
* 继承 Thread 类。

实现 Runnable 和 Callable 接口的类只能当做一个可以在线程中运行的任务，不是真正意义上的线程，因此最后还需要通过 Thread 来调用。可以说任务是通过线程驱动从而执行的。

```
public static void main(String[] args) {
    FutureTask<Integer> task = new FutureTask<>(new Callable<Integer>() {
        @Override
        public Integer call() throws Exception {
            return 1;
        }
    });
    Thread thread = new Thread(task);
    thread.start();
    try {
        System.out.println(task.get()); /* 阻塞等待 */
    } catch (Exception e) {
        e.printStackTrace();
	}
}
```

