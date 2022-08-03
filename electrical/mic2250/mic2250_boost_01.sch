EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:switches
LIBS:relays
LIBS:motors
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
LIBS:mic2250
LIBS:mic2250_boost_01-cache
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
L MIC2250 U1
U 1 1 59DDF3A9
P 3050 1850
F 0 "U1" H 3050 1550 60  0000 C CNN
F 1 "MIC2250" H 3050 2150 60  0000 C CNN
F 2 "lms6002-pmod.pretty:SOT-753_(TSOP-5)_Handsoldering" H 3450 1550 60  0001 C CNN
F 3 "" H 3450 1550 60  0001 C CNN
	1    3050 1850
	-1   0    0    -1  
$EndComp
$Comp
L D_Schottky D1
U 1 1 59DDF4D4
P 3950 1700
F 0 "D1" H 3950 1800 50  0000 C CNN
F 1 "40V, 1.1A" H 3950 1600 50  0000 C CNN
F 2 "w_smd_diode.pretty:do219ab" H 3950 1700 50  0001 C CNN
F 3 "SL40" H 3950 1700 50  0001 C CNN
	1    3950 1700
	-1   0    0    -1  
$EndComp
$Comp
L L L1
U 1 1 59DDF53B
P 3050 1350
F 0 "L1" V 3000 1350 50  0000 C CNN
F 1 "22 uH, 1.9A" V 3125 1350 50  0000 C CNN
F 2 "manuf.pretty:MURATA-DG6050C" H 3050 1350 50  0001 C CNN
F 3 "" H 3050 1350 50  0001 C CNN
	1    3050 1350
	0    -1   -1   0   
$EndComp
$Comp
L R R2
U 1 1 59DDF5E5
P 4400 2300
F 0 "R2" V 4500 2300 50  0000 C CNN
F 1 "15K" V 4300 2300 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 4330 2300 50  0001 C CNN
F 3 "" H 4400 2300 50  0001 C CNN
	1    4400 2300
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 59DDF67C
P 4400 1900
F 0 "R1" V 4500 1900 50  0000 C CNN
F 1 "130K" V 4300 1900 50  0000 C CNN
F 2 "Resistors_SMD:R_0805" V 4330 1900 50  0001 C CNN
F 3 "" H 4400 1900 50  0001 C CNN
	1    4400 1900
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 59DDF6E6
P 4800 2100
F 0 "C2" H 4825 2200 50  0000 L CNN
F 1 "2.2 uF" H 4825 2000 50  0000 L CNN
F 2 "boost:C_1206" H 4838 1950 50  0001 C CNN
F 3 "" H 4800 2100 50  0001 C CNN
	1    4800 2100
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 59DDF74F
P 2000 2100
F 0 "C1" H 2025 2200 50  0000 L CNN
F 1 "10 uF" H 2025 2000 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206" H 2038 1950 50  0001 C CNN
F 3 "" H 2000 2100 50  0001 C CNN
	1    2000 2100
	1    0    0    -1  
$EndComp
Text Label 2000 1700 0    60   ~ 0
V_IN
Text Label 2000 2600 0    60   ~ 0
GND
Text Label 4800 1700 0    60   ~ 0
V_OUT
Text Label 4400 2100 0    60   ~ 0
FB
Text Label 3600 1350 0    60   ~ 0
SW_OUT
Wire Wire Line
	3600 1700 3800 1700
Wire Wire Line
	1150 1700 2550 1700
Wire Wire Line
	2550 1350 2550 2000
Wire Wire Line
	2550 1350 2900 1350
Wire Wire Line
	3200 1350 3600 1350
Wire Wire Line
	3600 1350 3600 1700
Connection ~ 2550 1700
Wire Wire Line
	2000 1950 2000 1700
Wire Wire Line
	2000 2600 2000 2250
Connection ~ 2000 2600
Wire Wire Line
	4400 2600 4400 2450
Connection ~ 4400 2600
Wire Wire Line
	4400 2150 4400 2050
Wire Wire Line
	4100 1700 6000 1700
Wire Wire Line
	4800 1950 4800 1700
Connection ~ 4800 1700
Wire Wire Line
	4800 2600 4800 2250
Wire Wire Line
	3600 1850 3700 1850
Wire Wire Line
	3700 1850 3700 2600
Connection ~ 3700 2600
Wire Wire Line
	3600 2000 3950 2000
Wire Wire Line
	3950 2000 3950 2100
Wire Wire Line
	3950 2100 4400 2100
Connection ~ 4400 2100
Wire Wire Line
	4400 1750 4400 1700
Connection ~ 4400 1700
Connection ~ 4800 2600
Connection ~ 3600 1700
Connection ~ 2000 1700
Wire Wire Line
	6000 2600 1150 2600
Wire Wire Line
	1150 1700 1150 2100
Wire Wire Line
	1150 2600 1150 2200
Wire Wire Line
	6000 2200 6000 2600
Wire Wire Line
	6000 1700 6000 2100
$Comp
L Conn_01x02 J1
U 1 1 59DE351E
P 750 2200
F 0 "J1" H 750 2300 50  0000 C CNN
F 1 "IN" H 750 2000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 750 2200 50  0001 C CNN
F 3 "" H 750 2200 50  0001 C CNN
	1    750  2200
	-1   0    0    1   
$EndComp
Wire Wire Line
	1150 2100 950  2100
Wire Wire Line
	1150 2200 950  2200
$Comp
L Conn_01x02 J2
U 1 1 59DE361F
P 5800 2200
F 0 "J2" H 5800 2300 50  0000 C CNN
F 1 "OUT" H 5800 2000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 5800 2200 50  0001 C CNN
F 3 "" H 5800 2200 50  0001 C CNN
	1    5800 2200
	-1   0    0    1   
$EndComp
$EndSCHEMATC
