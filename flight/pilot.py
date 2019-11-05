from dronekit import connect, VehicleMode, LocationGlobalRelative


class Pilot:
    def __init__(self, port):
        # Connect to the Vehicle.
        vehicle = connect(port, wait_ready=True, baud=921600)

        # print vehicle state
        print(" GPS: %s" % vehicle.gps_0)
        print(" Battery: %s" % vehicle.battery)
        print(" Last Heartbeat: %s" % vehicle.last_heartbeat)
        print(" Is Armable?: %s" % vehicle.is_armable)
        print(" System status: %s" % vehicle.system_status.state)
        print(" Mode: %s" % vehicle.mode.name)    # settable

    def close(self):
        vehicle.close()
