import numpy as np
import os
os.chdir(os.path.split(os.path.realpath(__file__))[0])

f = open('addresses-locality.txt', 'w')
for ii in range(10000):
    f.write(str(np.random.randint(0, 65535)) + "\n")