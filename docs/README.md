# Ember Documentation

This document describes the Ember CLI, the `.emx` executable format at a high
level, and the Ember assembly language from a user's perspective.

Examples here stay intentionally small. More complete programs can live in
[`examples/`](./examples/). Benchmark-specific documentation lives in
[`benchmarks/README.md`](./benchmarks/README.md).

## Table of contents

- [CLI](#cli)
- [Executable files](#executable-files)
- [Assembly language](#assembly-language)
- [Function calls and jumps](#function-calls-and-jumps)
- [Instruction reference](#instruction-reference)

## CLI

The main entry point is `ember-cli`.

```bash
ember-cli --version
```

### Assemble

Use `assemble` to turn an `.emb` source file into an `.emx` executable:

```bash
ember-cli assemble hello.emb
```

If you do not provide an output path, Ember writes `a.emx`.

To choose an explicit output file:

```bash
ember-cli assemble hello.emb -o hello.emx
```

### Disassemble

Use `disassemble` to inspect an `.emx` file in a human-readable form:

```bash
ember-cli disassemble hello.emx
```

Example output:

```text
Ember Executable
header:
    magic: EMX
    version: 1
    function_count: 2
    start_function_idx: 0

function[0] $entry:
    arity: 0
    local_count: 0
    constant_count: 0
    code_count: 4

    code:
        0000 call 1
        0003 halt
```

The disassembly shows the executable header, emitted functions, constant pools,
and bytecode instructions.

### Run

Use `run` to execute an `.emx` file with the Ember VM:

```bash
ember-cli run hello.emx
```

### Inspect instructions

Use `instruction` to list all available assembly instructions:

```bash
ember-cli instruction
```

You can also inspect a single instruction:

```bash
ember-cli instruction call
```

## Executable files

Ember executables use the `.emx` extension. They are binary files with an `EMX`
header followed by serialized functions, constants, and bytecode.

A minimal executable may look like this in hex:

```text
00000000: 45 4d 58 00 01 00 02 00 00 00 06 00 00 00 24 65  EMX...........$e
00000010: 6e 74 72 79 00 00 00 00 00 00 04 00 00 00 1e 01  ntry............
00000020: 00 20 04 00 00 00 6d 61 69 6e 00 00 00 00 01 00  . ....main......
00000030: 04 0e 00 00 00 48 65 6c 6c 6f 2c 20 57 6f 72 6c  .....Hello, Worl
00000040: 64 21 0a 04 00 00 00 00 00 21 1f                 d!.......!.
```

You usually do not need to work with the binary layout directly. `assemble`
creates `.emx` files, `run` executes them, and `disassemble` lets you inspect
their contents.

## Assembly language

An Ember source file is plain text and is usually stored with the `.emb`
extension.

### Program structure

An Ember program is organized into named sections:

- `.constants:` for compile-time values
- `.globals:` for runtime global storage
- `.code:` for function definitions

Only assignments belong in `.constants` and `.globals`. Only function
definitions belong in `.code`.

Minimal example:

```text
.constants:
  greeting = "Hello, World!\n"

.globals:
  counter = 0

.code:
  func main(arity=0, locals=0):
    ldc greeting
    print
    ret
```

### Constants

Constants are symbolic names for literal values. When you reference a constant,
the assembler resolves it to the underlying value.

Example:

```text
.constants:
  answer = 42
```

Used in code:

```text
ldc answer
```

### Globals

Globals are named runtime storage slots. They are initialized before `main`
runs, then accessed with `getg` and `setg`.

Example:

```text
.globals:
  counter = 0
```

Used in code:

```text
getg counter
ldc 1
iadd
setg counter
```

### Code

Functions are declared in the `.code` section:

```text
func main(arity=0, locals=0):
  ldc "Hello\n"
  print
  ret
```

Function headers declare:

- the function name
- `arity`, the number of arguments the function expects
- `locals`, the number of local slots reserved for the function

`main` is required. Ember generates an internal entry function that performs any
global initialization, calls `main`, and then halts the VM.

### Values and references

Instruction operands can be:

- literal values such as `1`, `3.14`, `true`, or `"text"`
- constant names
- global names
- function names
- label names

Examples:

```text
ldc 123
ldc "hello"
ldc true
getg counter
call print_number
jmp done
```

## Function calls and jumps

### Function calls

Use `call` with a function name:

```text
.code:
  func greet(arity=0, locals=0):
    ldc "hi\n"
    print
    ret

  func main(arity=0, locals=0):
    call greet
    ret
```

Use `geta` to read arguments inside a function:

```text
func echo(arity=1, locals=0):
  geta 0
  print
  ret
```

### Jumps and labels

Labels are local to a function and are declared with `@name:`. Jump
instructions refer to the label name without the `@`.

Unconditional jump:

```text
func main(arity=0, locals=0):
  jmp done
  ldc "skipped\n"
  print
@done:
  ret
```

Conditional jump:

```text
func main(arity=0, locals=0):
  ldc false
  jmp_if_false else_branch
  ldc "then\n"
  print
  ret
@else_branch:
  ldc "else\n"
  print
  ret
```

`jmp_if_false` consumes the top stack value and jumps when that value is false.

## Recoverable status codes

Some instructions can fail without stopping the VM. In those cases the process
status is updated instead of raising a runtime error, and the program can
inspect that status with `status_ok`, `status_code`, and `status_clear`.

This is separate from fatal VM errors such as invalid bytecode, invalid local
indices, or invalid jumps. Fatal errors stop execution. Status codes do not.

The built-in status constants are:

| Status | Meaning |
| --- | --- |
| `STATUS_OK` | The last recoverable operation succeeded. |
| `STATUS_INVALID_CAST` | A conversion or parse operation could not produce the requested value. |
| `STATUS_DIVISION_BY_ZERO` | `idiv`, `irem`, or `fdiv` attempted to divide by zero. |
| `STATUS_INVALID_TYPE` | An instruction received a value of the wrong type. |
| `STATUS_IO_ERROR` | `input` failed to read from standard input. |

These codes are set in the VM loop in a few predictable places:

- arithmetic instructions set `STATUS_INVALID_TYPE` when operands have the
  wrong type
- `idiv`, `irem`, and `fdiv` set `STATUS_DIVISION_BY_ZERO` when the divisor is
  zero
- `concat`, cast instructions, and parse instructions set
  `STATUS_INVALID_TYPE` when their input value has the wrong kind
- `parse_int`, `parse_float`, and `parse_bool` set `STATUS_INVALID_CAST` when
  the input is a string but cannot be parsed
- `input` sets `STATUS_IO_ERROR` when reading from `stdin` fails
- those same recoverable instructions set `STATUS_OK` on success

When a recoverable instruction fails it still pushes a value so the stack stays
consistent. In the current VM implementation that fallback value is `none`.

## Instruction reference

The table below lists the instructions currently recognized by the assembler.

| Instruction | Operands | Description |
| --- | --- | --- |
| `ldc` | `<value:any>` | Loads a value onto the stack. |
| `dup` | `<value:int>` | Duplicates stack values. |
| `pop` | `<value:int>` | Removes values from the stack. |
| `geta` | `<value:int>` | Loads a function argument onto the stack. |
| `getl` | `<value:int>` | Loads a local variable onto the stack. |
| `setl` | `<value:int>` | Stores the top stack value into a local variable. |
| `getg` | `<global>` | Loads a global variable onto the stack. |
| `setg` | `<global>` | Stores the top stack value into a global variable. |
| `iadd` | none | Adds two integer values. |
| `isub` | none | Subtracts two integer values. |
| `imul` | none | Multiplies two integer values. |
| `idiv` | none | Divides two integer values. |
| `irem` | none | Computes the integer remainder. |
| `fadd` | none | Adds two floating-point values. |
| `fsub` | none | Subtracts two floating-point values. |
| `fmul` | none | Multiplies two floating-point values. |
| `fdiv` | none | Divides two floating-point values. |
| `concat` | none | Concats two string objects. |
| `and` | none | Computes logical AND. |
| `or` | none | Computes logical OR. |
| `not` | none | Computes logical NOT. |
| `eq` | none | Compares two values for equality. |
| `neq` | none | Compares two values for inequality. |
| `gt` | none | Checks whether one value is greater than another. |
| `gte` | none | Checks whether one value is greater than or equal to another. |
| `lt` | none | Checks whether one value is less than another. |
| `lte` | none | Checks whether one value is less than or equal to another. |
| `jmp` | `<label>` | Jumps to a label. |
| `jmp_if_false` | `<label>` | Jumps to a label if the top stack value is false. |
| `call` | `<function>` | Calls a function. |
| `ret` | none | Returns from the current function. |
| `halt` | none | Stops the VM. |
| `print` | none | Prints the top stack value. |
| `input` | none | Reads an input value and pushes it onto the stack. |
| `status_ok` | none | Pushes boolean whether most recent instruction was successful or not. |
| `status_code` | none | Pushes the most recent status code. |
| `status_clear` | none | Clears the status code. |
| `i2f` | none | Casts an int value to a float value. |
| `f2i` | none | Casts a float value to an int value. |
| `i2b` | none | Casts an int value to a boolean value. |
| `f2b` | none | Casts a float value to a boolean value. |
| `s2b` | none | Casts a string value to a boolean value. |
| `n2b` | none | Casts a none value to a boolean value. |
| `i2s` | none | Casts an int value to a string value. |
| `f2s` | none | Casts a float value to a string value. |
| `b2s` | none | Casts a boolean value to a string value. |
| `n2s` | none | Casts a none value to a string value. |
| `parse_bool` | none | Parses a string value as a boolean value. |
| `parse_float` | none | Parses a string value as a float value. |
| `parse_int` | none | Parses a string value as an int value. |

For the live instruction list from your build, run:

```bash
ember-cli instruction
```

## Small examples

### Retry `input` until `parse_int` succeeds

This loop keeps asking for input until the entered string can be parsed as an
integer. `parse_int` pushes `none` and sets `STATUS_INVALID_CAST` when parsing
fails, so the failed value is popped before retrying.

```text
.constants:
  prompt = "Enter an integer: "
  invalid = "Not a valid integer, try again.\n"

.code:
  func main(arity=0, locals=0):
@read_again:
    ldc prompt
    print
    input
    parse_int
    status_ok
    jmp_if_false invalid_input
    print
    ret

@invalid_input:
    pop 1
    ldc invalid
    print
    jmp read_again
```

### Branch on a specific status code

This example handles division by zero explicitly. `idiv` does not stop the VM;
it pushes `none` and sets `STATUS_DIVISION_BY_ZERO` instead.

```text
.constants:
  divide_failed = "Cannot divide by zero.\n"

.code:
  func main(arity=0, locals=0):
    ldc 10
    ldc 0
    idiv
    status_code
    ldc STATUS_DIVISION_BY_ZERO
    eq
    jmp_if_false ok
    pop 1
    ldc divide_failed
    print
    ret

@ok:
    print
    ret
```
