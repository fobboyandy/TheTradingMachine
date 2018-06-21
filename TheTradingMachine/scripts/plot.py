import numpy as np
import matplotlib.pyplot as plt
import time
import csv


filename = input("Enter filename: ");
priceReader = csv.reader(open(filename, newline=''), delimiter=',', quotechar='|')
# ##this plots a line graph connecting candles' closing prices
# i = 0
graph = plt.subplot(3,1,1)

# # ##candle stick plot
spamReader = csv.reader(open(filename, newline=''), delimiter=',', quotechar='|')
i = 0
for row in spamReader:
	plt.plot([i,i], [float(row[0]),float(row[1])], 'b-')
	plt.plot([i,i], [float(row[2]),float(row[3])], 'b-')
	if(float(row[0]) < float(row[3])):
		plt.plot([i,i], [float(row[0]),float(row[3])], 'go-')
	elif(float(row[0]) > float(row[3])):
		plt.plot([i,i], [float(row[0]),float(row[3])], 'ro-')
	else:
		plt.plot(i, float(row[1]), 'bo')
	i = i + 1;
	

plt.subplot(3,1,2, sharex=graph)	

#scatter plot of ohlc
spamReader = csv.reader(open(filename, newline=''), delimiter=',', quotechar='|')
i = 0
for row in spamReader:
	plt.plot([i], [float(row[0])], 'bo')
	plt.plot([i], [float(row[1])], 'bo')
	plt.plot([i], [float(row[2])], 'bo')
	plt.plot([i], [float(row[3])], 'bo')
	i = i + 1;


plt.subplot(3,1,3, sharex=graph)	

#resistance support of line graphs
priceReader = csv.reader(open(filename, newline=''), delimiter=',', quotechar='|')
prevClosePrice = 0;
i = 0;
for row in priceReader:

	if(float(row[0]) < float(row[3])):
		plt.plot([i,i], [float(row[1]),float(row[2])], 'g-')
	elif(float(row[0]) > float(row[3])):
		plt.plot([i,i], [float(row[1]),float(row[2])], 'r-')
		
	if(i > 0):
		plt.plot([i - 1,i], [prevClosePrice,float(row[3])], 'r-')
	prevClosePrice = float(row[3]);
	
	if(row[5] == "support"):
		plt.plot(i, float(row[3]), 'go');
	elif(row[5] == "resistance"):
		plt.plot(i, float(row[3]), 'ro');
	
	i = i + 1;


# # ##volume plot
# priceReader = csv.reader(open(filename, newline=''), delimiter=',', quotechar='|')
# i = 0
# for row in priceReader:
	# if(float(row[0]) < float(row[3])):
		# plt.plot([i,i], [0,int(row[4])], 'go-')
	# elif(float(row[0]) > float(row[3])):
		# plt.plot([i,i], [0,int(row[4])], 'ro-')
	# else:
		# plt.plot([i,i], [0,int(row[4])], 'bo-')
	# i = i + 1;
	
plt.show()

