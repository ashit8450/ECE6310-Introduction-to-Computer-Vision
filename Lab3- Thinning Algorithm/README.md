# Assignment 3 : Thinning Algorithm

### Problem Statement
In this project, I had to implement a Zhang-Suen thinning algorithm which would help in weeding out false positives in detecting the letter "e" in the image provided. This project builds up on the previous one to make the optical character recognition more robust. Please read `lab3.pdf` for more details into this project.

#### Original Image, MSF Image
<p float="left">
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab2-%20Optical%20Character%20Recognition/images/parenthood.jpg" width="310" />
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab2-%20Optical%20Character%20Recognition/images/parenthood_msf.jpg" width="310" />
</p>

#### Output Images
From left to right: Original Image of letter `e` as in input image, Binary Image of letter `e`, Thinned Image of letter `e`, Detection of letter `e`
<p float="left">
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab3-%20Thinning%20Algorithm/images/Original_e_letter.png" width="155" />
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab3-%20Thinning%20Algorithm/images/Binary_e_letter.png" width="155" />
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab3-%20Thinning%20Algorithm/images/Thinned_e_letter.png" width="155" />
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab3-%20Thinning%20Algorithm/images/e_letter_detection.png" width="310" />
</p>

#### Conclusion
The ROC curve plot shows the ideal relation between True Positive Rate and False Positive Rate.
<p float="left">
  <img src="https://github.com/ashit8450/ECE6310-Introduction-to-Computer-Vision/blob/master/Lab3-%20Thinning%20Algorithm/images/ROC%20Curve.png" width="610" />
</p>
We can see that in this ROC curve the False Positive Rate axis has been drastically reduced. This indicates that we have been successful in removing false positives from our OCR algorithm.

#### Instruction
The following command needs to be executed first to create an executable,
`gcc -o <executable name> lab3_ashit.c`

The following command is then executed to run the program,
`./<executable name> <lower threshold value> <higher threshold value>`

Note: While running the program, clone the entire repository, in order to use the input images.

 

