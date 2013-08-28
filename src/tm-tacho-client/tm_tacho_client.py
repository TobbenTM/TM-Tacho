import sys
from tm_tacho_io import *

def main():

	upspeed_pin = 23
	downspeed_pin = 24
	tacho_cylinder = 4
	tacho_max_rpm = 10000
	targetHost = "10.13.37.1"
	targetPort = 2002


	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((targetHost, targetPort))

	upspeed_gpio = gpioThread(upspeed_pin, rpmToFreq(tacho_max_rpm))
	upspeed_gpio.setDaemon(True)
	
	downspeed_gpio = gpioThread(downspeed_pin, rpmToFreq(tacho_max_rpm))
	downspeed_gpio.setDaemon(True)

	upspeed_gpio.start()
	downspeed_gpio.start()

	def killAll():
		upspeed_gpio.running = False
		downspeed_gpio.running = False

	def rpmToFreq(rpm):
		return rpm / 60 * tacho_cylinder
	
	while 1:
		try:
			input = sock.recv(2048)
			if input and input[0].isdigit():
				inputlist = input.split('|')
				downspeed_gpio.freq = rpmToFreq(int(inputlist[0].strip()))
				upspeed_gpio.freq = rpmToFreq(int(inputlist[1].strip()))
		except KeyboardError:
			killAll()
			sys.exit()

if __name__ == '__main__': main()