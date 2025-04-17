基于 sylar 框架的 Webserver

开发环境:
- Ubuntu 22.04 LTS
- gcc 11.4.0
- cmake

项目路径:
- bin: 二进制输出
- build: 中间文件路径
- cmake: cmake 函数文件夹
- CMakeLists.txt: cmake 的定义文件
- lib: 库的输出路径
- Makefile
- sylar: 源代码路径
- tests: 测试代码路径

### 1. 日志模块
支持流式日志风格写日志和格式化风格写日志, 支持日志格式自定义, 日志级别, 多日志分离等功能

- 流式日志使用: `SYLAR_LOG_INFO(g_logger) << "this is a log";`
- 格式化日志使用: `SYLAR_LOG_FMT_INFO(g_logger, "%s", "this is a log");`

支持时间, 线程 id, 线程名称, 日志级别, 日志名称, 文件名, 行号等内容的自由配置

### 2. 配置模块
采用约定由于配置的思想, 定义即可使用, 不需要单独去解析 

支持变更通知功能, 使用 YAML 文件做为配置内容. 支持级别格式的数据类型, 支持 STL 容器 (vector, list, set, map 等), 支持自定义类型的支持 (需要实现序列化和反序列化方法)

使用方式如下:
```cpp
static sylar::ConfigVar<int>::ptr g_tcp_connect_timeout =
    sylar::Config::Lookup("tcp.connect.timeout", 5000, "tcp connect timeout");
```
定义了一个 TCP 连接超时参数，可以直接使用 g_tcp_connect_timeout->getValue() 获取参数的值, 当配置修改重新加载, 该值自动更新

上述配置格式如下:
```
tcp:
    connect:
            timeout: 10000
```

### 3. 线程模块
线程模块, 封装了 pthread 里面的一些常用功能: Thread, Semaphore, Mutex, RWMutex, Spinlock 等对象, 可以方便开发中对线程日常使用

为什么不适用 C++11 里面的 thread ?

本框架是使用 C++11 开发, 不使用 thread, 是因为 thread 其实也是基于 pthread 实现的. 并且 C++11 里面没有提供读写互斥量: RWMutex, Spinlock 等, 在高并发场景, 这些对象是经常需要用到的, 所以选择了自己封装 pthread

### 4. 协程模块
协程: 用户态的线程, 相当于线程中的线程, 更轻量级

后续配置 socket, hook, 可以把复杂的异步调用, 封装成同步操作, 降低业务逻辑的编写复杂度

目前该协程是基于 ucontext_t 来实现的, 后续将支持采用 boost.context 里面的 fcontext_t 的方式实现

### 5. 协程调度模块
协程调度器, 管理协程的调度, 内部实现为一个线程池, 支持协程在多线程中切换, 也可以指定协程在固定的线程中执行

是一个 N-M 的协程调度模型, N 个线程, M 个协程, 重复利用每一个线程.

### 6. IO 协程调度模块
继承与协程调度器, 封装了 epoll (Linux), 并支持定时器功能 (使用 epoll 实现定时器, 精度毫秒级), 支持 Socket 读写时间的添加, 删除, 取消功能, 支持一次性定时器, 循环定时器, 条件定时器等功能

### 7. Hook 模块
hook 系统底层和 socket 相关的 API, socket io 相关的 API, 以及 sleep 系列的 API

hook 的开启控制是线程粒度的, 可以自由选择. 通过 hook 模块, 可以使一些不具异步功能的API, 展现出异步的性能, 如 MySQL

### 8. Socket 模块
封装了 Socket 类，提供所有 socket API 功能, 统一封装了地址类, 将 IPv4, IPv6, Unix 地址统一起来. 并且提供域名, IP 解析功能

### 9. ByteArray 序列化模块
ByteArray 二进制序列化模块, 提供对二进制数据的常用操作

读写入基础类型 int8_t, int16_t, int32_t, int64_t 等, 支持 Varint, std::string 的读写支持, 支持字节序转化, 支持序列化到文件, 以及从文件反序列化等功能

### 10. TcpServer 模块
基于 Socket 类, 封装了一个通用的 TcpServer 的服务器类, 提供简单的 API, 使用便捷, 可以快速绑定一个或多个地址, 启动服务, 监听端口, accept 连接, 处理 socket 连接等功能. 具体业务功能更的服务器实现, 只需要继承该类就可以快速实现

### 11. Stream 模块
封装流式的统一接口, 将文件, socket 封装成统一的接口. 使用的时候, 采用统一的风格操作

基于统一的风格, 可以提供更灵活的扩展. 目前实现了 SocketStream

### 12. HTTP 模块
采用 Ragel (有限状态机, 性能媲美汇编), 实现了 HTTP/1.1 的简单协议实现和 uri 的解析

基于 SocketStream 实现了 HttpConnection (HTTP 的客户端) 和 HttpSession (HTTP 服务器端的链接)

基于 TcpServer 实现了 HttpServer. 提供了完整的 HTTP 的客户端 API 请求功能, HTTP 基础 API 服务器功能

### 13. Servlet 模块
仿照 Java 的 servlet, 实现了一套 Servlet 接口, 实现了 ServletDispatch, FunctionServlet, NotFoundServlet

支持 uri 的精准匹配, 模糊匹配等功能. 和 HTTP 模块, 一起提供 HTTP 服务器功能