import dickerbotclient
import cv2
import time

# This example shows how to read IMU, distance, and camera data from the DickerBot.
# Keshav Shankar, 04/27/2025

if __name__ == "__main__":
    # Create DickerBot class object
    bot = dickerbotclient.DickerBotClient()

    # Connect to the socket
    bot.connect("10.0.0.8", 8765)
    time.sleep(1) # Wait for connection

    try:
        while True:
            # Get and display IMU and distance data
            sensor_data = bot.get_sensor_data()
            print("Sensor Data:", sensor_data)

            # Get and display camera data
            image = bot.get_image_data()
            if image is not None:
                print("Image received")
                cv2.imshow("Camera View", image)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

    except KeyboardInterrupt:
        print("Stopping...")
    finally:
        # Disconnect from socket
        bot.disconnect()

        cv2.destroyAllWindows()