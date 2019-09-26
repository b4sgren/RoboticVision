import numpy as np
import matplotlib.pyplot as plt

path = []
with open("ball_traj.txt") as f:
    data = f.readlines()
    for i in range(len(data)):
        temp = data[i].split("\t")
        z = [float(temp[0]), float(temp[1]), float(temp[2])]
        path.append(z)

traj = np.array(path)

fin = 30  # traj.shape[0]
x = traj[0:fin,0]
y = traj[0:fin,1]
z = traj[0:fin,2]

Az = np.column_stack((z * z, z))
Az = np.column_stack((Az, z**0))

# Tested just using the first half of the data
# Az = Az[0:20,:]
# y = y[0:20]
# x = x[0:20]

y_coeffs = np.linalg.pinv(Az) @ y
x_coeffs = np.linalg.pinv(Az) @ x

z_pred = np.linspace(0, z[0], 5000)
A = np.column_stack((z_pred * z_pred, z_pred))
A = np.column_stack((A, z_pred**0))
x_pred = A @ x_coeffs
y_pred = A @ y_coeffs

print(x_pred.item(0))
print(-y_pred.item(0))

plt.figure(1)
plt.plot(traj[:,0], traj[:,2],'b')
plt.plot(x_pred, z_pred, 'r--')
plt.axis([-20, 20, 0, 450])
plt.xlabel('x (in)')
plt.ylabel('z (in)')
plt.title('Top View')
plt.legend(["Camera", "Predicted"])

plt.figure(2)
plt.plot(traj[:,2], -traj[:,1])
plt.plot(z_pred, -y_pred, 'r--')
plt.axis([0, 450, -20, 70])
plt.xlabel('z (in)')
plt.ylabel('y (in)')
plt.title('Side View')
plt.legend(["Camera", "Predicted"])

plt.show()
