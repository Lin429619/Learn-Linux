##### socket系统调用
* `socket()`创建一个新 `socket`。
*  `bind()`将一个 `socket` 绑定到一个地址上。通常,服务器需要使用这个调用来将其 `socket` 绑定到一个众所周知的地址上使得客户端能够定位到该 `socket` 上。
* `listen()`允许一个流 `socket` 接受来自其他`socket` 的接入连接。
* `accept()`在一个监听流 `socket` 上接受来自一个对等应用程序的连接,并可选地返回对等 `socket` 的地址。
* `connect()`建立与另一个 `socket` 之间的连接。
  
> 监听`socket`上的客户端连接是通过`accept()`来接受的,它将返回一个与客户端的`socket`进行连接的新`socket`的文件描述符。

##### 值-结果参数

##### `getaddrinfo()`函数
> 给定一个主机名和服务器名,`getaddrinfo()`函数返回一个`socket`地址结构列表,每个结构都包含一个地址和端口号。
> 
```
int getaddrinfo(const char *host, const char *service, const struct addrinfo *hints, struct addrinfo **result);
// result参数返回一个结构列表而非单个结构
```
#####  释放`addrinfo`列表:`freeaddrinfo()`函数
> `getaddrinfo()`函数会动态地为`result`引用的所有结构分配内存,其结果是调用者必须要在不再需要这些结构时释放它们。使用`freeaddrinfo()`函数可以方便地在一个步骤中执行这个释放任务。如果要保留`addrinfo`结构或其关联的`socket`地址结构的一个副本，必须在调用`freeaddrinfo()`之前复制这些结构。