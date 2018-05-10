_Authors_:
  Eduardo Berg
  Logan Lawrence
  
GENERAL NOTES:
==============

We first thought of doing this as our final project, but with permission for the instructor we ended up doing it as extra credit and chesstronic (https://eberg857.github.io/Chesstronic/, https://youtu.be/dcaVRzoHaNs) became our final project instead.

Our bike system's purpose is to display different colors depending on how fast the user is riding, and upon stopping, displays a unique animation. 
Check out our video of the result: https://youtu.be/eMoFD-dRQzU.

This project required two meter-long APA102-C RGB LED strips, an MSP430 microcontroller, and several battery units (as well as a working bicycle, of course).

Also make sure to check out the code we used to make this happen :)

DESIGN:
=======

One of the more challenging aspects of the 

Unfortunately, since this was a last minute extra credt project, we didn't order a PCB specifically designed for our system. However, we were able to repurpose our mood ring lab from halfway through the semester to suit our needs. 

CODE:
=====

All of the bike logic occurs in the infinite while loop of the main function. In this loop, there are essentially two different block of code, the logic that updates the LEDs based on the state of the system, and a block of code dedicated to interpretting the current value of the sensor and changing the state:

**_Updating LEDs_**: Another big problem with dealing with the LED array was finding a method that used the transmission buffer without storing a very large array of chars which would take up too much space on the MSP430. To combat this, instead of sending a position in an some private array attribute of the class, we had a bit of logic determining the char value of what would be sent to the transmission buffer. It does this by keeping an index variable indicating the pseudo-position in the previously existing array, and then sends logic to the transmission buffer depending on how far in the original array the transmission sequence should be. The code for this can be seen in the beginning of the infnite while loop in main, where there are various statements, ie. the catch for the first 4 chars sent, which need to be 0x00, that handle different places in the transmitted array.

Since we wanted the polling speed of the hall effect sensor to be very high to ensure that the sensor picked up each revolution of the wheel (where the actual speed of the magnet is moving around 2 m/s), the data that's pushed to the LED array is changed dynamically. By this, we mean that on every iteration of the 
