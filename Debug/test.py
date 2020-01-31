import glob
import os
import re
import time

inputs = glob.glob('*.in')

for file in inputs:
  start = time.perf_counter()
  os.system('maxflow.exe < ' + file + " > my" + re.sub('in', 'out', file))
  print("%s : %s seconds ---" % (file, (time.perf_counter() - start)))
  with open(re.sub('in', 'out', file), 'r') as f:
    with open("my" + re.sub('in', 'out', file), 'r') as f2:
      lines = f.readlines()
      lines2 = f2.readlines()
      match = True
      for (a,b) in zip(lines, lines2):
        if (a != b):
          match = false
          break
  print(match)