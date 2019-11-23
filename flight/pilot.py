from dronekit import connect, VehicleMode, LocationGlobalRelative
from time import sleep
from pymavlink.mavutil import mavlink


class Pilot:
    def __init__(self, port):
        self.servo_num = 5

        # Connect to the Vehicle.
        self.vehicle = connect(port, wait_ready=True, baud=57600)

        while(True):
            # print vehicle state
            print(" GPS: %s" % self.vehicle.gps_0)
            print(" Battery: %s" % self.vehicle.battery)
            print(" Last Heartbeat: %s" % self.vehicle.last_heartbeat)
            print(" Is Armable?: %s" % self.vehicle.is_armable)
            print(" System status: %s" % self.vehicle.system_status.state)
            print(" Mode: %s" % self.vehicle.mode.name)    # settable

            # Set mode to guided - this is optional as the goto method will change the mode if needed.
            self.vehicle.mode = VehicleMode("STABILIZE")

            # Set the target location in global-relative frame
            # a_location = LocationGlobalRelative(-34.364114, 149.166022, 30)
            # vehicle.simple_goto(a_location)

            for i in range(500, 2400, 100):
                self.turn_servo(i)
                sleep(0.1)

            self.turn_servo(500)
            sleep(1)

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
