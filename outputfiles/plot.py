import numpy as np
import matplotlib.pyplot as plt
import time
import csv

timeIndex = 0
openIndex = 1
highIndex = 2
lowIndex = 3
closeIndex = 4
volumeIndex = 5

filename = input("Enter filename: ");
priceReader = csv.reader(open(filename, newline=''), delimiter=',', quotechar='|')
# ##this plots a line graph connecting candles' closing prices
# i = 0
graph = plt.subplot(3,1,1)

# # ##candle stick plot
spamReader = csv.reader(open(filename, newline=''), delimiter=',', quotechar='|')
i = 0
for row in spamReader:
	plt.plot([i,i], [float(row[openIndex]),float(row[highIndex])], 'b-')
	plt.plot([i,i], [float(row[lowIndex]),float(row[closeIndex])], 'b-')
	if(float(row[openIndex]) < float(row[closeIndex])):
		plt.plot([i,i], [float(row[openIndex]),float(row[closeIndex])], 'g-')
	elif(float(row[openIndex]) > float(row[closeIndex])):
		plt.plot([i,i], [float(row[openIndex]),float(row[closeIndex])], 'r-')
	else:
		plt.plot(i, float(row[highIndex]), 'b')
	i = i + 1;
	
plt.subplot(3,1,2, sharex=graph)	

#scatter plot of ohlc
spamReader = csv.reader(open(filename, newline=''), delimiter=',', quotechar='|')
i = 0
for row in spamReader:
	plt.plot([i], [float(row[openIndex])], 'bo')
	plt.plot([i], [float(row[highIndex])], 'bo')
	plt.plot([i], [float(row[lowIndex])], 'bo')
	plt.plot([i], [float(row[closeIndex])], 'bo')
	i = i + 1;


plt.subplot(3,1,3, sharex=graph)	

#resistance support of line graphs
# priceReader = csv.reader(open(filename, newline=''), delimiter=',', quotechar='|')
# prevClosePrice = 0;
# i = 0;
# for row in priceReader:

	# if(float(row[openIndex]) < float(row[closeIndex])):
		# plt.plot([i,i], [float(row[highIndex]),float(row[lowIndex])], 'g-')
	# elif(float(row[openIndex]) > float(row[closeIndex])):
		# plt.plot([i,i], [float(row[highIndex]),float(row[lowIndex])], 'r-')
		
	# if(i > 0):
		# plt.plot([i - 1,i], [prevClosePrice,float(row[closeIndex])], 'r-')
	# prevClosePrice = float(row[closeIndex]);
	
	# if(row[5] == "support"):
		# plt.plot(i, float(row[closeIndex]), 'go');
	# elif(row[5] == "resistance"):
		# plt.plot(i, float(row[closeIndex]), 'ro');
	
	# i = i + 1;


# # ##volume plot
# priceReader = csv.reader(open(filename, newline=''), delimiter=',', quotechar='|')
# i = 0
# for row in priceReader:
	# if(float(row[openIndex]) < float(row[closeIndex])):
		# plt.plot([i,i], [0,int(row[4])], 'go-')
	# elif(float(row[openIndex]) > float(row[closeIndex])):
		# plt.plot([i,i], [0,int(row[4])], 'ro-')
	# else:
		# plt.plot([i,i], [0,int(row[4])], 'bo-')
	# i = i + 1;
	
plt.show()