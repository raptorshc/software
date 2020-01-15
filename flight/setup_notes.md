1. Install Raspbian on a RPi3, set up environment to connect however you want.
2. Create python3 venv `python3 -m venv venv`
3. Source venv `source venv/bin/activate`
4. Install dev packages of libxml2 and libxslt `sudo apt-get install libxml2-dev libxslt-dev`
6. Install dronekit-python `python3 -m pip install dronekit dronekit-sitl mavproxy`
7. Follow instructions in 'Disable the OS control of the serial port' in this [tutorial](https://ardupilot.org/dev/docs/raspberry-pi-via-mavlink.html).
   1. You're probably using the latest version of Raspbian, so follow the note provided.
8. Reboot Raspberry Pi
9. Enable serial connection and disable bluetooth as in the [tutorial](https://ardupilot.org/dev/docs/raspberry-pi-via-mavlink.html).
   1.  `sudo nano /boot/config.txt` to open the file, then controls are like normal text editor. You can press ctrl-x then y to save.
10. You should then be able to test the connection by `sudo mavproxy.py --master=/dev/ttyAMA0 --baudrate [baudrate] --aircraft MyCopter`, replacing [baudrate] with whatever you've set the baudrate to.
11. 

Largely sourced from this page: https://ardupilot.org/dev/docs/raspberry-pi-via-mavlink.html