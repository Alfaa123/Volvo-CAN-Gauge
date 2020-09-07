# Volvo-CAN-Gauge
Reverse engineering the Volvo VIDA protocol to gather diagnostic information not available via OBD2 on Volvo cars.

# Hardware:
- 2011 Volvo C30 T5
- "generic" 128x64 OLED Display on I^2C protocol
- Machinna M1.1 (Old, no longer manufactured) (More info here: https://www.macchina.cc/guide/m1/software/software-setup-arduino-ide-1516)

**NOTE: The Machinna M1.1 is basically an Arduino Mega with a buck style regulator and a few built in interfaces. In this project, we're only using the CAN bus interface, which is identical to a regular CAN bus shield based on the MCP2515. The only change you'll need to make in order for this to run on a regular Mega + a CAN shield is to change the CS pin of the CAN shield, as the Machinna uses a 'non-arduino' pin for this purpose.**

# Included Libraries:
- U8g2: Used to drive the OLED display.
- Seeed CAN bus shield: CAN shield library with modifications to allow it to work with the Machinna M1.1

# Basic Functional Description:

The Volvo VIDA protocol is a basic message/response protocol not very different from ISO 15765-4. However, unlike ISO 15765-4, VIDA can also write controller firmware, activate actuators and run diagnostic tests.

In this project, we are only concerned with recieving data that would be otherwise unavailable with ISO 15765-4. Boost pressure, for example, is not available via ISO 15765-4 on Volvo cars. (a partial list of discovered codes from the VIDA database files is in Codes.txt)

We also use data from some broadcasted CAN frames that are used elsewhere in the car (for dashboard brightness, ignition status and headlights). Ideally, we would capture ALL of our information from broadcast frames as that involves a lot less overhead and traffic on the bus. However, some of the information that we need never gets broadcast, so we sometimes have no choice.

The code uses a psudo multi-tasking approach where the message recieve loop is always running if one of the other loops isn't currently running. This allows us to update/check broadcast frames in the background for brightness changes, ignition status changes and button presses and update the global variables accordingly.

In the display loop, we can show boost pressure, coolant temperature and intake temperature. Using the OLEDs massive resolution, we can also graph each of those values for a short amount of time to track dynamic changes.

For right now, the only button we track is the cruise control cancel button. Holding the button for more than 2 seconds changes the currently displayed page. Perhaps in the future, a menu system could be implemented.

# Other notes:

- Compatibility with other vehicles is unknown at this point. Most likely, any other cars using the Volvo Bosch ME9 implementation will work with no modifications (although I have heard some reports that the broadcast IDs change from vehicle to vehicle, so that may need to be tweaked)
- Volvo uses extended IDs for their CAN frames. I am not 100% sure why they do this yet.
- Volvos of this vintage have 2 CAN networks, a High Speed bus at 500kbps and a Low Speed bus at 125kbps. The high speed bus is connected to the ECU, steering modules, braking and other modules. The low speed bus is connected to the radio, door modules, instrument cluster and other associated accessories. The CEM (Central Electronics Module) acts as a gateway between the high and low speed busses. This project connects directly to the high speed bus.
- Older vehicles have a diagnostic relay that needs to be activated via K-line in order to access the CAN buses via the OBD2 port. Because mine does not require this, I don't have much information about it.
- The Arduino Mega is not fast enough to recieve all the traffic on the bus. I strongly suspect that we miss a lot of traffic with this implementation, espically because we don't use inturrupt based message handling.

# Other Resources:
- http://hackingvolvo.blogspot.com/
- https://github.com/hackingvolvo
- http://opengarages.org/handbook/ebook/
