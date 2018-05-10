Authors:
  Eduardo Berg
  Logan Lawrence
  
==============
GENERAL NOTES:
==============

We first thought of doing this as our final project, but with permission for the instructor we ended up doing it as extra credit and chesstronic (https://eberg857.github.io/Chesstronic/, https://youtu.be/dcaVRzoHaNs) became our final project instead.

Our bike system's purpose is to display different colors depending on how fast the user is riding, and upon stopping, displays a unique animation. 
Check out our video of the result: https://youtu.be/eMoFD-dRQzU.

This project required two meter-long APA102-C RGB LED strips, an MSP430 microcontroller, and several battery units (as well as a working bicycle, of course).

Also make sure to check out the code we used to make this happen :)

=====
CODE:
=====

All of the bike logic occurs in the infinite while loop of the main function. In this loop, there are essentially 

Another big problem with dealing with the LED array was finding a method that used the transmition buffer without storing a very large array of chars which would take up too much space on the MSP430. 

Since we wanted the polling speed of the hall effect sensor to be very high to ensure that the sensor picked up each revolution of the wheel (where the actual speed of the magnet is moving around 2 m/s), the data that's pushed to the LED array is changed dynamically. By this, we mean that on every iteration of the 

  
