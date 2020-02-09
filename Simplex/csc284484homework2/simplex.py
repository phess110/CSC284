#!/usr/bin/python3

import os
import sys
import numpy as np
import numpy.linalg as la

"""
max c^Tx
Ax = b
x >= 0

Initial feasible basis: B
"""
def simplex(A, b, c, B):
  pass



"""
Assumptions:
A : m x n (rank(A) = m)
b : m x 1
x : n x 1
y : m x 1

The feasible basis will be given by the basis B
used by the simplex algorithm when it halt with value 0.
(With possible adjustment to remove the y-variables)

min c^Tx' (actually: max -c^Tx') subject to
  A'x' = b
  x' >= 0
where
  c = [0_{1 x n}|1_{1 x m}] : 1 x (m+n)
  x' = [x|y]                : 1 x (m+n)
  A' = [A|I_{n x n}]        : m x (m+n)
  Feasible basis: {n,..., n+m-1}
"""
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
