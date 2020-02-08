from dronekit import connect, VehicleMode, LocationGlobalRelative, Vehicle
from time import sleep, time
from sched import scheduler
from pymavlink.mavutil import mavlink
from solenoid import Solenoid
from subprocess import Popen

CUTDOWN_ALT = 609.6  # 1000ft
GROUND_ALT = 15.24  # 50ft


class Pilot:
    PRINT_TIME = 1
    SOL_PIN = 23
    DESTINATION = LocationGlobalRelative(-34.364114, 149.166022, 30)

    def __init__(self, port='/dev/serial0'):
        self.servo_num = 5
        self.last_print = time()

        # Connect to the Vehicle.
        self.vehicle = connect(port, wait_ready=True, baud=57600)
        self.vehicle.mode = VehicleMode("GUIDED")

        # initialize the solenoid using the default pin
        self.solenoid = Solenoid(self.SOL_PIN)
        self.flight_state = 0

        # schedule the data logging
        self.schedule = scheduler(time, sleep)
        self.schedule.enter(1, 1, log_data)  # once every second (first arg)

    def save_power(self):
        """ Turns off USB and HDMI to save power,
        MAKE SURE TO CALL stop() to re-enable! """
        process = Popen(
            "echo '1-1' |sudo tee /sys/bus/usb/drivers/usb/unbind", shell=True)
        process.wait()

        Popen("sudo /opt/vc/bin/tvservice -o", shell=True)

    def fly(self):
        # just use a scheduler for everything, that way we stay asleep
        # for a majority of time to save power
        self.schedule.run()

    def stop(self):
        process = Popen(
            "echo '1-1' |sudo tee /sys/bus/usb/drivers/usb/bind", shell=True)
        process.wait()

        Popen("sudo /opt/vc/bin/tvservice -p", shell=True)
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

    @vehicle.on_attribute('location.global_frame.alt')
    def location_listener(self, name, value):
        """ Altitude listener that performs cutdown and landed actions """
        if self.solenoid.status == 'closed' and value > CUTDOWN_ALT:
            # open solenoid and start autopilot
            self.solenoid.open()
            self.vehicle.simple_goto(DESTINATION)
        elif self.solenoid.status == 'open' and value < GROUND_ALT:
            # start scheduler for playing buzzer
            self.schedule.enter(1, 2, self.vehicle.play_tune, argument=A,)

    def startup_sequence(self):
        self.vehicle.play_tune(AA)
        for i in range(10):
            for i in range(500, 2400, 100):
                p.turn_servo(i)
                sleep(0.1)

        p.turn_servo(500)


if __name__ == "__main__":
    p = Pilot()
    p.startup_sequence()

    while True:
        p.fly()
