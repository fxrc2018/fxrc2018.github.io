## System-V进程间通信

进程间通信可能出于以下的目的。
* 数据传输：一个进程需要将它的数据发送给另一个进程。
* 资源共享：多个进程之间共享同样的资源。
* 通知事件：一个进程需要向另一个或一组进程发送消息，通知它（它们）发生了某种事件，如进程终止时要通知父进程。
* 进程控制：有些进程希望完全控制另一个进程的执行，如Debug进程，此时控制进程希望能够拦截另一个进程的所有陷入和异常，并能够及时知道它的状态改变。

进程间通信需要IPC对象，IPC对象有以下几种持续性。
* 随进程持续：一直存在知道打开的最后一个进程结束，如pipe和FIFO。
* 随内核持续：一直存在知道内核自举或显式删除，如System V消息队列、共享内存、信号量。
* 随着文件系统持续：一直存在知道显式删除，及时内核自举还存在。如POSIX消息队列、共享内存、信号量如果是使用映射文件来实现。

进程间通信有两种基本模型：共享内存和消息传递。共享内存模型会建立起一块供协作进程共享的内存区域，进程通过向此共享内存区域读入或写入数据来交换信息。消息传递模型通过在协作进程间交换消息来实现通信。

### System V 消息队列

消息队列这个模型，简单来说，就是一个公共存储区，只不过存和取都是以消息为单位。只要知道了这个公共存储区（非私有的），任何进程都可以往里面放消息，也可以从里面取消息。System V消息队列提供了以下函数。

```cpp
int msgget(key_t key, int oflag); /* 创建一个新的消息队列或访问一个已存在的消息队列 */
int msgsnd(int msqit, const void *ptr, size_t length, int flag); /* 向消息队列放置一个消息 */
 /* 从某个消息队列读出一个消息 */
size_t msgrcv(int msgid, void *ptr, size_t length, long type, int flag);
/* 消息队列上的各种操作 */
int msgctl(int msgid, int cmd, struct msgid_ds *buff); 
```

下面使用System V消息队列实现一个echo程序。为了简单起见，这里省略了头文件，先定义一些辅助数据和函数。

```cpp
#define ERR_EXIT(m) \
    do \
    { \
        perror(m);\
        exit(EXIT_FAILURE);\
    }while(0)

#define MSGMAX 1024
struct msgbuf{
    long mtype;
    int mpid;
    char mtext[MSGMAX];
};
```

这里直接给出服务端程序。

```cpp
void echo_srv(int msgid){
    struct msgbuf msg;
    memset(&msg,0,sizeof(msg));
    int n;
    while(1){
        if((n = msgrcv(msgid, &msg, MSGMAX, 1, 0)) < 0){
            ERR_EXIT("msgrcv");
        }
        int pid = msg.mpid;
        fputs(msg.mtext,stdout);
        msg.mpid = pid;
        msgsnd(msgid, &msg, n,0);
    }
}

int main(int argc, char const *argv[]){
    int msgid;
    if((msgid = msgget(1234,IPC_CREAT|0666)) == -1){
        ERR_EXIT("msgget");
    }
    echo_srv(msgid);
    return 0;
}
```
服务器端程序就是打开一个消息队列，然后循环等待接收一个消息编号为1的消息，如果收到了，就复制消息，把消息的编号改为发送者的pid，然后把它添加到消息队列中去。下面是客户端的代码。

```cpp
void echo_client(int msgid){
    struct msgbuf msg;
    int n;
    int pid = getpid();

    while((fgets(msg.mtext,MSGMAX,stdin)) != NULL){
        msg.mtype = 1;
        msg.mpid = pid;
        msgsnd(msgid,&msg,sizeof(int) + strlen(msg.mtext),0);
        memset(&msg,0,sizeof(msg));
        if((n = msgrcv(msgid, &msg, MSGMAX, pid, 0)) < 0){
            ERR_EXIT("msgrcv");
        }
        fputs(msg.mtext,stdout);
    }
}

int main(int argc, char const *argv){
    int msgid;
    if((msgid = msgget(1234,0)) == -1){
        ERR_EXIT("msgget");
    }
    echo_client(msgid);
}
```

### System V共享内存

共享内存区是最快的IPC形式。一旦这样的内存映射到共享它的进程地址空间，这些进程间数据传递不再涉及到内核，换句话说是进程不再通过执行进入内核的系统调用来传递彼此的数据。然而往该内存区存放信息或从中取走信息的进程间通常需要某种形式的同步。

共享内存，可以通过mmap映射同一个文件名来实现。但System V也提供了一些函数来实现共享内存。

```cpp
/* 创建一个新的共享内存区，或访问一个已经存在的共享内存区 */
int shmget(key_t key, size_t size, int oflag); 
/* 将共享内存区附接到调用进程的地址空间 */
void *shmat(int shmid, const void *shmaddr, int flag); 
int shmdt(const void *shmaddr); /* 断接内存区 */
int shmctl(int shmid, int cmd, struct shmid_ds *buff); /* 对一个共享区的多种操作 */
```

### System V信号量

信号量和P、V原语是由Dijkstra提出的，主要是为了解决同步和互斥问题。使用信号量来达到互斥，P、V在同一个进程中，而使用信号量来达到同步，P、V不再同进程中。

信号量的有以下的含义，S>0，S表示可用资源个数；S=0，表示无可用资源，无等待进程；S<0，S的绝对值表示等待队列中进程个数。

```cpp
struct semaphore{
	int value;
	pointer_PCB_queue;
}

P(s){
	s.value--;
    if(s.value < 0){
    	该进程状态置为等待状态
        将该进程的PCB插入相应的等待队列s.queue末尾
    }
}

V(s){
    s.value ++;
    if(s.value <= 0){
		唤醒相应等待队列s.queue中等待的一个进程
		改变其状态位就绪态
		并将其插入就绪队列
    }
}
```

System V提供了以下的函数。

```cpp
int semget(key_t key, int nsems, int oflag); /* 创建一个信号量集或访问一个已存在的信号量集 */
int semop(int semid, struct sembuf *opsptr, size_t nops); /* 对其中一个或多个信号量进行操作 */
/* 对一个信号量执行各种控制操作 */
int semctl(int semid, int semnum, int cmd, ... /* union semun arg */); 
```

我个人认为使用这种信号量的API编程对理解进程间的同步和互斥有很大的帮助，因为这个模型比较原始，对理解一些高级的API很有帮助。可以利用上面的函数进行一些简单的封装。

```cpp
int semCreate(key_t key){
	int semid;
	semid = semget(key,1,IPC_CREAT | IPC_EXCL | 0666);
	if(semid == -1){
		ERR_EXIT("semget");
	}
	return semid;
}

int semOpen(key_t key){
	int semid;
	semid = semget(key,0,0);
	if(semid == -1){
		ERR_EXIT("semget");
	}
	return semid;
}

int semSetVal(int semid, int val){
	union semun su;
	su.val = val;
	int ret = semctl(semid,0,SETVAL,su);
	if(ret == -1){
		ERR_EXIT("semctl");
	}
	return 0;
}

int semGetVal(int semid){
	int ret = semctl(semid,0,GETVAL,0);
	if(ret == -1){
		ERR_EXIT("semctl");
	}
	return ret;
}

int semDel(int semid){
	int ret = semctl(semid,0,IPC_RMID,0);
	if(ret == -1){
		ERR_EXIT("semctl");
	}
	return ret;
}

int semP(int semid){
	struct sembuf sb = {0,-1,0};
	int ret = semop(semid,&sb, 1);
	if(ret == -1){
		ERR_EXIT("semop");
	}
	return ret;
}

int semV(int semid){
	struct sembuf sb = {0,1,0};
	int ret = semop(semid,&sb,1);
	if(ret == -1){
		ERR_EXIT("semop");
	}
	return ret;
}
```
信号量的最简单的应用就是实现互斥。

```cpp
int semid;

void print(char c){
	int i;
	srand(getpid());
	for(i=0;i<10;i++){
		semP(semid);
		printf("%c",c);
		sleep(rand()%3);
		printf("%c\n",c);
		fflush(stdout);
		sleep(rand()%3);
		semV(semid);
	}
}

int main(int argc, char *argv[]){
	semid = semCreate(IPC_PRIVATE);
	semSetVal(semid,1);
	pid_t pid = fork();
	if(pid == -1){
		ERR_EXIT("fork");
	}
	if(pid == 0){
		print('O');
	}else{
		print('X');
		wait(NULL);
		semDel(semid);
	}
	return 0; 
}
```

可以使用信号量实现哲学家就餐问题，这里是利用可以对多个信号量同时操作的特性，解决死锁的方案是，一次性获取两只筷子。

```cpp
int semid = 0;

void wait2Fork(int no){
	int left = no;
	int right = (no + 1)%5;
	struct sembuf sb[2] = {
		{left,-1,0},
		{right,-1,0}
	};
	semop(semid,sb,2);
}

void free2Fork(int no){
	int left = no;
	int right = (no + 1) % 5;
	struct sembuf sb[2] = {
		{left,1,0},
		{right,1,0}
	};
	semop(semid,sb,2);
}

void philosopher(int no){
	srand(getpid());
	for(;;){
		printf("%d is thinking\n",no);
		sleep(rand()%3);
		wait2Fork(no);
		printf("%d is eating\n",no);
		sleep(rand()%3);
		free2Fork(no);	
	}	
}

int main(int argc, char *argv[]){	
	
	semid = semget(IPC_PRIVATE,5,IPC_CREAT | 0666);
	if(semid == -1){
		ERR_EXIT("semget");
	}
	union semun su;
	su.val = 1;
	int i;
	for(i=0; i<5; i++){
		int ret = semctl(semid,i,SETVAL,su);
		if(ret == -1){
			ERR_EXIT("semctl");
		}
	}

	int no = 0;
	pid_t pid;
	for(i=1; i<5; i++){
		pid = fork();
		if(pid == -1){
			ERR_EXIT("fork");
		}
		if(pid == 0){
			no = i;
			break;
		}
	}
	philosopher(no);
	return 0; 
}
```

### shmfifo的实现

shmfifo实现的是进程间的生产者消费者模式，主要是利用System V的共享内存和信号量。这里做了一些假设，每次生产的数据的大小是固定的，然后FIFO总的大小是不变的。个人认为生产者消费者问题，只要记住一句话即可，两个条件变量，一个互斥量。这里虽然都是用信号量来实现的，但是含义是差不多的。具体的代码如下。

```cpp
struct shmhead{
	unsigned int blksize;
	unsigned int blocks;
	unsigned int rdIndex;
	unsigned int wrIndex;
};

struct shmfifo{
	struct shmhead *head;
	char *payload;
	int shmid;
	int semMutex;
	int semFull;
	int semEmpty;
};

typedef struct shmfifo shmfifo_t;
typedef struct shmhead shmhead_t;

shmfifo_t * shmfifoInit(int key, int blksize, int blocks){
	shmfifo_t *fifo = (shmfifo_t *)malloc(sizeof(shmfifo_t));
	memset(fifo,0,sizeof(shmfifo_t));

	int shmid;
	shmid = shmget(key,0,0);
	int size = sizeof(shmhead_t) + blksize * blocks;
	if(shmid == -1){
		fifo->shmid = shmget(key,size, IPC_CREAT | 0666);
		if(fifo->shmid == -1){
			ERR_EXIT("shmget");
		}
		fifo->head = (shmhead_t *)shmat(fifo->shmid,NULL,0);
		if(fifo->head == (shmhead_t*)-1){
			ERR_EXIT("shmat");
		}
		fifo->payload = (char *)(fifo->head + 1); 
		fifo->semMutex = semCreate(key);
	   	fifo->semFull = semCreate(key + 1);
		fifo->semEmpty = semCreate(key + 2);	
		semSetVal(fifo->semMutex,1);
		semSetVal(fifo->semFull,blocks);
		semSetVal(fifo->semEmpty,0);
		shmhead_t *head = fifo->head;
		head->blksize = blksize;
		head->blocks = blocks;
		head->rdIndex = 0;
		head->wrIndex = 0;
	}else{
		fifo->shmid = shmid;
		fifo->head = (shmhead_t *)shmat(fifo->shmid,NULL,0);
		if(fifo->head == (shmhead_t*)-1){
			ERR_EXIT("shmat");
		}		
		fifo->payload = (char *)(fifo->head + 1); 
		fifo->semMutex = semOpen(key);
	   	fifo->semFull = semOpen(key + 1);
		fifo->semEmpty = semOpen(key + 2);	

	}

	return fifo;
}

void shmfifoPut(shmfifo_t *fifo, const void *buf){
	semP(fifo->semFull);
	semP(fifo->semMutex);
	shmhead_t *head = fifo->head;
	memcpy(fifo->payload + head->blksize * head->wrIndex,buf,head->blksize);
	head->wrIndex = (head->wrIndex + 1)%head->blocks;

	semV(fifo->semMutex);
	semV(fifo->semEmpty);	
}

void shmfifoGet(shmfifo_t *fifo, char *buf){
	semP(fifo->semEmpty);
	semP(fifo->semMutex);
	shmhead_t *head = fifo->head;
	memcpy(buf,fifo->payload + head->blksize * head->rdIndex,head->blksize);
	head->rdIndex = (head->rdIndex + 1)%head->blocks;

	semV(fifo->semMutex);
	semV(fifo->semFull);
}

void shmfifoDestroy(shmfifo_t *fifo){
	semDel(fifo->semMutex);
	semDel(fifo->semFull);
	semDel(fifo->semEmpty);
	shmdt(fifo->head);
	shmctl(fifo->shmid,IPC_RMID,0);
	free(fifo);
}

struct student{
	char name[20];
	int age;
};
```

生产者的代码如下。

```cpp
int main(){
	shmfifo_t * fifo = shmfifoInit(1232,sizeof(struct student),3);
	struct student s;
	strcpy(s.name,"LiMing");
	s.age = 20;
	int i;
	for(i=0;i<5;i++){
		s.age++;
		printf("put %d\n",s.age);
		shmfifoPut(fifo,&s);
	}
	sleep(3);
	shmfifoDestroy(fifo);
	return 0;
}
```
消费者的代码如下。

```cpp
int main(){
	shmfifo_t *fifo = shmfifoInit(1232,sizeof(struct student),3);
	int i;
	struct student s;
	for(i=0;i<5;i++){
		shmfifoGet(fifo,(char *)&s);
		printf("%s %d\n",s.name,s.age);
	}
	return 0;
}
```
