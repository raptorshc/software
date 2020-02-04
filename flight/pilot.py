from dronekit import connect, VehicleMode, LocationGlobalRelative
from time import sleep, time
from pymavlink.mavutil import mavlink


class Pilot:
    PRINT_TIME = 1
    def __init__(self, port):
        self.servo_num = 5
        self.last_print = time()

        # Connect to the Vehicle.
        self.vehicle = connect(port, wait_ready=True, baud=57600)
        self.vehicle.mode = VehicleMode("GUIDED")

        for i in range(10):
            for i in range(500, 2400, 100):
                self.turn_servo(i)
                sleep(0.1)

        self.turn_servo(500)
        sleep(1)

    def stop(self):
        self.vehicle.close()

    def turn_servo(self, pwm):
        msg = self.vehicle.message_factory.command_long_encode(
            0, 0,    # target_system, target_component
            mavlink.MAV_CMD_DO_SET_SERVO,  # command
            0,  # confirmation
            self.servo_num,    # servo number
            pwm,          # servo position between 1000 and 2000
            0, 0, 0, 0, 0)    # param 3 ~ 7 not used
        self.vehicle.send_mavlink(msg)

    def log_data(self):
        data = f"""GPS: {self.vehicle.gps_0}
        Battery: {self.vehicle.battery}
        Last Heartbeat: {self.vehicle.battery}
        Attitude: {self.vehicle.attitude}
        Groundspeed: {self.vehicle.groundspeed}
        Heading; {self.vehicle.heading}
        Location: {self.vehicle.location.global_frame}"""

        print(data)

    def launch(self):
        if time() - self.last_print > self.PRINT_TIME:
            self.log_data()
    
    def ascent(self):
        if time() - self.last_print > self.PRINT_TIME:
            self.log_data()

    def descent(self):
        # Set the target location in global-relative frame
        a_location = LocationGlobalRelative(-34.364114, 149.166022, 30)
        vehicle.simple_goto(a_location)

        if time() - self.last_print > self.PRINT_TIME:
            self.log_data()

    def landed(self):
        if time() - self.last_print > self.PRINT_TIME:
            self.log_data()
            self.vehicle.play_tune(AA)

if __name__ == "__main__":
    p = Pilot('/dev/serial0')
    while True:
        p.descent()
