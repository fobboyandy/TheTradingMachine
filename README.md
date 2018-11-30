# TheTradingMachine

The Trading Machine is an application which enables retail algorithmic traders to accelerate their process in developing, testing, and live trading their algorithms. Currently, The Trading Machine only supports Interactive Broker on Windows.

# Dynamic Algorithm Loading

One main feature of The Trading Machine is the ability to load user defined algorithms at runtime like a plugin. Users can focus on development of their algorithms while The Trading Machine handles connection to Interactive Broker and delivering tick by tick data to their algorithm.

# Accelerated Strategy To Algorithm Deployment

Almost anyone with some programming skills can study a chart, propose a pattern, and develop a console based application to backtest their strategy. More likely than not, the strategy fails. We can debug our strategy and tweak its performance so by adding performance tracking lines in our code to visualize the profit. However, it's very likely that our strategy was not profitable to begin with and we have to come up with a new strategy. The Trading Machine aims to cut down on the time spent in repetitive parts of transforming a strategy into an algorithm. The repetitve parts for every new algorithmic strategy are creating algorithm specific code for visualizing the strategy's actions, tracking its performance, and setting up the environment to run (paper or live trade) the algorithm. The Trading Machine provides an abstract class BaseAlgorithm which does all of that. To visualize the strategy, BaseAlgorithm plots the movement of the chart along with the action of the algorithm. To track its performance, BaseAlgorithm marks the positions of entry and exit on the chart and displays the profit of each position and action taken by the algorithm. Lastly, BaseAlgorithm can trade your algorithm by calling your tickHandler whenever it gets data. Data can come realtime from the market or from a file. Your algorithm is abstracted away from the details where the data comes from. TLDR, the user only has to define the tickHandler function to create an algorithm. The tickHandler will be called whenever new data comes in. All basic actions taken tickHandler can be visualized on a GUI chart in real time. 

# Requirements

Windows Operating System with Microsoft Visual Studio

Interactive Broker (with streaming data subscription for real time data)

# Instructions

To Be Continued...

# Screenshots
![alt text](https://raw.githubusercontent.com/fobboyandy/TheTradingMachine/master/Screenshots/position_annotations.PNG)

