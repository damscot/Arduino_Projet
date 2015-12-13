EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L +5V #PWR01
U 1 1 5664C14B
P 5000 2300
F 0 "#PWR01" H 5000 2150 50  0001 C CNN
F 1 "+5V" H 5000 2440 50  0000 C CNN
F 2 "" H 5000 2300 50  0000 C CNN
F 3 "" H 5000 2300 50  0000 C CNN
	1    5000 2300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 5664C167
P 4850 5150
F 0 "#PWR02" H 4850 4900 50  0001 C CNN
F 1 "GND" H 4850 5000 50  0000 C CNN
F 2 "" H 4850 5150 50  0000 C CNN
F 3 "" H 4850 5150 50  0000 C CNN
	1    4850 5150
	1    0    0    -1  
$EndComp
$Comp
L ATMEGA328-P IC1
U 1 1 5664C184
P 6150 3850
F 0 "IC1" H 5400 5100 50  0000 L BNN
F 1 "ATMEGA328-P" H 6550 2450 50  0000 L BNN
F 2 "Housings_DIP:DIP-28_W7.62mm_LongPads" H 6150 3850 50  0000 C CIN
F 3 "" H 6150 3850 50  0000 C CNN
	1    6150 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 2750 5000 2750
Wire Wire Line
	5000 2300 5000 3050
Wire Wire Line
	5250 4950 4850 4950
Wire Wire Line
	4850 4950 4850 5150
Wire Wire Line
	5250 5050 5150 5050
Wire Wire Line
	5150 5050 5150 4950
Connection ~ 5150 4950
Wire Wire Line
	5000 3050 5250 3050
Connection ~ 5000 2750
$Comp
L GND #PWR03
U 1 1 5664C2B1
P 8800 5100
F 0 "#PWR03" H 8800 4850 50  0001 C CNN
F 1 "GND" H 8800 4950 50  0000 C CNN
F 2 "" H 8800 5100 50  0000 C CNN
F 3 "" H 8800 5100 50  0000 C CNN
	1    8800 5100
	1    0    0    -1  
$EndComp
$Comp
L LED D1
U 1 1 5664C2DE
P 8800 4700
F 0 "D1" H 8800 4800 50  0000 C CNN
F 1 "LED" H 8800 4600 50  0000 C CNN
F 2 "LEDs:LED-3MM" H 8800 4700 50  0001 C CNN
F 3 "" H 8800 4700 50  0000 C CNN
	1    8800 4700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8800 5100 8800 4900
Wire Wire Line
	7150 2950 8800 2950
Wire Wire Line
	8800 3600 8800 4500
$Comp
L R R1
U 1 1 5664C697
P 8800 3450
F 0 "R1" V 8880 3450 50  0000 C CNN
F 1 "1000" V 8800 3450 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 8730 3450 50  0001 C CNN
F 3 "" H 8800 3450 50  0000 C CNN
	1    8800 3450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8800 2950 8800 3300
$EndSCHEMATC
