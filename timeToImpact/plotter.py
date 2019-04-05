import numpy as np
import matplotlib.pyplot as plt

# with open("build/task1data.txt") as f:
with open("build/task2data.txt") as f:
    data = f.readlines()
    index = []
    tti = []
    for i in range(len(data)):
        temp = data[i].split("\t")
        index.append(temp[0])
        tti.append(temp[1])

tti = np.array(tti, dtype='float')
index = np.array(index, dtype='float')

tti = tti[1:]
index = index[1:]

# Do best linear fit
A = np.vstack((index, np.ones_like(index))).T
x = np.linalg.pinv(A) @ tti

temp = np.linspace(1, 25, 25)
B = np.vstack((temp, np.ones_like(temp))).T
y = B @ x

plt.figure(1)
plt.scatter(index, tti, color='r')
plt.plot(temp, y)
plt.xlabel('Frame Number')
plt.ylabel('Frames to Impact')
plt.show()
