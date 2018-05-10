_Authors_: Eduardo Berg, Logan Lawrence
  
GENERAL NOTES:
==============

We first thought of doing this as our final project, but with permission for the instructor we ended up doing it as extra credit and chesstronic (https://eberg857.github.io/Chesstronic/, https://youtu.be/dcaVRzoHaNs) became our final project instead.

Our bike system's purpose is to display different colors depending on how fast the user is riding, and upon stopping, displays a unique animation. 
Check out our video of the result: https://youtu.be/eMoFD-dRQzU.

This project required one Hall effect sensor - US5881LUA (https://www.adafruit.com/product/158), two meter-long APA102-C RGB LED strips, an MSP430 microcontroller, and several battery units (as well as a working bicycle, of course).

Also make sure to check out the code we used to make this happen :)

DESIGN:
=======

**_Basic Ideology_**: Our main goal was to use a 

**_PCB and MSP430 Power_**: Unfortunately, since this was a last minute extra credt project, we didn't order a PCB specifically designed for our system. However, we were able to repurpose our mood ring lab from halfway through the semester to suit our needs. Since the LED array is powered externally through AA batteries, there was only need to send the sensor's signal to the MSP430, and take that interpreted signal and relay it to the LEDs. For this reason, we had a seperate power source for the MSP430 and sensor, which were both on the makeshift PCB.

**_Configuring LED Chains_**:From there, the the data and clock signals were sent to the LED chain, which used a power source of their own. There were two sepearte LED strips used. Each which used a pack of AA battery holders, which had 2 AA batteries, in total using 4 AA batteries. From there we simply connected the data and clock signals from the first LED strips to the second LED strip.

CODE:
=====

All of the bike logic occurs in the infinite while loop of the main function. In this loop, there are essentially two different block of code, the logic that updates the LEDs based on the state of the system, and a block of code dedicated to interpretting the current value of the sensor and changing the state:

**_Updating LEDs_**: Another big problem with dealing with the LED array was finding a method that used the transmission buffer without storing a very large array of chars which would take up too much space on the MSP430. To combat this, instead of sending a position in an some private array attribute of the class, we had a bit of logic determining the char value of what would be sent to the transmission buffer. It does this by keeping an index variable indicating the pseudo-position in the previously existing array, and then sends logic to the transmission buffer depending on how far in the original array the transmission sequence should be. The code for this can be seen in the beginning of the infnite while loop in main, where there are various statements, ie. the catch for the first 4 chars sent, which need to be 0x00, that handle different places in the transmitted array.

**_Interpreting Sesnor Values_**: The bottom half of the infinite while loop deals solely with interpreting the values from the Hall effect sensor. Even though the sensor itself was reliable enough (meaning that it would consistently display the same values when in proximity to a magnet and had a reliable boundary for when it was not), there was one other small challenge that presented itself. This problem in essence was that our while loop ran so quickly that when the magnet would pass the sensor, the while loop would interpret multiple wheel revolutions since the while loop would iterate multiple times while the magnet was still in front of the sensor. To fix this problem, we simply added a "cooldown" counter whose purpose was to say that after a revolution had been read from the sensor, wait a certain number of iterations of the main while loop until you poll the sensor again. The value that we ended up using for our code was completely arbitrary, where a some of the contributing factors would be the clock speed you choose for the MSP, the diameter of the wheel that you are using, and some approximate values for how fast the user rides.

**_About the Logic_**: Since we wanted the polling speed of the hall effect sensor to be very high to ensure that the sensor picked up each revolution of the wheel (where the actual speed of the magnet is moving around 2 m/s), the data that's pushed to the LED array is changed dynamically. By this, we mean that on every iteration of the while loop, only one new char is trasmitted and then the sensor is polled again. Originally, we had an inner while loop dedicated to ensuring that all of the LEDs were updated before polling the sensor again, which resulted in the sensor being very very unreliable since it was once being polled a fraction of the times that it usually was. Now, each subsequent value in the LED chain is changed as the sensor is polled, creating an unavoidable effect of the new updated color speeding across the LED chain, as since the sensor could update the state halfway through the LED chain update, the new color will start from that halfway point in the LED chain and continue to update all of the subsequent LEDs and loop back around to its starting place.
