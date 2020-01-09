# DerbyTimer

Simple derby car timer - for gravity fed races at Henry's his demos.

# Setup

Install photo diode sensor at finish. Make sure that the LED goes on/off as a car passes. Adjust the variable resistor as needd.

Wire something to the servo motor to hold the car.

# Racing

1. Place the car
2. Display should show waiting.
3. Press the red button
4. Ready .. set... go
5. timer runs
6. Passes the photo cell and timer stops.
7. Press the red button to reset it for the next race.

Pressing the red button during a race will also reset the timer.

The serial port outputs the same at a baud rate of 115200 baud.

# Electronics

Simple photo cell - in reverse bias to an LM386 opmap; one pin to a variable resistor to set the cut-off; the other pin to the diode and a 300k resitor devider. With an transistor to up the opamp output to drive an LED.

Servo is directly controlled with a PWM from the arduino. Unfortunately the 'wrong' ever running type.

Start button is wired between GPIO pin and ground. And needs SW pullup.

Buzzer is a simple transitor on a GPIO.


