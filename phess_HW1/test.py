#!/usr/bin/python3

import glob
import os
import re
import time
import sys

inputs = glob.glob('*.in')
inputs.sort()

os.system("rm -f my*")
runFlag = " -p" if len(sys.argv) > 1 and sys.argv[1] == "-p" else ""

for file in inputs:
  start = time.time()
  os.system('./a.out < ' + file + " > my" + re.sub('in', 'out', file) + runFlag)
  print("%s\t%s\t" % (file, round(time.time() - start, 3)), end = '')
  with open(re.sub('in', 'out', file), 'r') as f:
    with open("my" + re.sub('in', 'out', file), 'r') as f2:
      lines = f.readlines()
      lines2 = f2.readlines()
      match = True
      for (a,b) in zip(lines, lines2):
        if (a != b):
          match = false
          break
  print("Correct" if match else "Incorrect")
