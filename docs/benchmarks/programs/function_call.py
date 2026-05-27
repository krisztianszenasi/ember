#!/usr/bin/env python3

import sys


def add_one(x: int) -> int:
    return x + 1


def function_call_bench(n: int) -> int:
    i = 0

    while i < n:
        i = add_one(i)

    return i


def main() -> None:
    text = sys.stdin.readline()

    try:
        n = int(text)
    except ValueError:
        print("Please write a number!")
        return

    print(f"Output: {function_call_bench(n)}")


if __name__ == "__main__":
    main()