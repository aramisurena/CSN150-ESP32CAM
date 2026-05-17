# Cybersecurity: CSN150

# ESP32-CAM Software Motion Detection Security Monitor

## Purpose

The purpose of this project is to create a software-based motion detection security system using a low-cost IoT device. For this project, I used an ESP32-CAM to monitor an area, detect visual movement, and display security status through a web dashboard.

## Project Overview

This project is an expansion of my earlier ESP32-CAM lab work. Earlier in the semester, I used the ESP32-CAM to set up the Arduino environment and test basic ESP32 functionality. For the final cybersecurity project, I expanded the device into a software-based motion detection security monitor with a web dashboard.

The ESP32-CAM captures repeated grayscale camera frames and compares them over time. If the visual difference between frames passes a set motion threshold, the system reports a possible motion event. The result is displayed through both the Arduino Serial Monitor and a browser-based dashboard hosted by the ESP32-CAM.

From a cybersecurity perspective, this project demonstrates physical intrusion detection, IoT security monitoring, and awareness of unauthorized access near protected areas such as computers, desks, doorways, or network equipment.

## Cybersecurity Function

This project performs a basic intrusion detection function. Instead of detecting network attacks, it focuses on physical security. Physical access is an important part of cybersecurity because an unauthorized person with access to a device, router, or computer may be able to steal information, tamper with equipment, or compromise a system.

The ESP32-CAM helps monitor a protected area by detecting motion or major visual changes. When movement is detected, the system alerts the user through the Serial Monitor and the web dashboard.

## Equipment

* [ESP32-CAM](https://www.amazon.com/Aideepen-ESP32-CAM-Bluetooth-ESP32-CAM-MB-Arduino/dp/B08P2578LV/ref=sr_1_3?crid=4FY0ECFW0ZX7&keywords=ESP32+Cam&qid=1678902050&sprefix=esp32+cam%2Caps%2C240&sr=8-3)
* [USB Micro Data Cable](https://www.amazon.com/AmazonBasics-Male-Micro-Cable-Black/dp/B0711PVX6Z/ref=sr_1_1_sspa?keywords=micro+usb+data+cable&qid=1678902214&sprefix=Micro+USB+data+%2Caps%2C89&sr=8-1-spons&psc=1&spLa=ZW5jcnlwdGVkUXVhbGlmaWVyPUFaU0NaUVZHU1RFUlAmZW5jcnlwdGVkSWQ9QTA3NTA4MDVFVERCS01HVlgxM1YmZW5jcnlwdGVkQWRJZD1BMDE4NTE1NTIwWUdONkdWSzU1M1Amd2lkZ2V0TmFtZT1zcF9hdGYmYWN0aW9uPWNsaWNrUmVkaXJlY3QmZG9Ob3RMb2dDbGljaz10cnVl)
* Computer with Arduino IDE installed
* Wi-Fi network or mobile hotspot

## Software and Tools Used

* Arduino IDE
* ESP32 board package for Arduino
* ESP32 camera library
* WiFi library
* WebServer library
* GitHub for documentation
* ChatGPT for project planning, troubleshooting, and README support

## How It Works

1. The ESP32-CAM initializes the camera.
2. The device connects to a Wi-Fi network.
3. The ESP32-CAM starts a local web dashboard.
4. The camera captures a grayscale image as the baseline frame.
5. The camera continues capturing new frames at regular intervals.
6. The software compares the current frame to the previous frame.
7. The program calculates a difference score.
8. If the difference score is higher than the motion threshold, the dashboard displays `MOTION DETECTED`.
9. If the difference score is lower than the threshold, the dashboard displays `NO MOTION DETECTED`.

## Steps I Followed

1. Connected the ESP32-CAM to my computer using a USB cable and programmer board.
2. Opened the Arduino IDE.
3. Selected the `AI Thinker ESP32-CAM` board.
4. Installed or confirmed the ESP32 board package was available in Arduino IDE.
5. Created a new Arduino sketch for the motion detection dashboard.
6. Added my Wi-Fi or mobile hotspot name and password to the code.
7. Uploaded the code to the ESP32-CAM.
8. Opened the Serial Monitor at `115200` baud.
9. Pressed the reset button on the ESP32-CAM.
10. Confirmed that the ESP32-CAM connected to Wi-Fi.
11. Copied the dashboard IP address from the Serial Monitor.
12. Opened the dashboard IP address in a web browser.
13. Tested the motion detection by moving in front of the camera.
14. Verified that the dashboard changed between `NO MOTION DETECTED` and `MOTION DETECTED`.

## Problems and Solutions

### Problem 1: ESP32-CAM Did Not Connect to Wi-Fi

**Problem:**  
The ESP32-CAM did not successfully connect to the Wi-Fi network, and the dashboard IP address appeared blank.

**Solution:**  
I checked the Wi-Fi name and password in the code. I also tested the project using a mobile hotspot. Once the ESP32-CAM successfully connected to the hotspot, the Serial Monitor displayed the dashboard IP address.

### Problem 2: Dashboard IP Address Was Blank

**Problem:**  
The Serial Monitor did not show a valid dashboard IP address.

**Solution:**  
I realized that the ESP32-CAM only displays an IP address after it successfully connects to Wi-Fi. I corrected the Wi-Fi settings and reset the ESP32-CAM. After reconnecting, the IP address appeared in the Serial Monitor.


## Final Report
The ESP32-CAM was successfully configured as a software-based motion detection security monitor. The device connects to Wi-Fi, captures repeated grayscale camera frames, compares the frames for visual changes, and reports motion events through a browser-based dashboard.

This project demonstrates how a low-cost IoT device can be used for physical security monitoring and basic intrusion detection. It also highlights important IoT security concerns, such as using trusted networks, securing Wi-Fi access, and avoiding public exposure of unsecured camera devices.
