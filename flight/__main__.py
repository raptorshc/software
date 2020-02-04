import time
import argparse
from pilot import Pilot
from time import sleep

DEFAULT_PORT = '/dev/serial0'

if __name__ == '__main__':
    p = Pilot(DEFAULT_PORT)

    p.startup_sequence()

    while True:
        if(p.flight_state == 0):
            p.launch()
        elif(p.flight_state == 1):
            p.ascent()
        elif(p.flight_state == 3):
            p.landed()
        else:
            p.descent()
