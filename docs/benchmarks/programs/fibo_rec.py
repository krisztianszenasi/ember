#!/usr/bin/env python3

import sys


def fib(n: int) -> int:
    if n < 2:
        return n

    return fib(n - 1) + fib(n - 2)


def main() -> None:
    text = sys.stdin.readline()

    try:
        n = int(text)
    except ValueError:
        print("Please write a number!")
        return

    print(f"Output: {fib(n)}")


if __name__ == "__main__":
    main()