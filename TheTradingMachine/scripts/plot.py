import numpy as np
import matplotlib.pyplot as plt
import time
import csv

spamReader = csv.reader(open('AMD.csv', newline=''), delimiter=',', quotechar='|')

i = 0;
for row in spamReader:
	# currentline = row.split(",")
	plt.plot([i,i], [row[0],row[3]], 'ro-')
	i = i + 1;
	print (row[0], row[3])
	# print (row[3])
	
plt.show()

# x = np.arange(0, 10, 0.1)
# y = np.sin(x)

# plt.ion()
# ax = plt.gca()
# ax.set_autoscale_on(True)
# line, = ax.plot(x, y)

# for i in range(100):
    # line.set_ydata(y)
    # ax.relim()
    # # ax.autoscale_view(True,True,True)
    # plt.draw()
    # y=y*1.1
    # plt.pause(0.1)
	
# for i in range(10):
	# plt.plot([i,i], [i,i+1], 'ro-')
	# plt.pause(5)


plt.show()