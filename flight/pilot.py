from dronekit import connect, VehicleMode, LocationGlobalRelative
from time import sleep, time
from pymavlink.mavutil import mavlink
from solenoid import Solenoid

GROUND_ALT = 15.24  # 50ft
CUTDOWN_ALT = 609.6  # 1000ft


class Pilot:
    PRINT_TIME = 1
    SOL_PIN = 23

    def __init__(self, port):
        self.servo_num = 5
        self.last_print = time()

        # Connect to the Vehicle.
        self.vehicle = connect(port, wait_ready=True, baud=57600)
        self.vehicle.mode = VehicleMode("GUIDED")

        self.solenoid = Solenoid(self.SOL_PIN)
        self.flight_state = 0

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
        data = f"""GPS: {self.vehicle.gps_0}\n
        Battery: {self.vehicle.battery}\n
        Last Heartbeat: {self.vehicle.battery}\n
        Attitude: {self.vehicle.attitude}\n
        Groundspeed: {self.vehicle.groundspeed}\n
        Heading; {self.vehicle.heading}\n
        Location: {self.vehicle.location.global_frame}\n
        Solenoid: {self.vehicle.solenoid.status}"""

        print(data)

    def launch(self):
        if time() - self.last_print > self.PRINT_TIME:
            self.log_data()

        if self.vehicle.location.global_frame.alt > GROUND_ALT:
            self.flight_state = 1

    def ascent(self):
        if time() - self.last_print > self.PRINT_TIME:
            self.log_data()

        if self.vehicle.location.global_frame.alt > CUTDOWN_ALT:
            self.solenoid.open()
            self.flight_state = 2

    def descent(self):
        if time() - self.last_print > self.PRINT_TIME:
            self.log_data()

        if self.vehicle.location.global_frame.alt > GROUND_ALT:
            self.flight_state = 1

    def landed(self):
        if time() - self.last_print > self.PRINT_TIME:
            self.log_data()
            self.vehicle.play_tune(AA)

    def startup_sequence(self):
        self.vehicle.play_tune(AA)
        for i in range(10):
            for i in range(500, 2400, 100):
                p.turn_servo(i)
                sleep(0.1)

        p.turn_servo(500)


if __name__ == "__main__":
    p = Pilot('/dev/serial0')
    p.startup_sequence()

    while True:
        p.descent()
