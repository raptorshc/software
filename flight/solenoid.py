from RPi.GPIO import setup, output, OUT, setmode, BCM
from time import sleep


class Solenoid:
    def __init__(self, pin):
        self.pin = pin
        setmode(BCM)
        setup(self.pin, OUT)
        self.close()

    def open(self):
        output(self.pin, 0)
        self.status = 'open'

    def close(self):
        output(self.pin, 1)
        self.status = 'closed'


if __name__ == "__main__":
    s = Solenoid(23)
    while True:
        s.open()
        sleep(0.5)
        s.close()
        sleep(0.5)
