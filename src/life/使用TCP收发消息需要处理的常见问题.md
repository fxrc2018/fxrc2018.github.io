要实现服务器和客户端之间完整的收发消息，需要处理以下的几个问题。
##粘包半包问题
粘包就是一次从socket缓冲区中读取到的数据不止一条消息。半包就是一次从socket缓冲区读取的数据只是一条消息的部分。

要解决粘包半包的问题，常用的方法有长度信息法、固定长度法和结束符号法。一般的游戏开发中使用的长度信息法。

长度信息法在要发送的信息前面，加上一个固定长度的信息长度信息，解析的时候，先读出这个固定长度的信息数据，然后读取信息长度的数据。可见，这个方法的核心是实现一个能从socket缓冲区读取指定长度数据的函数。

从socket缓冲区中读取指定长度的数据，可以参考《unix网络编程》中的readn函数。
```
ssize_t						/* Read "n" bytes from a descriptor. */
readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		} else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}
/* end readn */
```
##不完整发送问题

当socket的缓冲区满了的时候，再往缓冲区写数据，写入的数据可能小于要发送的数据。如果不对这种情况进行处理，那发送的数据就可能不完整。因此，我们需要一个向缓冲区写入固定长度数据的函数，同样，我们也可以参考《UNIX网络编程》中的写法。

```
ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr   += nwritten;
	}
	return(n);
}
/* end writen */
```

##大端小端问题

因为消息前面有一个表示长度的数字，而数字的表示有大端小端之分，要处理这个问题，只需要规定传输的数字是大端表示还是小端表示。然后，在传输的时候进行判断，如果和传输的表示方式不同，进行转换即可。在收到数字的时候，使用自定义的函数来解析这个数字。

##连接断开的问题

游戏开发中，客户端通常要和服务器保持TCP连接，这时候，就需要处理连接断开问题了。当发送消息的时候，需要检查连接是否正常。关闭的时候，要检查缓冲区的数据是否已经发送完毕，要等数据发送完毕才能关闭。

我对这方面不是很熟，先写这么多吧。
