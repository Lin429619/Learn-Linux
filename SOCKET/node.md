##### socket系统调用
* `socket()`创建一个新 `socket`。
*  `bind()`将一个 `socket` 绑定到一个地址上。通常,服务器需要使用这个调用来将其 `socket` 绑定到一个众所周知的地址上使得客户端能够定位到该 `socket` 上。
* `listen()`允许一个流 `socket` 接受来自其他`socket` 的接入连接。
* `accept()`在一个监听流 `socket` 上接受来自一个对等应用程序的连接,并可选地返回对等 `socket` 的地址。
* `connect()`建立与另一个 `socket` 之间的连接。
  
> 监听`socket`上的客户端连接是通过`accept()`来接受的,它将返回一个与客户端的`socket`进行连接的新`socket`的文件描述符。