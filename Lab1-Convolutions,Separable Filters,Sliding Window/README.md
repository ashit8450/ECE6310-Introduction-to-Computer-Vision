# Assignment 1 : Convolutions, Separable Filters, Sliding Window

### Problem Statement
In this assignment, three versions of a 7x7 mean filter were used to perform convolutions. The first version uses a simple 2D convolution. The second version uses two separable filters of size 1x7 and 7x1. The third version uses separable filters along with a sliding window, to speed up the computation time. 

#### Original Image
<p float="left">
  <img src="https://github.com/srivas18/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab1-Convolution-and-Timing/bridge.PNG" width="310" />
</p>

#### Output Images
From left to right: 2D Convolution, Separable Filters, Separable Filters + Sliding Windows
<p float="left">
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab1-Convolutions%2CSeparable%20Filters%2CSliding%20Window/images/smoothed.jpg" width="310" />
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab1-Convolutions%2CSeparable%20Filters%2CSliding%20Window/images/smoothed_sep.jpg" width="310" />
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab1-Convolutions%2CSeparable%20Filters%2CSliding%20Window/images/smoothed_sep_sw.jpg" width="310" />
</p>

#### Conclusion
The output from all the three variations of the mean filters are the same. However the convolution becomes faster.

#### Instruction
The following command needs to be executed first to create an executable,
`gcc -o <executable name> lab1_ashit.c`

The following command is then executed to run the program,
`./<executable name>`

Note: While running the program, clone the entire repository, in order to use the input images.

 

