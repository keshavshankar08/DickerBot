import dickerbotclient
import time

# This example shows how to write control data to the DickerBot.
# Keshav Shankar, 04/27/2025

if __name__ == "__main__":
    # Create DickerBot class object
    bot = dickerbotclient.DickerBotClient()

    # Connect to the socket
    bot.connect("10.0.0.8", 8765)
    time.sleep(1) # Wait for connection

    # Command robot to move forward, backward, turn left, turn right, and stop in 3 second intervals
    bot.set_control_data(100, 1, 100, 1) # forward
    time.sleep(3)
    bot.set_control_data(100, 2, 100, 2) # backward
    time.sleep(3)
    bot.set_control_data(0, 0, 100, 1) # turn left
    time.sleep(3)
    bot.set_control_data(100, 1, 0, 0) # turn right
    time.sleep(3)
    bot.set_control_data(0, 0, 0, 0) # stop

    # Disconnect from socket
    bot.disconnect()
