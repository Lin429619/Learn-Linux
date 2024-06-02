##### `socket`系统调用
* `socket()`创建一个新 `socket`。
*  `bind()`将一个 `socket` 绑定到一个地址上。通常,服务器需要使用这个调用来将其 `socket` 绑定到一个众所周知的地址上使得客户端能够定位到该 `socket` 上。
* `listen()`允许一个流 `socket` 接受来自其他`socket` 的接入连接。
* `accept()`在一个监听流 `socket` 上接受来自一个对等应用程序的连接,并可选地返回对等 `socket` 的地址。
* `connect()`建立与另一个 `socket` 之间的连接。
  
> 监听`socket`上的客户端连接是通过`accept()`来接受的,它将返回一个与客户端的`socket`进行连接的新`socket`的文件描述符。

##### 值-结果参数

##### `getaddrinfo()`函数
> 给定一个主机名和服务器名,`getaddrinfo()`函数返回一个`socket`地址结构列表,每个结构都包含一个地址和端口号。

> `getnameinfo()`函数是`getaddrinfo()`的逆函数。给定一个socket地址结构,会返回一个包含对应的主机和服务名的字符串或者在无法解析名字时返回一个等价的数值。

```
int getaddrinfo(const char *host, const char *service, const struct addrinfo *hints, struct addrinfo **result);
// result参数返回一个结构列表而非单个结构
```
#####  释放`addrinfo`列表:`freeaddrinfo()`函数
> `getaddrinfo()`函数会动态地为`result`引用的所有结构分配内存,其结果是调用者必须要在不再需要这些结构时释放它们。使用`freeaddrinfo()`函数可以方便地在一个步骤中执行这个释放任务。如果要保留`addrinfo`结构或其关联的`socket`地址结构的一个副本，必须在调用`freeaddrinfo()`之前复制这些结构。

##### `shutdown()`系统调用
> * int shutdown(int sockfd, int how)
> 在套接字上调用`close()`会将双向通信通道的两端都关闭,系统调用`shutdown()`可以根据参数`how`的值选择关闭套接字通道的一端还是两端。
> `SHUT_RD`:关闭连接的读端。
> `SHUT_WR`:关闭连接的写端。在`shutdown()`中最常用到的操作就是`SHUT_WR`,有时候也被称为半关闭套接字。
> SHUT_RDWR:将连接的读端和写端都关闭.
> * 除了参数 `how`的语义之外,`shutdown()`同`close()`之间的另一个重要区别是:无论该套接字上是否还关联有其他的文件描述符,`shutdown()`都会关闭套接字通道。换句话说,`shutdown()`是根据打开的文件描述`(open file description)`来执行操作,而同文件描述符无关。
> * `shutdown()`并不会关闭文件描述符,就算参数`how`指定为`SHUT_RDWR`时也是如此。要关闭文件描述符,我们必须另外调用`close()`。

##### `sendfile()`系统调用
> * ==零拷贝传输==：应用程序调用`sendfile()`时,文件内容会直接传送到套接字上,而不会经过用户空间。
> 可以使用`sendfile()`将数据从文件传递到套接字上,但==反过来就不行==。另外,也不能通过`sendfile()`在两个套接字之间直接传送数据。




##### `setsockopt()`和 `getsockopt()`（分别用于设定和获取套接字选项）
```
int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
```
> * `option_name`: 需要设置的选项名,常用选项有以下：
> SO_REUSEADDR：允许重用本地地址和端口
> SO_RCVBUF：接收缓冲区大小
> SO_SNDBUF：发送缓冲区大小
> 
> ==SO_REUSEADDR套接字==选项最常见的用途:避免当TCP服务器重启时,尝试将套接字绑定到当前已经同 TCP 结点相关联的端口上时出现的EADDRINUSE(地址已使用)错误。
