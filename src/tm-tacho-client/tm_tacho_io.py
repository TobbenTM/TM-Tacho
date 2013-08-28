import threading
import time
import RPi.GPIO as GPIO

class gpioThread (threading.Thread):
	def __init__(self, pin, max):
		self.pin = pin
		self.freq = 1.0
		self.max = float(max)
		self.running = True
		GPIO.setmode(GPIO.BCM)
		GPIO.setup(self.pin, GPIO.OUT)
		threading.Thread.__init__(self)
	def run(self):
		try:
			while self.running:
				if self.freq < 1.0:
					self.freq = 1.0
				if self.freq > self.max:
					self.freq = self.max

				GPIO.output(self.pin, True)			
				time.sleep(0.002)
				GPIO.output(self.pin, False)
				time.sleep((1.0/self.freq/2.0) - 0.002)
		finally:
			GPIO.cleanup()