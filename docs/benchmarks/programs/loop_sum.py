#!/usr/bin/env python3

import sys


def loop_sum(n: int) -> int:
    total = 0
    i = 0

    while i < n:
        total += i
        i += 1

    return total


def main() -> None:
    text = sys.stdin.readline()

    try:
        n = int(text)
    except ValueError:
        print("Please write a number!")
        return

    print(f"Output: {loop_sum(n)}")


if __name__ == "__main__":
    main()