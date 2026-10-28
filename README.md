# HyperMQ

一个使用 C++ 从零实现的轻量级消息队列学习项目，设计思路参考 Kafka 的追加日志、分区和 offset 模型。

项目面向游戏服务器等事件驱动场景：游戏逻辑服负责发布事件，战绩、奖励、排行榜、日志等下游服务可以独立消费。

HyperMQ 的核心实现使用 C++ 编写，后续计划提供稳定的 C ABI，并通过 Go 的 `cgo` 为 Go 程序提供调用接口。

## 项目目标

- 理解消息队列的核心机制；
- 实现适合游戏服务器场景的轻量级消息队列；
- 逐步支持 Topic、Partition、Consumer、Consumer Group 和 offset；
- 通过 C++ 获得对内存、并发和性能的更细粒度控制；
- 通过 C ABI 和 `cgo` 连接 Go 业务程序。

## 核心模型

```text
Broker
  └── Topic: player-events
        ├── Partition 0: [offset 0] [offset 1] ...
        ├── Partition 1: [offset 0] [offset 1] ...
        └── Partition 2: [offset 0] [offset 1] ...
```

同一个 Key 会稳定进入同一个 Partition，因此可以保证同一个玩家、房间或对局内的消息顺序。

```text
player-1001 → Partition 1 → offset 0, 1, 2 ...
player-1002 → Partition 0 → offset 0, 1, 2 ...
```

offset 只在单个 Partition 内有意义，不保证整个 Topic 的全局顺序。

## 计划中的 C++ API

核心 C++ API 将围绕以下对象设计：

```text
Broker
  └── Topic
        └── Partition
              └── Message
```

后续还会加入：

- Consumer：绑定一个或多个 Partition 读取消息；
- Consumer Group：在多个 Consumer 之间分配 Partition；
- Commit：记录每个 Group、每个 Partition 的消费进度；
- 持久化：支持进程重启后恢复消息和 offset；
- C ABI：为 Go、C 或其他语言提供稳定的调用边界。

## 计划中的 Go 调用方式

C++ 核心库不会直接暴露 C++ 类给 Go，而是提供 C ABI，例如：

```c
typedef struct hypermq_handle hypermq_handle;

hypermq_handle* hypermq_create_broker(void);
void hypermq_destroy_broker(hypermq_handle* handle);
```

Go 程序之后通过 `cgo` 调用这些 C 接口。

C ABI 层需要特别处理：

- 内存由哪一侧分配和释放；
- 字符串和字节数组的长度；
- 错误码与错误信息；
- 句柄生命周期；
- 线程安全边界；
- C++ 异常不能跨越 C ABI 边界。

## 计划中的目录结构

```text
include/hypermq/
  message.h
  partition.h
  topic.h
  broker.h
  consumer.h
  group.h

src/
  message.cpp
  partition.cpp
  topic.cpp
  broker.cpp
  consumer.cpp
  group.cpp

tests/
  message_test.cpp
  partition_test.cpp
  topic_test.cpp
  consumer_test.cpp

bindings/go/
  hypermq.go
```

## 构建与测试

项目将使用 CMake 管理构建：

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## 开发路线

1. C++ 内存版 Partition 追加日志；
2. Topic 与多 Partition 路由；
3. Consumer 与 offset；
4. Consumer Group 与 Partition 分配；
5. 并发安全与性能测试；
6. 文件持久化与崩溃恢复；
7. C ABI；
8. Go `cgo` 封装；
9. 游戏服务器场景下的重试、背压和延迟消息。

