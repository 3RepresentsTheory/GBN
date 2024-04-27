# A Simple Go Back To N Network Stack

## 计网实验一，简易的Go Back To N网络协议栈

---

一个简易的基于UDP的Go Back To N 协议，可以实现基本的文件传输功能

---
### 代码规范
* 函数名全部以大写开头，静态方法全部小写
* 类名全部以大写开头
* 成员变量以及成员静态变量统一小写，并在结尾加上```_```
* 常量全部大写


---
### 协议实现规定
对于PDU帧：
* 采用定长帧，帧格式```GBNPDU.h```有详细介绍
* 定长帧中数据包部分长度统一为4KB
 
对于滑动窗口以及缓冲：
* 使用std::deque进行缓冲，没有缓冲区大小限制

对于套接字接口：
* 套接字接口中，默认阻塞读取并无超时，默认阻塞写入并且无限重传