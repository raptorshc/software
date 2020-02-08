import time
import argparse
from pilot import Pilot
from time import sleep
from signal import signal, SIGINT


def handler(signal_received, frame):
    print("Pilot stopped.")
    p.stop()
    exit(0)


if __name__ == '__main__':
    signal(SIGINT, handler)
    p = Pilot()

    p.startup_sequence()
    p.save_power()
    while True:
        p.fly()
