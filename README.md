# simple-protobuf

English | [简体中文](README_ZH.md)

A simplified variant of Google's ProtoBuf.

# format
A simple-protobuf file can be filled into a C struct directly, but no pointer is supported.

The sp file format is shown below.

```bash
[struct_len] + n*[[type][data_len][data]]
```

1. `type` is a length-variable LE number(less than 2^57^) indicating the aligned type length of each struct item.
2. `*_len` is a length-variable LE number(less than 2^57^) indicating the length of `*`.
3. `data` contains `data_len` bytes of the data.

# usage
You can read `test.c` to find out the detailed usage.

## Save a struct
### 1. Create items type length info array

This array has type `uint64_t`, which indicates the aligned length of each struct item. You should call `align_struct` to get that array.

### 2. Call set_pb to save

## Read into a struct

### 1. Open file

### 2. Call get_pb to read