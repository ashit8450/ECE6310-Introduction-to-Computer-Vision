# Assignment 2 : Optical Character Recognition

### Problem Statement
In this assignment, the letter 'e' has to be detected in an image using the matched spatial filter. The accuracy of the convolution is verified from the ground truth locations provided. The subsequent True Positives, False Positives, True Negatives and False Negatives are used to compute the ROC curve for the entire range of threshold of the 8-bit image scale.

#### Original Image
<p float="left">
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab2-%20Optical%20Character%20Recognition/images/parenthood.jpg" width="310" />
</p>

#### Output Images
From left to right: MSF Image, Binary Image(white dots showing probable locations for a threshold of 195)
<p float="left">
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab2-%20Optical%20Character%20Recognition/images/parenthood_msf.jpg" width="310" />
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab2-%20Optical%20Character%20Recognition/images/parenthood_binary.jpg" width="310" />
</p>

#### Conclusion
The ROC curve plot shows the ideal relation between True Positive Rate and False Positive Rate.
<p float="left">
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab2-%20Optical%20Character%20Recognition/images/ROC%20Curve.jpg" width="310" />
</p>

#### Instruction
The following command needs to be executed first to create an executable,
`gcc -o <executable name> lab2_ashit.c`

The following command is then executed to run the program,
`./<executable name> <lower threshold value> <higher threshold value>`

Note: While running the program, clone the entire repository, in order to use the input images.

 

