import csv
import os
dirname = os.path.dirname(__file__)
filename = os.path.join(dirname, 'oltest2.csv')
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal

if __name__ == "__main__":
    time= []
    pos = []

    with open(filename) as csvDataFile:
        csvReader = csv.reader(csvDataFile)
        for row in csvReader:
            time.append(row[0])
            pos.append(row[1])


timepos = np.column_stack((time[1:],pos[1:])).astype(np.float)
print(timepos[1:,:])
vel = np.gradient(timepos[:,1], 0.003, axis=0)
# plt.plot(timepos[:,0], timepos[:,1])

lpf = signal.butter(1, 0.1, btype='lowpass', analog=False, output='sos', fs=1)
filtered = signal.sosfilt(lpf, vel)
vel_pre_filtered = np.gradient(filtered, timepos[:,0], axis=0)
# plt.plot(vel_pre_filtered, color='green',  linewidth=1)#[:,0], vel[:,1])
plt.plot(timepos[:,0], vel, color='red', linewidth=1)
plt.plot(timepos[:,0], filtered, color = 'blue', linewidth=1)
# plt.plot(timepos[:,0], timepos[:,1], color='blue')
plt.show()