# ASSIGNMENT 4: Interactive Region Growing
## Submitted by: Huzefa Shabbir Hussain Kagalwala

### Problem Statement:
In this project I implemented interactive region growing by developing a Windows program using the Win32 library. The program allows the user to load and display an image, and demonstrates several GUI and event handling techniques. The program was written using Microsoft Visual Studio 2019

Please read `lab4.pdf` for more details.

### Implementation:

Some of the salient features of the application are:
- Entering the values of the predicates the region growing is dependent on, to study its effects.
- The user can click on any location to visualize the region growing. 
- Multiple region grows can be accomplished at a single time.
- The user can select the color of the pixels being drawn while region growing.
- The user can reset the results of a previous region grow.
- Two modes of region growing:
	- Auto: The new pixels of a region are drawn every 1 millisecond.
	- Step: Pixels are drawn in a region every time the user presses the key 'J'.
  These modes can be switched as per the users liking during the region growing too.

**Working video of the application**

![](https://github.com/Huzefa-Kagalwala/ECE6310-Introduction-to-Computer-Vision/blob/master/4-Interactive%20Region%20Growing/Data/program.gif)

Please read the report `Huzefa_Kagalwala_Lab4` for further details of the project.

### Instructions:

The folder “HW04-Interactive Region Growing” contains the project files for the application. Open MS Visual Studio and load the file `HW4.sln` into it to open the project. 

Now build the solution and run it.