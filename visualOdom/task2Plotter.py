import numpy as np
import matplotlib.pyplot as plt

from IPython.core.debugger import Pdb

with open('HallwaySequenceEstimate.txt') as f:
    data = f.readlines()  #Change this part
    estimate = np.zeros((3,1))
    for i in range(len(data)):
        temp = data[i].split("\t")
        T = np.array([[float(temp[0]), float(temp[1]), float(temp[2]), float(temp[3])],
                      [float(temp[4]), float(temp[5]), float(temp[6]), float(temp[7])],
                      [float(temp[8]), float(temp[9]), float(temp[10]), float(temp[11])],
                      [0, 0, 0, 1]])
        # Pdb().set_trace()
        estimate= np.concatenate((estimate,T[0:3,-1].reshape((3,1))), axis=1)

plt.figure(1)
plt.plot(estimate[0,:], estimate[2,:], color='b', label='est')
plt.show()
