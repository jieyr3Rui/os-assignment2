import numpy as np
f = open('addresses-locality.txt', 'w')
for ii in range(1000):
    f.write(str(np.random.randint(0, 65535)) + "\n")