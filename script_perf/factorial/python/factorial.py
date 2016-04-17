#!/usr/bin/python

import sys

def factorial(n):
    ret = 1
    for i in range(1, n+1):
        ret = ret * i

    return ret


if len(sys.argv) < 2 :
    print "Please specify number which need calculate its factorial"
    exit(1)

num = int(sys.argv[1])
loops = 1
if len(sys.argv) >= 3:
    loops = int(sys.argv[2])

for i in range(0, loops):
    ret = factorial(num)

