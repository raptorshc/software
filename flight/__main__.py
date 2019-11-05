import time
import argparse
from pilot import Pilot

DEFAULT_PORT = '/dev/serial0'

if __name__ == '__main__':
    p = Pilot(DEFAULT_PORT)

    # except KeyboardInterrupt:
    #     # Close vehicle object before exiting script
    #     p.close()

    #     print("Completed")
