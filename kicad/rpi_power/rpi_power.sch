EESchema Schematic File Version 4
EELAYER 30 0
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
L Connector_Generic:Conn_01x04 J1
U 1 1 5FD578F8
P 7150 3400
F 0 "J1" H 7150 3600 50  0000 C CNN
F 1 "RPI_POWER" V 7250 3350 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B4B-EH-A_1x04_P2.50mm_Vertical" H 7150 3400 50  0001 C CNN
F 3 "~" H 7150 3400 50  0001 C CNN
	1    7150 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 3300 6850 3300
Wire Wire Line
	6950 3400 6850 3400
Wire Wire Line
	6850 3400 6850 3300
Connection ~ 6850 3300
$Comp
L power:GNDD #PWR07
U 1 1 5FD57DA6
P 6850 3650
F 0 "#PWR07" H 6850 3400 50  0001 C CNN
F 1 "GNDD" H 6854 3495 50  0000 C CNN
F 2 "" H 6850 3650 50  0001 C CNN
F 3 "" H 6850 3650 50  0001 C CNN
	1    6850 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 3600 6850 3600
Wire Wire Line
	6950 3500 6850 3500
Wire Wire Line
	6850 3500 6850 3600
Connection ~ 6850 3600
$Comp
L Device:CP_Small C2
U 1 1 5FD57DCE
P 6050 2650
F 0 "C2" H 6138 2696 50  0000 L CNN
F 1 "470.0" H 6138 2605 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 6050 2650 50  0001 C CNN
F 3 "~" H 6050 2650 50  0001 C CNN
	1    6050 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 2750 6050 3000
Wire Wire Line
	6050 3000 6450 3000
$Comp
L Device:C_Small C3
U 1 1 5FD57DE0
P 6450 2650
F 0 "C3" H 6542 2696 50  0000 L CNN
F 1 "0.1" H 6542 2605 50  0000 L CNN
F 2 "Capacitor_THT:C_Rect_L7.0mm_W3.5mm_P5.00mm" H 6450 2650 50  0001 C CNN
F 3 "~" H 6450 2650 50  0001 C CNN
	1    6450 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6450 2750 6450 3000
Wire Wire Line
	5150 2850 5150 3000
Connection ~ 5150 3000
$Comp
L power:GNDD #PWR03
U 1 1 5FD57DFC
P 5150 3100
F 0 "#PWR03" H 5150 2850 50  0001 C CNN
F 1 "GNDD" H 5154 2945 50  0000 C CNN
F 2 "" H 5150 3100 50  0001 C CNN
F 3 "" H 5150 3100 50  0001 C CNN
	1    5150 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 3100 5150 3000
$Comp
L power:GNDD #PWR05
U 1 1 5FD57E09
P 5750 3100
F 0 "#PWR05" H 5750 2850 50  0001 C CNN
F 1 "GNDD" H 5754 2945 50  0000 C CNN
F 2 "" H 5750 3100 50  0001 C CNN
F 3 "" H 5750 3100 50  0001 C CNN
	1    5750 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5750 3100 5750 3000
Connection ~ 5750 3000
Wire Wire Line
	5750 3000 5750 2850
Wire Wire Line
	5750 3000 6050 3000
Connection ~ 6050 3000
Wire Wire Line
	5950 2300 6050 2300
Connection ~ 6050 2300
Wire Wire Line
	6050 2300 6450 2300
Wire Wire Line
	6450 2300 6450 2550
Wire Wire Line
	4350 2300 4100 2300
Wire Wire Line
	4100 2450 4100 2300
Connection ~ 4100 2300
$Comp
L Transistor_FET:IRLML5203 Q2
U 1 1 603CB87C
P 4550 2400
F 0 "Q2" V 4892 2400 50  0000 C CNN
F 1 "IRLML5203" V 4801 2400 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4750 2325 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml5203pbf.pdf?fileId=5546d462533600a40153566868da261d" H 4550 2400 50  0001 L CNN
	1    4550 2400
	0    1    -1   0   
$EndComp
$Comp
L Device:R R3
U 1 1 60410A16
P 4100 2600
F 0 "R3" H 4000 2550 50  0000 C CNN
F 1 "100k" H 3850 2650 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 4030 2600 50  0001 C CNN
F 3 "~" H 4100 2600 50  0001 C CNN
	1    4100 2600
	-1   0    0    1   
$EndComp
Wire Wire Line
	4550 2650 4550 2600
Wire Wire Line
	6850 3600 6850 3650
Wire Wire Line
	4100 3400 4550 3400
Wire Wire Line
	4550 3400 4550 2950
Wire Wire Line
	4100 2750 4100 3400
$Comp
L Transistor_FET:IRLML0030 Q1
U 1 1 60482082
P 4450 3800
F 0 "Q1" H 4650 3800 50  0000 L CNN
F 1 "IRLML6244" H 4650 3700 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4650 3725 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml0030pbf.pdf?fileId=5546d462533600a401535664773825df" H 4450 3800 50  0001 L CNN
	1    4450 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	4550 4200 4550 4000
$Comp
L Device:R R1
U 1 1 604F5B01
P 3700 4000
F 0 "R1" H 3600 3950 50  0000 C CNN
F 1 "1k" H 3600 4050 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 3630 4000 50  0001 C CNN
F 3 "~" H 3700 4000 50  0001 C CNN
	1    3700 4000
	-1   0    0    1   
$EndComp
Wire Wire Line
	3700 3850 3700 3800
Connection ~ 3700 3800
Wire Wire Line
	3700 3800 3400 3800
$Comp
L power:GNDD #PWR02
U 1 1 604D697C
P 4550 4200
F 0 "#PWR02" H 4550 3950 50  0001 C CNN
F 1 "GNDD" H 4554 4045 50  0000 C CNN
F 2 "" H 4550 4200 50  0001 C CNN
F 3 "" H 4550 4200 50  0001 C CNN
	1    4550 4200
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR01
U 1 1 605068A4
P 3700 4200
F 0 "#PWR01" H 3700 3950 50  0001 C CNN
F 1 "GNDD" H 3704 4045 50  0000 C CNN
F 2 "" H 3700 4200 50  0001 C CNN
F 3 "" H 3700 4200 50  0001 C CNN
	1    3700 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 4200 3700 4150
Text Label 3400 3800 0    50   ~ 0
STBY
Text Label 2500 1950 0    50   ~ 0
RPI_AC1
Wire Wire Line
	3250 1950 2500 1950
Wire Wire Line
	3250 2000 3250 1950
Wire Wire Line
	2900 2300 2950 2300
Wire Wire Line
	3700 3800 3850 3800
$Comp
L Transistor_FET:IRLML0030 Q3
U 1 1 605FA4E1
P 5350 3700
F 0 "Q3" H 5550 3800 50  0000 L CNN
F 1 "IRLML6244" H 5550 3700 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 5550 3625 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml0030pbf.pdf?fileId=5546d462533600a401535664773825df" H 5350 3700 50  0001 L CNN
	1    5350 3700
	-1   0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR04
U 1 1 6060E085
P 5250 4200
F 0 "#PWR04" H 5250 3950 50  0001 C CNN
F 1 "GNDD" H 5254 4045 50  0000 C CNN
F 2 "" H 5250 4200 50  0001 C CNN
F 3 "" H 5250 4200 50  0001 C CNN
	1    5250 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 4200 5250 3900
Wire Wire Line
	5250 3400 5250 3500
$Comp
L Transistor_FET:IRLML0030 Q4
U 1 1 6062F31C
P 5900 3950
F 0 "Q4" H 6105 3996 50  0000 L CNN
F 1 "IRLML6244" H 6105 3905 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 6100 3875 50  0001 L CIN
F 3 "https://www.infineon.com/dgdl/irlml0030pbf.pdf?fileId=5546d462533600a401535664773825df" H 5900 3950 50  0001 L CNN
	1    5900 3950
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5550 3700 5800 3700
Wire Wire Line
	5800 3700 5800 3750
$Comp
L power:GNDD #PWR06
U 1 1 606442E7
P 5800 4200
F 0 "#PWR06" H 5800 3950 50  0001 C CNN
F 1 "GNDD" H 5804 4045 50  0000 C CNN
F 2 "" H 5800 4200 50  0001 C CNN
F 3 "" H 5800 4200 50  0001 C CNN
	1    5800 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 4200 5800 4150
$Comp
L Device:R R5
U 1 1 60676FD1
P 5800 3500
F 0 "R5" H 5700 3450 50  0000 C CNN
F 1 "100k" H 5550 3550 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 5730 3500 50  0001 C CNN
F 3 "~" H 5800 3500 50  0001 C CNN
	1    5800 3500
	-1   0    0    1   
$EndComp
Wire Wire Line
	5800 3650 5800 3700
Connection ~ 5800 3700
Wire Wire Line
	4550 3400 4550 3600
Connection ~ 4550 3400
Wire Wire Line
	5250 3400 4550 3400
Wire Wire Line
	5800 3350 5800 3300
$Comp
L Diode_Bridge:GBU8K D1
U 1 1 5FD57DB5
P 3250 2300
F 0 "D1" H 3400 2550 50  0000 L CNN
F 1 "GBU8K" H 3400 2450 50  0000 L CNN
F 2 "Diode_THT:Diode_Bridge_Vishay_GBU" H 3400 2425 50  0001 L CNN
F 3 "http://www.vishay.com/docs/88656/gbu8a.pdf" H 3250 2300 50  0001 C CNN
	1    3250 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 2300 3650 2300
Connection ~ 3650 2300
Wire Wire Line
	3650 2300 4100 2300
Connection ~ 6450 2300
Wire Wire Line
	6450 2300 6850 2300
Wire Wire Line
	6850 2300 6850 3300
$Comp
L Device:R R7
U 1 1 608EA7AA
P 6550 3500
F 0 "R7" H 6450 3450 50  0000 C CNN
F 1 "10k" H 6350 3550 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 6480 3500 50  0001 C CNN
F 3 "~" H 6550 3500 50  0001 C CNN
	1    6550 3500
	-1   0    0    1   
$EndComp
Wire Wire Line
	6550 3350 6550 3300
Wire Wire Line
	2900 2300 2900 3000
Wire Wire Line
	2900 3000 3650 3000
Connection ~ 3650 3000
Wire Wire Line
	3650 3000 5150 3000
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 603E8660
P 7150 3950
F 0 "J2" H 7100 4050 50  0000 L CNN
F 1 "RPI_OFF" V 7250 3750 50  0000 L CNN
F 2 "" H 7150 3950 50  0001 C CNN
F 3 "~" H 7150 3950 50  0001 C CNN
	1    7150 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 3950 6550 3950
$Comp
L power:GNDD #PWR08
U 1 1 603EB0D7
P 6850 4200
F 0 "#PWR08" H 6850 3950 50  0001 C CNN
F 1 "GNDD" H 6854 4045 50  0000 C CNN
F 2 "" H 6850 4200 50  0001 C CNN
F 3 "" H 6850 4200 50  0001 C CNN
	1    6850 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 4050 6850 4050
Wire Wire Line
	6850 4050 6850 4200
Wire Wire Line
	6550 3650 6550 3950
$Comp
L ampcontrol-f103:XL4015_module U1
U 1 1 603BCAE3
P 5450 2550
F 0 "U1" H 5450 3065 50  0000 C CNN
F 1 "XL4015_module" H 5450 2974 50  0000 C CNN
F 2 "" H 5750 2950 50  0001 C CNN
F 3 "" H 6800 2450 50  0001 C CNN
	1    5450 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 2300 4950 2300
Wire Wire Line
	6050 2300 6050 2550
$Comp
L Device:R R6
U 1 1 603E89A2
P 6300 3950
F 0 "R6" V 6200 3950 50  0000 C CNN
F 1 "10k" V 6400 3900 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 6230 3950 50  0001 C CNN
F 3 "~" H 6300 3950 50  0001 C CNN
	1    6300 3950
	0    1    1    0   
$EndComp
Connection ~ 6550 3300
Wire Wire Line
	6550 3300 6850 3300
Connection ~ 6550 3950
Wire Wire Line
	6100 3950 6150 3950
Wire Wire Line
	5800 3300 6550 3300
Wire Wire Line
	3650 2850 3650 3000
Wire Wire Line
	3650 2300 3650 2650
$Comp
L Device:CP_Small C1
U 1 1 5FD57DBF
P 3650 2750
F 0 "C1" H 3738 2796 50  0000 L CNN
F 1 "1000.0" H 3738 2705 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm_P7.50mm" H 3650 2750 50  0001 C CNN
F 3 "~" H 3650 2750 50  0001 C CNN
	1    3650 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3250 2600 3250 2900
Wire Wire Line
	3250 2900 2500 2900
Text Label 2500 2900 0    50   ~ 0
RPI_AC2
Wire Wire Line
	6450 3950 6550 3950
$Comp
L Device:R R2
U 1 1 603F3C32
P 4000 3800
F 0 "R2" V 3900 3800 50  0000 C CNN
F 1 "10k" V 4100 3750 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 3930 3800 50  0001 C CNN
F 3 "~" H 4000 3800 50  0001 C CNN
	1    4000 3800
	0    1    1    0   
$EndComp
Wire Wire Line
	4150 3800 4250 3800
$Comp
L Device:R R4
U 1 1 60411DEB
P 4550 2800
F 0 "R4" H 4450 2750 50  0000 C CNN
F 1 "10k" H 4350 2850 50  0000 L CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 4480 2800 50  0001 C CNN
F 3 "~" H 4550 2800 50  0001 C CNN
	1    4550 2800
	-1   0    0    1   
$EndComp
$EndSCHEMATC
