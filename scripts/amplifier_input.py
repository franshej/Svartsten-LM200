#!/usr/bin/env python3

import argparse
import RPi.GPIO as GPIO
import time

# Define global pin numbers
PIN_INPUT_1 = 12
PIN_INPUT_2 = 26

def setup_gpio():
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    GPIO.setup(PIN_INPUT_1, GPIO.IN)
    GPIO.setup(PIN_INPUT_2, GPIO.IN)
    time.sleep(1)

def switch_to_mode(mode):
    setup_gpio()
    if mode == 'MOODE':
        GPIO.setup(PIN_INPUT_1, GPIO.OUT)
        GPIO.setup(PIN_INPUT_2, GPIO.OUT)
        GPIO.output(PIN_INPUT_1, GPIO.HIGH)
        GPIO.output(PIN_INPUT_2, GPIO.HIGH)
    elif mode == 'ANALOG':
        GPIO.setup(PIN_INPUT_1, GPIO.IN)
        GPIO.setup(PIN_INPUT_2, GPIO.IN)

def main():
    parser = argparse.ArgumentParser(description='Switch between INPUT_MOODE and INPUT_ANALOG.')
    parser.add_argument('mode', choices=['MOODE', 'ANALOG'], help='Switch to INPUT_MOODE or INPUT_ANALOG')
    args = parser.parse_args()

    switch_to_mode(args.mode)

if __name__ == '__main__':
    main()