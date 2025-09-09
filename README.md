# Core Utils Reimplementation – `dog` & `elles`

This project is a personal learning exercise where I reimplemented two classic Unix tools in C:

- **`dog`** → a simplified version of [`cat`](https://man7.org/linux/man-pages/man1/cat.1.html), for printing file contents.  
- **`elles`** → a simplified version of [`ls`](https://man7.org/linux/man-pages/man1/ls.1.html), for listing directory contents.  

The names are a playful twist: every `cat` vs. `dog` and `elles` sounds like `l` and `s`. 🐾

---

## Skills Learned
- Low-level **C programming** with system calls  
- **File I/O**  
- **Directory handling**  
- **File metadata**  
- **Permission bit manipulation**  
- **Error handling** with `perror`  
- **Command-line flag parsing**  
- Hands-on exploration of **Linux internals**
---

## Features

### `dog`
- Reads from one or more files and writes to standard output.  
- Falls back to `stdin` if no files are given.  
- Buffer size dynamically adjusts (up to 4096 bytes).  
- Prints debug info: number of bytes read/written.  
- Basic error handling for missing/unreadable files.  

### `elles`
- Lists files and directories.  
- Supports:
  - `-a` → show hidden files  
  - `-l` → long listing format (permissions, UID, GID, timestamp, size)  
- Custom permission string formatter.  
- Human-readable timestamps.  
- Handles multiple paths and mixed flags.  
- Error handling for invalid directories.  

---

## Limitations
- These are **not full GNU coreutils replacements**.  
- `dog` includes extra debug output not present in `cat`.  
- `elles` formatting differs from real `ls`.  

The project focuses on **learning system calls, directory traversal, and Unix tool design**.

---

## Build & Run

### Requirements
- C compiler  
- CMake 

### Build
``` bash
mkdir build && cd build
cmake ..
make
````

### Usage

```bash
# dog (like cat)
./dog file1.txt file2.txt
./dog > ouput.txt

# elles (like ls)
./elles            # default listing
./elles -a         # show hidden
./elles -l         # long format
./elles -l -a /etc   # combine flags, custom path
```
---

## Example

```bash
$ echo "hello world" > hello.txt
$ ./dog hello.txt
hello world
read 12 and written 12

$ ./elles -l
hello.txt    rw-r--r-- 1000 1000 2028-02-29 18:00:00 12
```
