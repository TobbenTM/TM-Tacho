import threading
import time
import RPi.GPIO as GPIO
import os
import sys

class gpioThread (threading.Thread):
	def __init__(self, pin, max):
		self.pin = pin
		self.freq = 20.0
		self.max = self.max
		self.running = True
		GPIO.setmode(GPIO.BCM)
		GPIO.setup(self.pin, GPIO.OUT)
		threading.Thread.__init__(self)
	def run(self):
		while self.running:
			if self.freq < 1.0:
				self.freq = 1.0
			if self.freq > self.max:
				self.freq = self.max

			GPIO.output(self.pin, True)			
			time.sleep(1.0/self.freq/2.0)
			GPIO.output(self.pin, False)
			time.sleep(1.0/self.freq/2.0)

def main():
	gpiothread = gpioThread(23)
	gpiothread.setDaemon(True)
	gpiothread.start()
	freq = 1.0

	def killAll():
		gpiothread.running = False

	while(1):
		try:
			time.sleep(0.25)
			print '  %d  -   %d'  % (gpiothread.freq, gpiothread.high)
			if gpiothread.high >= 50:
				gpiothread.freq = 50
			if gpiothread.high >= 200:
				gpiothread.freq = 5
			if gpiothread.high >= 250:
				gpiothread.freq = 200
		except KeyboardError:
			killAll()
			sys.exit()

if __name__ == '__main__': main()