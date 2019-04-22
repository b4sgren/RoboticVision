import numpy as np
import matplotlib.pyplot as plt

from IPython.core.debugger import Pdb

with open('PracticeSequenceEstimate.txt') as f:
    data = f.readlines()  #Change this part
    estimate = np.zeros((3,1))
    pose = np.array([[0, 0, 0, 1]]).T
    for i in range(len(data)):
        temp = data[i].split("\t")
        T = np.array([[float(temp[0]), float(temp[1]), float(temp[2]), float(temp[3])],
                      [float(temp[4]), float(temp[5]), float(temp[6]), float(temp[7])],
                      [float(temp[8]), float(temp[9]), float(temp[10]), float(temp[11])],
                      [0, 0, 0, 1]])
        # Pdb().set_trace()
        pose = T @ pose
        estimate= np.concatenate((estimate,pose[0:3]), axis=1)

with open("PracticeImgs/PracticeSequenceTruth.txt") as f:
    data = f.readlines()
    truth = np.zeros((3,1))
    pose = np.array([[0, 0, 0, 1]]).T
    for i in range(len(data)-1):
        temp = data[i].split(" ")
        l = temp[-1].split("\n")
        T = np.array([[float(temp[0]), float(temp[1]), float(temp[2]), float(temp[3])],
                      [float(temp[4]), float(temp[5]), float(temp[6]), float(temp[7])],
                      [float(temp[8]), float(temp[9]), float(temp[10]), float(l[0])],
                      [0, 0, 0, 1]])
        pose = np.array([[float(temp[3]), float(temp[7]), float(l[0])]]).T # East, Down, North coordinate frame
        truth= np.concatenate((truth,pose[0:3]), axis=1)

plt.figure(1)
plt.plot(truth[0,:], truth[2,:], color='b', label='truth')
plt.axis([-200, 200, 0, 550])

plt.figure(2)
plt.plot(estimate[0,:], estimate[2,:], color='r', label='est')
# plt.axis([-200, 200, 0, 550])

plt.show()
