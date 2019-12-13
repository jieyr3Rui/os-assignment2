import matplotlib.pyplot as plt
import os
import re
import numpy as np
os.chdir(os.path.split(os.path.realpath(__file__))[0])

delta = 100

f = open("out-locality.txt","r")
flist = []
line = ' '
while line:
    
    line = f.readline()  
    line = line[:-1] 
    if line == '':
        break
    [_, physics, __] = re.findall(r"\d+",line)
    flist.append(int(int(physics) / 256))
num = len(flist)
f.close() 
frame = np.array(flist)
x = np.zeros_like(frame)
for ii in range(num):
    x[ii] = int(ii / delta)

plt.scatter(x, frame)
plt.show()
