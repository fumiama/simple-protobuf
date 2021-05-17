# simple-protobuf

[English](README.md) | 简体中文

Google ProtoBuf的简化变体。

# 数据格式
本程序生成的文件与C结构体一一映射，但该结构体不可包含指针，具体如下所示。

```bash
[struct_len] + n*[[type][data_len][data]]
```

1. `type` 指明该项的类型，亦即其在结构体中实际占用的空间的对数。
2. `*_len` 是一个小于 2^57^ 的变长数字。
3. `data` 是 `data_len` 长度的数据。

# 用法
比较简单，详见`test.c`。

## 保存结构体
### 1. 创建 items_type 数组

是结构体所有项的`type`组成的数组。

### 2. 类型对齐

接下来需要用`align_struct`进行对齐。如果您已经将数组对齐或结构体本身是对齐的，则无需此步骤。

### 3. 调用 set_pb 保存


## 读入结构体

### 1. 打开文件

### 2. 调用 get_pb 读取
