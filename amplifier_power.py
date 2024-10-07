#!/usr/bin/env python3

import argparse
import RPi.GPIO as GPIO
import time

# Define global pin numbers
PIN_SOFTSTART_OFF = 20
PIN_POWER_OFF = 16

def setup_gpio():
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    GPIO.setup(PIN_SOFTSTART_OFF, GPIO.IN)
    GPIO.setup(PIN_POWER_OFF, GPIO.IN)
    time.sleep(1)

def turn_on():
    setup_gpio()
    GPIO.setup(PIN_SOFTSTART_OFF, GPIO.OUT)
    GPIO.output(PIN_SOFTSTART_OFF, GPIO.HIGH)

def turn_off():
    setup_gpio()
    GPIO.setup(PIN_POWER_OFF, GPIO.OUT)
    GPIO.output(PIN_POWER_OFF, GPIO.HIGH)

def main():
    parser = argparse.ArgumentParser(description='Control amplifier power.')
    parser.add_argument('action', choices=['ON', 'OFF'], help='Turn amplifier ON or OFF')
    args = parser.parse_args()

    if args.action == 'ON':
        turn_on()
    elif args.action == 'OFF':
        turn_off()

if __name__ == '__main__':
    main()