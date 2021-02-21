Handler机制在Android中算是一种比较重要的机制，它的实现原理相对来说，还是比较简单的。它的简单类图如下。

![Handler简单类图.png](https://upload-images.jianshu.io/upload_images/10373084-da0b6a952b0eebd8.png?imageMogr2/auto-orient/strip%7CimageView2/2/w/1240)

这个类图并不复杂，但这个类的许多写法还是有值得借鉴的地方。先View类中说起，View类中，子线程是无法更新主线程的东西的，但是，是怎么做到这一点的呢，其实很简单，只要几行代码就可以了。
```
public class TextView {
	
	private Thread mThread;
	
	public TextView() {
		mThread = Thread.currentThread();
	}
	
	public void setText(String str) {
		checkThread();
		System.out.println("UI线程更新："+str);
	}

	private void checkThread() {
		if(mThread!=Thread.currentThread()) {
			throw new RuntimeException("Only the original thread "
			+ "that created a view hierarchy can touch its views.");
		}
	}
}
```

在构造的时候，将构造的线程传进去，然后在更新的时候，再去判断当前线程是不是主线程就可以了。下面在看看Looper类的写法：
```
public class Looper {
	private static final ThreadLocal<Looper> sThreadLocal = 
                                        new ThreadLocal<Looper>();
	
	public MessageQueue mQueue;
	
	public Looper() {
		mQueue = new MessageQueue();
	}
	
	public static void prepare() {
        sThreadLocal.set(new Looper());
	}

	public static void loop() {
		Looper looper = myLooper();
		for(;;){
			MessageQueue queue = looper.mQueue;
			Message message = queue.next();
			if(message == null){
				return;
			}
			message.target.handleMessage(message);
		}
	}

	public static Looper myLooper() {
		return sThreadLocal.get();
	}	
}
```
我觉得主要值得学习的是ThreadLocal类的用法，这样的写法保证了一个线程，只有一个Looper，并且可以通过静态方法获取到这个Looper。Looper的loop函数很好理解，就是从MessageQueue中获取Message，然后得到Handler，然后对用handleMessage方法。下面是Handler类的东西：
```
public class Handler {

	MessageQueue mQueue;
	
	public Handler() {
		Looper looper = Looper.myLooper();
		if(looper == null){
			throw new RuntimeException(
	                "Can't create handler 
                       inside thread that has not                 
                       called Looper.prepare()");
		}
		mQueue= looper.mQueue;
	}
	
	public void sendMessage(Message message) {
		sendMessageDelayed(message,0);
	}
	
	public final boolean sendMessageDelayed(Message msg, long delayMillis)
    {
        if (delayMillis < 0) {
            delayMillis = 0;
        }
        return sendMessageAtTime(msg, System.currentTimeMillis() 
                + delayMillis);
    }
	
	public boolean sendMessageAtTime(Message msg, long uptimeMillis) {
        MessageQueue queue = mQueue;
        return enqueueMessage(queue, msg, uptimeMillis);
    }
	
	private boolean enqueueMessage(MessageQueue queue, Message msg, 
                  long uptimeMillis) {
        msg.target = this;
        return queue.enqueueMessage(msg, uptimeMillis);
    }
	
	public void handleMessage(Message msg) {}
}
```
Handler中的handleMessage方法就是把消息加入到MessageQueue中去。然后看看MessageQueue这个类：
```
public class MessageQueue {

	private Message mMessages;

	public boolean enqueueMessage(Message msg, long when) {
		synchronized (this) {
			msg.when = when;
			Message p = mMessages;
			if (p == null || when == 0 || when < p.when) {
				// New head, wake up the event queue if blocked.
				msg.next = p;
				mMessages = msg;
			} else {
				Message prev;
				for (;;) {
					prev = p;
					p = p.next;
					if (p == null || when < p.when) {
						break;
					}
				}
				msg.next = p; // invariant: p == prev.next
				prev.next = msg;
			}
		}
		return true;
	}

	public Message next() {
		for (;;) {
			synchronized (this) {
				final long now = System.currentTimeMillis();
				Message prevMsg = null;
				Message msg = mMessages;
				if (msg != null && msg.target == null) {
					do {
						prevMsg = msg;
						msg = msg.next;
					} while (msg != null);
				}
				if (msg != null) {
					if (now < msg.when) {
					} else {
						if (prevMsg != null) {
							prevMsg.next = msg.next;
						} else {
							mMessages = msg.next;
						}
						msg.next = null;
						return msg;
					}
				} else {
				}
			}
		}
	}
}
```
MessageQueue主要提供了两个方法，一个是把加入的Message按照时间排序，然后提供一个方法返回Message。Message类就是一个链表。
```
public class Message {
	public Object obj;
	public Handler target;
	public long when;
	public Message next;
}
```
在主线程中已经实现了一个自动调用了Looper的两个方法，所以不用去自己调用，而在子线程中需要自己显示的去调用。Handler这样一种架构，我觉得还是很值得学习的。

