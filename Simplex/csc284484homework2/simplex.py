#!/usr/bin/python3

import os
import sys

def simplex(A, b, B):
  pass

def feasibleSoln(A, b):
  pass

def printBasis(B):
  pass

# Problem count
k = int(sys.stdin.readline())

for i in range(k):
  # n = num vars
  # m = num constraints
  n, m = map(int, sys.stdin.readline().split(' '))
  A, b = [], []

  for j in range(m):
    row = list(map(int, sys.stdin.readline().split(' ')))
    b.append(row[-1])
    A.append(row[:-1])
    printBasis(feasibleSoln)
