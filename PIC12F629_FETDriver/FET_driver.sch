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
LIBS:microchip-2
LIBS:microchip-3
LIBS:microchip-4
LIBS:microchip-5
LIBS:PIC_FET_driver-cache
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
L PIC12F629P IC1
U 1 1 55BFB894
P 5900 3350
F 0 "IC1" H 4500 3950 50  0000 L BNN
F 1 "PIC12F629P" H 5100 3150 50  0000 L BNN
F 2 "microchip-3-DIL8" H 5900 3500 50  0001 C CNN
F 3 "" H 5900 3350 60  0000 C CNN
	1    5900 3350
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_DGS Q1
U 1 1 55BFBB45
P 6200 3200
F 0 "Q1" H 6500 3250 50  0000 R CNN
F 1 " " H 6850 3150 50  0000 R CNN
F 2 "" H 6400 3300 29  0000 C CNN
F 3 "" H 6200 3200 60  0000 C CNN
	1    6200 3200
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 55BFBC14
P 6000 3400
F 0 "R2" V 6080 3400 50  0000 C CNN
F 1 "15k" V 6000 3400 50  0000 C CNN
F 2 "" V 5930 3400 30  0000 C CNN
F 3 "" H 6000 3400 30  0000 C CNN
	1    6000 3400
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 55BFBD15
P 4200 3100
F 0 "C1" H 4225 3200 50  0000 L CNN
F 1 "100nF" H 4225 3000 50  0000 L CNN
F 2 "" H 4238 2950 30  0000 C CNN
F 3 "" H 4200 3100 60  0000 C CNN
	1    4200 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4300 2850 4200 2850
Wire Wire Line
	4200 2500 4200 2950
Wire Wire Line
	4050 3350 4300 3350
Wire Wire Line
	4200 3250 4200 3600
Wire Wire Line
	4200 3600 6550 3600
Wire Wire Line
	6000 3600 6000 3550
Connection ~ 4200 3350
Wire Wire Line
	6300 3600 6300 3400
Connection ~ 6000 3600
$Comp
L R R1
U 1 1 55BFBFFA
P 6000 3000
F 0 "R1" V 6080 3000 50  0000 C CNN
F 1 "100" V 6000 3000 50  0000 C CNN
F 2 "" V 5930 3000 30  0000 C CNN
F 3 "" H 6000 3000 30  0000 C CNN
	1    6000 3000
	1    0    0    -1  
$EndComp
Wire Wire Line
	6000 3150 6000 3250
$Comp
L CONN_01X02 P3
U 1 1 55BFE512
P 6750 2450
F 0 "P3" H 6750 2600 50  0000 C CNN
F 1 "POWER" V 6850 2450 50  0000 C CNN
F 2 "" H 6750 2450 60  0000 C CNN
F 3 "" H 6750 2450 60  0000 C CNN
	1    6750 2450
	1    0    0    1   
$EndComp
$Comp
L CONN_01X02 P2
U 1 1 55BFE576
P 5900 2450
F 0 "P2" H 5900 2600 50  0000 C CNN
F 1 "LOAD" V 6000 2450 50  0000 C CNN
F 2 "" H 5900 2450 60  0000 C CNN
F 3 "" H 5900 2450 60  0000 C CNN
	1    5900 2450
	-1   0    0    1   
$EndComp
Connection ~ 6300 3600
Wire Wire Line
	6100 2400 6550 2400
$Comp
L CP C2
U 1 1 55BFE830
P 6300 2600
F 0 "C2" H 6325 2700 50  0000 L CNN
F 1 "10μf" H 6325 2500 50  0000 L CNN
F 2 "" H 6338 2450 30  0000 C CNN
F 3 "" H 6300 2600 60  0000 C CNN
	1    6300 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 3000 6300 2750
Wire Wire Line
	6300 2450 6300 2400
Connection ~ 6300 2400
Wire Wire Line
	6100 2500 6100 2750
Connection ~ 6300 2750
Wire Wire Line
	6550 3600 6550 2500
Connection ~ 6000 3200
Wire Wire Line
	5700 2950 5900 2950
Wire Wire Line
	5900 2950 5900 2850
Wire Wire Line
	5900 2850 6000 2850
$Comp
L CONN_01X03 P1
U 1 1 55C107E6
P 3850 2500
F 0 "P1" H 3850 2700 50  0000 C CNN
F 1 "SERVO CONN" V 3950 2500 50  0000 C CNN
F 2 "" H 3850 2500 60  0000 C CNN
F 3 "" H 3850 2500 60  0000 C CNN
	1    3850 2500
	-1   0    0    1   
$EndComp
Wire Wire Line
	4050 2500 4200 2500
Connection ~ 4200 2850
Wire Wire Line
	4050 2600 4050 3350
Wire Wire Line
	4050 2400 4350 2400
Wire Wire Line
	4350 2400 4350 2650
Wire Wire Line
	4350 2650 5800 2650
Wire Wire Line
	5800 2650 5800 3150
Wire Wire Line
	5800 3150 5700 3150
Wire Wire Line
	6100 2750 6300 2750
$EndSCHEMATC
