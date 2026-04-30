# Calculator – C

A simple command-line calculator written in C.

## Requirements

- GCC or Clang
- Make

## Build

```bash
make
```

## Usage

```bash
./calculator <number> <operator> <number>
```

**Supported operators:** `+`, `-`, `*`, `/`

**Examples:**

```bash
./calculator 10 + 5
# Output: 15

./calculator 9 / 3
# Output: 3

./calculator 2.5 '*' 4
# Output: 10
```

> Note: Wrap `*` in quotes in most shells to avoid glob expansion.

## Clean

```bash
make clean
```
