# LostRecord Language

LostRecord is a simple, custom-built programming language with a unique, narrative-driven syntax.

## About the Project

This repository contains the source code for the LostRecord compiler, which translates `.lr` files into assembly code and then compiles it into a native executable.

## How to Build

You can build the compiler in either `debug` or `release` mode.

*   **Release (default):**

    ```bash
    make
    ```

*   **Debug:**

    ```bash
    make debug
    ```

The compiled executable will be placed in the `bin/` directory.

## How to Run

To compile and run a LostRecord program, use the `run` target, specifying the source file with the `SOURCE` variable:

```bash
make run SOURCE=test.lr
```

This will:

1.  Compile your `.lr` file using the LostRecord compiler.
2.  Generate `output.s` (assembly code).
3.  Assemble it into `output.o` using `nasm`.
4.  Link it into an executable named `program` using `ld`.
5.  Run the final `program`.

## Example

Here is a simple example of a LostRecord program (`test.lr`):

```LostRecord
the story tells: "Hello, World!".
the story ends a line.

a value x, type int, begins at 10.
the story tells: x.
the story ends a line.
```
