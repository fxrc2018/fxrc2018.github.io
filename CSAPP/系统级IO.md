## 系统级IO

### 文件的基本操作
一个Linux文件就是一个m个字节的序列：

$$B_0,B_1, ... ,B_k, ... ,B_(m-1)$$

所有的I/O设备（例如网络、磁盘和终端）都被模型化为文件，而所有的输入和输出都被当作对应的文件的读写来执行。这种将设备优雅地映射为文件的方式，允许Linux内核引出yield简单、低级的应用接口，称为Unix I/O，这使得所有的输入输出都能以一种统一且一致的方式来执行：

* 打开文件。一个应用程序通过要求内核打开相应的文件，来宣告它想要访问一个I/O设备。内核返回一个小的非负整数，叫做描述符，它在后续对此文件的所有操作中标识这个文件。内核记录有关这个打开文件的所有信息。应用程序只需要记住这个描述符。Linux shell创建的每个进程开始时都有三个打开的文件：标准输入（描述符为0，宏为STDIN_FILENO），标准输出（描述符为1，宏为STDOUT_FILENO），标准错误（描述符为2，STDERR_FILENO）。
* 改变当前的文件位置。对于每个打开的文件，内核保持着一个文件位置k，初始值为0。这个文件位置是从文件开始的字节偏移量。应用程序能够通过执行seek操作，显式地设置文件的当前位置k。
* 读写文件。一个读操作就是从文件复制n>0个字节到内存，从当前文件位置k开始，然后将k增加到k+n。给定一个大小为m字节的文件，当k≥m时执行读操作会触发一个end-of-file（EOF）条件，应用程序能检测到这个条件。在文件末尾并没有明确的“EOF符号”。类似的，写操作就是从内存复制n>0个字节到一个文件，从文件的当前位置k开始，然后更新k。
* 关闭文件。当应用完成了对文件的访问之后，它就通知内核关闭这个文件。作为响应，内核释放文件打开时创建的数据结构，并将这个描述符恢复到可用的描述符池中。无论一个进程因为何种原因终止时，内核都会关闭所有打开的文件并释放它们的内存资源。

下面是函数的原型：

```cpp
#include <sys/types.h>
 #include <unistd.h>
/* 文件名，多个掩码位，模式（一般为0），成功返回文件描述符，失败返回-1 */
int open(char *filename,int flags,mode_t mode);
/* 成功返回0，失败返回-1.关闭一个已关闭的文件描述符会出错 */
int close(int fd);
/* 成功返回读取的字节数，遇到EOF返回0，出错返回-1 */
ssize_t read(int fd,void *buf,size_t n);
/* 若成功返回写入的字节数，出错返回-1 */
ssize_t write(int fd,const void *buf,size_t n); 
/* 成功返回新的文件偏移量；若出错，则返回-1，可以从文件开始SEEK_SET，当前位置SEEK_CUR,和文件末尾SEEK_END开始偏移 */    
off_t lseek(int fd, off_t offset, int whence);
```

在某些情况下，read和write传送的字节比应用程序要求要少。这些不足值不表示有错误。出现这样情况的原因有：
* 读时遇到EOF。
* 从终端读文本行。每次read函数将一次传送一个文本行，返回的不足值得大小等于文本行的大小。
* 读写网络套接字或Linux管道。

下面是一个例子：

```cpp
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>

int main(){
	/* 向屏幕输出 */
	char s[] = "hello world\n";
	char s2[] = "\n--io test\n"; 
	char buf[128];
	write(STDOUT_FILENO,s,strlen(s));
	int fd1;
	int ret;/* 用来接收返回值 */	
	fd1 = open("1.txt",O_CREAT|O_TRUNC|O_RDWR,0);
	/* 从标准输入读数据写入文件 */
	while((ret = read(STDIN_FILENO,buf,128)) >0){
		write(fd1,buf,ret);
	}
	/* 在末尾加上其他东西 */
	lseek(fd1,0,SEEK_END);
	write(fd1,s2,strlen(s2));
	close(fd1);
	return 0;
}
```

### readn、writen和readline函数
readn、writen和readline函数在网络编程中相当重要，一个重要原因是TCP是一个流协议，协议本身并没有维护消息的边界，可能出现粘包的情况。所以需要应用层自己维护消息的边界，最常用的有两种：发送定长的消息和使用特殊字符分割消息。特殊字符一般都是换行符，所以三个函数应用相当广泛。对于它们的实现方法也是很值得学习的。先看readn和writen的实现方法。

```cpp
ssize_t readn(int fd, void *buf,size_t n){
        size_t nleft = n;
        ssize_t nread;
        char *ptr = (char *)buf;

        while(nleft > 0){
                if((nread = read(fd,ptr,nleft)) < 0){
                        if(errno == EINTR){
                                continue;
                        }else{
                                return -1;
                        }
                }else if(errno == 0){ /* EOF */
                        break;
                }
                nleft -= nread;
                ptr += nread;
        }
        return n - nleft;
}

ssize_t writen(int fd, void *buf, size_t n){
        size_t nleft = n;
        ssize_t nwriten;
        char *ptr = (char *)buf;

        while(nleft > 0){
                if((nwriten = write(fd,ptr,nleft)) <= 0){
                        if(errno == EINTR){
                                continue;
                        }else{
                                return -1;
                        }
                }
                nleft -= nwriten;
                ptr += nwriten;
        }
        return n;
}
```
这里实现的技巧就是通过循环，记录读写的位置，中断后重启来实现读写固定的字节数。对于readline函数的实现，我先展示（不是我写的）一个简单的版本，然后再介绍书中的实现方法。

```cpp
ssize_t recv_peek(int fd, void *buf, size_t len){
        while(1){
                int ret = recv(fd,buf,len,MSG_PEEK);
                if(ret == -1 && errno == EINTR){
                        continue;
                }
                return ret;
        }
}

ssize_t readline(int fd, void *buf, size_t maxline){
        int ret;
        int nleft = maxline;
        int nread;
        char *ptr = (char *)buf;

        while(nleft > 0){
                ret = recv_peek(fd,ptr,nleft);
                if(ret < 0){
                        return -1;
                }else if(ret == 0){ /* EOF */
                        return maxline - nleft;
                } 
                nread = ret;
                int i;
                for(i=0;i<nread;i++){
                        if(ptr[i] == '\n'){
                                ret = readn(fd,ptr,i+1);
                                if(ret != i+1){ /* imposible for failure if work correctly*/
                                        exit(EXIT_FAILURE);
                                }
                                return maxline - nleft + i + 1;
                        }
                }
                ret = readn(fd,ptr,nread);
                if(ret != nread){
                        exit(EXIT_FAILURE);
                }
                nleft -= nread;
                ptr += nread;
        }
        return maxline;
} 
```

这里的实现和readn函数比较像，值得注意的是这样一个技巧，就是需要预读数据，书上的方法的思想和这个也是一样的，因为我们不知道什么时候才能读取到换行符，所以只能一个字符一个字符的读，但是这样的读取效率太低了，所以比较好的方法是一次读取多个，然后从缓存里面读；这样可以避免频繁的使用系统调用。其实，这个方法的实现，只是比readn函数多了一个预读和检测换行符的操作。

下面给出书上的实现，书中的实现方法差不多，只不过是先把文件读到一个缓冲区中，然后在进行类似的操作；而不是看一下文件内容，再进行读取。

```cpp
typedef struct{
        int fd;
        int cnt;
        char *ptr;
        char buf[BUFSIZE];
}rio_t;

void rio_init(rio_t *rp, int fd){
        rp->fd = fd;
        rp->cnt = 0;
        rp->ptr = rp->buf;
}

ssize_t rio_read(rio_t *rp, char *buf, size_t n){
        int cnt;
        /* refill if buf is empty,read fd until success */
        while(rp->cnt <= 0){
                if((rp->cnt = read(rp->fd,rp->buf,sizeof(rp->buf))) < 0){
                        if(errno != EINTR){
                                return -1;
                        }else if(rp->cnt == 0){
                                return 0; /* EOF */
                        }
                }else{
                        rp->ptr = rp->buf;
                }
        }

        /* read from rp->buf */
        cnt = n;
        if(rp->cnt < cnt){
                cnt = rp->cnt;
        }
        memcpy(buf,rp->buf,cnt);
        rp->cnt -= cnt;
        rp->ptr += cnt;
        return cnt;
}

ssize_t readline(rio_t *rp, void *buf, size_t maxline){
        int n, rc;
        char c,*ptr = (char *)buf;
        for(n=1;n<maxline;n++){
                if((rc = rio_read(rp,&c,1)) == 1){
                        *ptr++ = c;
                        if(c == '\n'){
                                break;
                        }
                }else if(rc == 0){
                        if(n == 1){
                                return 0; 
                        }else{
                                break;
                        }
                }else{
                        return -1;
                }
        }
        *ptr = '\0';
        return n -1;
}
```

### 文件、目录和IO重定向

下面介绍几个额外的操作I/O的方法：

```cpp
/* 检查文件是否存在或是否有读、写、执行权限 */
int access(const char *pathname, int mode);
/* 读取文件的源数据到buf，成功返回0，失败返回-1 */
int stat(const char *filename,struct stat *buf);
int fstat(int fd,struct stat *buf);
/* 打开目录,成功返回指针，失败返回NULL */
DIR *opendir(const char *name);
/* 每次调用，指向下一个目录项，若果没有更多目录返回NULL */
struct dirent *readdir(DIR *dirp);
/* 关闭目录,成功返回0，失败返回-1 */
int closedir(DIR *dirp);
/* 把newfd改为oldfd */
int dup2(int oldfd,int newfd);
```

下面是一个例子：

```cpp
#include<dirent.h>
#include<unistd.h>
#include<sys/stat.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

/* 遍历目录，打印文件名 */
int main(int argc,char *argv[]){
	struct dirent *file;
	DIR *path;
	int fd1;
	fd1 = open("file.txt",O_CREAT|O_TRUNC|O_RDWR,0);
	/* 把标准输出流替换为文件 */
	dup2(fd1,STDOUT_FILENO);
	path = opendir(argv[1]);
	struct stat buf;
	while((file = readdir(path))!= NULL){
		/* 只打印文件和目录 */
		stat(file->d_name,&buf);
		if(S_ISREG(buf.st_mode)){
			printf("file %s\n",file->d_name);
		}else if(S_ISDIR(buf.st_mode)){
			printf("dir %s\n",file->d_name);
		}
	}
	close(fd1);
	closedir(path);	
	return 0;
}
```
### 文件共享

想要共享Linux文件，我们首先要了解内核是如何表示打开的文件。内核用三个相关的数据结构来表示打开的文件：
* 描述符表。每个进程都有它独立的描述符表，它的表项是由进程打开的文件描述符来索引的。每个打开的描述符表项指向文件表中的一个表项。
* 文件表。打开文件的集合是由一张文件表来表示的，所有的进程共享这张表。每个文件表的表项组成包括当前的文件位置，引用计数，以及一个指向v-node表中对应表项的指针。关闭一个文件描述符会减少相应的文件表项中的引用计数。内核不会删除这个表项，直到它的引用计数为0。
* v-node表。同文件表一样，所有的进程共享这张v-node表。每个表项包含stat结构中大多数的信息，包括st_mode和st_size成员。

可以这样理解，描述符表相当于是文件表的句柄，句柄的意思，我的理解是，持有句柄，就可以操作句柄绑定的对象，和指针类似。文件表是在程序运行时才有的一种结构，它表明了对文件的操作状态，不同的进程可以对同一个文件进行操作。而v-node表是描述文件在磁盘上真实存在的状态，如大小、类型等信息。对于同一个文件，v-node表应该只有一张（在网上没有找到结论）。如下面的例子：

```cpp
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(){
        int fd1,fd2;
        char c1,c2;
        if(access("test.txt",F_OK) == -1){
                printf("file not exit.\n");
                return 0;
        }
        fd1 = open("test.txt",O_RDWR,0);
        fd2 = open("test.txt",O_RDWR,0);
        read(fd1,&c1,1);
        read(fd2,&c2,1);
        printf("c1 = %c, c2 = %c.\n",c1,c2);
        ftruncate(fd1,100);
        struct stat buf;
        fstat(fd2,&buf);
        printf("file size = %ld.\n",buf.st_size);
        close(fd1);
        close(fd2);

        int fd = open("test.txt",O_RDONLY,0); 
        if(fork() == 0){
                read(fd,&c1,1);
                printf("c1 = %c, ",c1);
                exit(0);
        }
        wait(NULL);
        read(fd,&c2,1);
        printf("c2 = %c.\n",c2);
        return 0;
}
```

对于一个只有6个字符“123456”的文件，输出的结果如下：

```shell
c1 = 1, c2 = 1.
file size = 100.
c1 = 1, c2 = 2.
```

这个结果的解释是，第一个是不同的文件文件表，所以都是从头开始读的，第二次是同一个文表，所以父进程读的是第二个字符。因为v-node表只有一张，所以虽然fd1和fd2是不同的文件表，但是它们指向的v-node表是同一张表，所以通过fd2得到的文件大小已经是更新过的了。
