EESchema Schematic File Version 4
EELAYER 29 0
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
L Diode_Bridge:GBU8K D1
U 1 1 5CF43CC7
P 5400 3650
F 0 "D1" H 5600 3850 50  0000 L CNN
F 1 "GBU606" H 5600 3750 50  0000 L CNN
F 2 "Diode_THT:Diode_Bridge_Vishay_GBU" H 5550 3775 50  0001 L CNN
F 3 "http://www.vishay.com/docs/88656/gbu8a.pdf" H 5400 3650 50  0001 C CNN
	1    5400 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:CP_Small C1
U 1 1 5CF45E82
P 5800 3850
F 0 "C1" H 5888 3896 50  0000 L CNN
F 1 "10000.0" H 5888 3805 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D25.0mm_P10.00mm_SnapIn" H 5800 3850 50  0001 C CNN
F 3 "~" H 5800 3850 50  0001 C CNN
	1    5800 3850
	1    0    0    -1  
$EndComp
$Comp
L Device:CP_Small C2
U 1 1 5CF46753
P 5800 4150
F 0 "C2" H 5888 4196 50  0000 L CNN
F 1 "10000.0" H 5888 4105 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D25.0mm_P10.00mm_SnapIn" H 5800 4150 50  0001 C CNN
F 3 "~" H 5800 4150 50  0001 C CNN
	1    5800 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 3350 5400 3300
Wire Wire Line
	5400 3300 4650 3300
Wire Wire Line
	5400 3950 5400 4000
Wire Wire Line
	5400 4000 4650 4000
Wire Wire Line
	5700 3650 5800 3650
Wire Wire Line
	5800 3650 5800 3750
Wire Wire Line
	5800 4250 5800 4300
Wire Wire Line
	5800 4300 5050 4300
Wire Wire Line
	5050 4300 5050 3650
Wire Wire Line
	5050 3650 5100 3650
Wire Wire Line
	5800 4050 5800 4000
Wire Wire Line
	4550 3650 4850 3650
Wire Wire Line
	4850 3650 4850 3750
$Comp
L power:GNDPWR #PWR0101
U 1 1 5CF4C77B
P 4850 3750
F 0 "#PWR0101" H 4850 3550 50  0001 C CNN
F 1 "GNDPWR" H 4854 3596 50  0001 C CNN
F 2 "" H 4850 3700 50  0001 C CNN
F 3 "" H 4850 3700 50  0001 C CNN
	1    4850 3750
	1    0    0    -1  
$EndComp
$Comp
L power:GNDPWR #PWR0102
U 1 1 5CF4CECC
P 5550 4100
F 0 "#PWR0102" H 5550 3900 50  0001 C CNN
F 1 "GNDPWR" H 5554 3946 50  0001 C CNN
F 2 "" H 5550 4050 50  0001 C CNN
F 3 "" H 5550 4050 50  0001 C CNN
	1    5550 4100
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 4100 5550 4000
Wire Wire Line
	5550 4000 5800 4000
Connection ~ 5800 4000
Wire Wire Line
	5800 4000 5800 3950
$Comp
L Connector:Screw_Terminal_01x03 J1
U 1 1 5CF4FD4E
P 4350 3650
F 0 "J1" H 4500 3600 50  0000 C CNN
F 1 "POW_IN" H 4600 3700 50  0000 C CNN
F 2 "TerminalBlock:TerminalBlock_bornier-3_P5.08mm" H 4350 3650 50  0001 C CNN
F 3 "~" H 4350 3650 50  0001 C CNN
	1    4350 3650
	-1   0    0    1   
$EndComp
Wire Wire Line
	4550 3550 4650 3550
Wire Wire Line
	4650 3550 4650 3300
Wire Wire Line
	4550 3750 4650 3750
Wire Wire Line
	4650 3750 4650 4000
$Comp
L Connector:Screw_Terminal_01x03 J2
U 1 1 5CF51917
P 6700 3750
F 0 "J2" H 6800 3800 50  0000 L CNN
F 1 "POW_L" H 6800 3700 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-3_P5.08mm" H 6700 3750 50  0001 C CNN
F 3 "~" H 6700 3750 50  0001 C CNN
	1    6700 3750
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x03 J3
U 1 1 5CF532CE
P 6700 4200
F 0 "J3" H 6800 4150 50  0000 L CNN
F 1 "POW_R" H 6800 4250 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_bornier-3_P5.08mm" H 6700 4200 50  0001 C CNN
F 3 "~" H 6700 4200 50  0001 C CNN
	1    6700 4200
	1    0    0    1   
$EndComp
Wire Wire Line
	6500 4300 6250 4300
Connection ~ 5800 4300
Wire Wire Line
	6500 3850 6250 3850
Wire Wire Line
	6250 3850 6250 4300
Connection ~ 6250 4300
Wire Wire Line
	6250 4300 5800 4300
Wire Wire Line
	5800 3650 6450 3650
Connection ~ 5800 3650
Wire Wire Line
	6500 3750 6350 3750
Wire Wire Line
	6350 3750 6350 4000
Wire Wire Line
	6350 4200 6500 4200
Wire Wire Line
	5800 4000 6350 4000
Connection ~ 6350 4000
Wire Wire Line
	6350 4000 6350 4200
Wire Wire Line
	6500 4100 6450 4100
Wire Wire Line
	6450 4100 6450 3650
Connection ~ 6450 3650
Wire Wire Line
	6450 3650 6500 3650
Text Label 6050 3650 0    50   ~ 0
VCC
Text Label 5200 4300 0    50   ~ 0
VEE
Text Label 4700 3650 0    50   ~ 0
GND
Text Label 4750 3300 0    50   ~ 0
VAR1
Text Label 4750 4000 0    50   ~ 0
VAR2
$EndSCHEMATC
