# Volvo-CAN-Gauge
Reverse engineering the Volvo VIDA protocol to gather diagnostic information not available via OBD2 on Volvo cars.

**Old video of the project with the OLED screen before the Due port is here:** https://www.youtube.com/watch?v=hdAKEG6ggRk
  
![](./IMG_20210218_131622663.jpg)

# Hardware:
- 2011 Volvo C30 T5
- 4D Systems ULCD-220RD round LCD Display (https://4dsystems.com.au/ulcd-220rd)
- Arduino Due
- Copperhill Technologies Dual CAN Interface For Arduino Due With Extended Power Range (https://copperhilltech.com/dual-can-bus-interface-for-arduino-due-with-extended-power-range/)
- Custom designed 3d-printed LCD mount designed to fit the shape of the bottom left corner of the dashboard bezel. (STL is in the Display Mount folder)

# Libraries:
- geneArduino: Used to send data to the 4D Systems display.
- due_can: Used to easily access the built in CAN interfaces on the Arduino Due (https://github.com/collin80/due_can)

# Basic Functional Description:

The Volvo VIDA protocol is a basic message/response protocol not very different from ISO 15765-4. However, unlike ISO 15765-4, VIDA can also write controller firmware, activate actuators and run diagnostic tests.

In this project, we are only concerned with recieving data that would be otherwise unavailable with ISO 15765-4. Boost pressure, for example, is not available via ISO 15765-4 on Volvo cars. (a partial list of discovered codes from the VIDA database files is in Codes.txt)

We also use data from some broadcasted CAN frames that are used elsewhere in the car (for dashboard brightness, ignition status and headlights). Ideally, we would capture ALL of our information from broadcast frames as that involves a lot less overhead and traffic on the bus. However, some of the information that we need never gets broadcast, so we sometimes have no choice.

The code uses a psudo multi-tasking approach where the message recieve loop is always running if one of the other loops isn't currently running. This allows us to update/check broadcast frames in the background for brightness changes, ignition status changes and button presses and update the global variables accordingly.

In the display loop, we can show boost pressure, coolant temperature and intake temperature.

We also react to the dashboard brightness broadcast frame so the display updates it's brightness along with the rest of the dashboard. If the headlights are on, display brightness is multiplied by 0.8.

For right now, the only button we track is the cruise control cancel button. Holding the button for more than 2 seconds changes the currently displayed page. Perhaps in the future, a menu system could be implemented.

In order for the gauge to feel more "analog" the needle and value of the gauge will not change between distant values instantaneously. There is a built in loop that will require the needle to go through all intermediate values if the variable changes by more than 1.

When the gauge switches pages, the needle will smoothly sweep to the next position instead of moving instantaneously. Again, this is for the "analog" feel.

# Notes:

- Compatibility with other vehicles is unknown at this point. Most likely, any other cars using the Volvo Bosch ME9 implementation will work with no modifications (although I have heard some reports that the broadcast IDs change from vehicle to vehicle, so that may need to be tweaked)
- Volvo uses extended IDs for their CAN frames. I am not 100% sure why they do this yet.
- Volvos of this vintage have 2 CAN networks, a High Speed bus at 500kbps and a Low Speed bus at 125kbps. The high speed bus is connected to the ECU, steering modules, braking and other modules. The low speed bus is connected to the radio, door modules, instrument cluster and other associated accessories. The CEM (Central Electronics Module) acts as a gateway between the high and low speed busses. This project connects to both the high and low speed busses.
- Older vehicles have a diagnostic relay that needs to be activated via K-line in order to access the CAN buses via the OBD2 port. Because mine does not require this, I don't have much information about it.

# Other Resources:
- http://hackingvolvo.blogspot.com/
- https://github.com/hackingvolvo
- http://opengarages.org/handbook/ebook/
