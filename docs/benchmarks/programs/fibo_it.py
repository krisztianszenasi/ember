#!/usr/bin/env python3

import sys


def fib(n: int) -> int:
    a = 0
    b = 1
    i = 0

    while i < n:
        next_value = a + b
        a = b
        b = next_value
        i += 1

    return a


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