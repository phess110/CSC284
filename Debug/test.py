import glob
import os
import re
import time

inputs = glob.glob('*.in')

for file in inputs:
  start = time.perf_counter()
  os.system('maxflow.exe < ' + file + " > my" + re.sub('in', 'out', file))
  print("--- %s seconds ---" % (time.perf_counter() - start))