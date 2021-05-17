# simple-protobuf

English | [简体中文](README_ZH.md)

A simplified variant of Google's ProtoBuf.

# format
A simple-protobuf file can be filled into a C struct directly, but no pointer is supported.

The sp file format is shown below.

```bash
[struct_len] + n*[[type][data_len][data]]
```

1. `type` is an one-byte number indicating the type length(2^type^bytes) of this struct item.
2. `*_len` is a length-variable LE number(less than 2^57^) indicating the length of `*`.
3. `data` contains `data_len` bytes of the data.

# usage
You can read `test.c` to find out the detailed usage.

## Save a struct
### 1. Create items type info array

This array has type `uint8_t`, which indicates the length of each struct item. Note that the length of each item must be 2^n^.

### 2. Align items type info array

You can use function `align_struct` to align each item to the proper size.

If you are sure that your struct has been aligned, you won't need to run `align_struct`.

### 3. Call set_pb to save


## Read into a struct

### 1. Open file

### 2. Call get_pb to read