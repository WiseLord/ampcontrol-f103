EESchema Schematic File Version 4
EELAYER 26 0
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
L power:GNDD #PWR0101
U 1 1 5C043025
P 4100 5400
F 0 "#PWR0101" H 4100 5150 50  0001 C CNN
F 1 "GNDD" H 4104 5245 50  0000 C CNN
F 2 "" H 4100 5400 50  0001 C CNN
F 3 "" H 4100 5400 50  0001 C CNN
	1    4100 5400
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 5200 4000 5300
Wire Wire Line
	4000 5300 4100 5300
Wire Wire Line
	4200 5300 4200 5200
Wire Wire Line
	4100 5200 4100 5300
Connection ~ 4100 5300
Wire Wire Line
	4100 5300 4200 5300
Wire Wire Line
	4100 5400 4100 5300
$Comp
L power:+3V3 #PWR0102
U 1 1 5C0430F5
P 4000 2000
F 0 "#PWR0102" H 4000 1850 50  0001 C CNN
F 1 "+3V3" H 4015 2173 50  0000 C CNN
F 2 "" H 4000 2000 50  0001 C CNN
F 3 "" H 4000 2000 50  0001 C CNN
	1    4000 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 2200 3900 2100
Wire Wire Line
	3900 2100 4000 2100
Wire Wire Line
	4100 2100 4100 2200
Wire Wire Line
	4000 2200 4000 2100
Connection ~ 4000 2100
Wire Wire Line
	4000 2100 4100 2100
Wire Wire Line
	4000 2000 4000 2100
$Comp
L power:+3V3 #PWR0103
U 1 1 5C0435B6
P 1300 3400
F 0 "#PWR0103" H 1300 3250 50  0001 C CNN
F 1 "+3V3" H 1315 3573 50  0000 C CNN
F 2 "" H 1300 3400 50  0001 C CNN
F 3 "" H 1300 3400 50  0001 C CNN
	1    1300 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0104
U 1 1 5C0435EA
P 1100 3600
F 0 "#PWR0104" H 1100 3350 50  0001 C CNN
F 1 "GNDD" H 1104 3445 50  0000 C CNN
F 2 "" H 1100 3600 50  0001 C CNN
F 3 "" H 1100 3600 50  0001 C CNN
	1    1100 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 3500 1100 3500
Wire Wire Line
	1100 3500 1100 3600
Wire Wire Line
	1300 3600 1300 3400
NoConn ~ 1500 3700
Wire Wire Line
	1500 3800 1250 3800
Text Label 1250 3800 0    50   ~ 0
RS
Wire Wire Line
	1500 3900 1250 3900
Wire Wire Line
	1250 4000 1500 4000
Wire Wire Line
	1300 3600 1500 3600
Wire Wire Line
	1500 4900 1250 4900
Wire Wire Line
	1500 5000 1250 5000
Wire Wire Line
	1500 5100 1250 5100
Wire Wire Line
	1500 5300 1250 5300
Text Label 1250 3900 0    50   ~ 0
WR
Text Label 1250 4000 0    50   ~ 0
RD
Text Label 1250 4900 0    50   ~ 0
CS
Text Label 1250 5000 0    50   ~ 0
F_CS
Text Label 1250 5100 0    50   ~ 0
RST
Text Label 1250 5300 0    50   ~ 0
BCKL
NoConn ~ 1500 5400
$Comp
L Connector_Generic:Conn_02x20_Top_Bottom J1
U 1 1 5C047533
P 1700 4400
F 0 "J1" H 1750 5517 50  0000 C CNN
F 1 "Conn_LCD" H 1750 5426 50  0000 C CNN
F 2 "ampcontrol-f103:PinSocket_2x20_P2.54mm_Vertical_Top_Bottom" H 1700 4400 50  0001 C CNN
F 3 "~" H 1700 4400 50  0001 C CNN
	1    1700 4400
	1    0    0    -1  
$EndComp
NoConn ~ 2000 5400
NoConn ~ 2000 5300
NoConn ~ 2000 4600
Wire Wire Line
	2000 3500 2250 3500
Wire Wire Line
	2250 3600 2000 3600
Wire Wire Line
	2000 3700 2250 3700
Wire Wire Line
	2250 3800 2000 3800
Wire Wire Line
	2000 3900 2250 3900
Wire Wire Line
	2250 4000 2000 4000
Wire Wire Line
	2000 4100 2250 4100
Wire Wire Line
	2250 4200 2000 4200
Wire Wire Line
	2000 4300 2250 4300
Wire Wire Line
	2000 4400 2250 4400
Wire Wire Line
	2000 4500 2250 4500
Wire Wire Line
	2000 4700 2250 4700
Wire Wire Line
	2000 4800 2250 4800
Wire Wire Line
	2000 4900 2250 4900
Wire Wire Line
	2000 5000 2250 5000
Wire Wire Line
	2000 5100 2250 5100
Wire Wire Line
	2000 5200 2250 5200
NoConn ~ 1500 5200
Text Label 2250 3500 2    50   ~ 0
DB0
Text Label 2250 3600 2    50   ~ 0
DB1
Text Label 2250 3700 2    50   ~ 0
DB2
Text Label 2250 3800 2    50   ~ 0
DB3
Text Label 2250 3900 2    50   ~ 0
DB4
Text Label 2250 4000 2    50   ~ 0
DB5
Text Label 2250 4100 2    50   ~ 0
DB6
Text Label 2250 4200 2    50   ~ 0
DB7
Text Label 2250 4300 2    50   ~ 0
T_CLK
Text Label 2250 4400 2    50   ~ 0
T_CS
Text Label 2250 4500 2    50   ~ 0
T_DIN
Text Label 2250 4700 2    50   ~ 0
T_DO
Text Label 2250 4800 2    50   ~ 0
T_IRQ
Text Label 2250 4900 2    50   ~ 0
SD_DO
Text Label 2250 5000 2    50   ~ 0
SD_CLK
Text Label 2250 5100 2    50   ~ 0
SD_DIN
Text Label 2250 5200 2    50   ~ 0
SD_CS
$Comp
L MCU_ST_STM32F1:STM32F103CBTx U1
U 1 1 5C050C11
P 4000 3700
F 0 "U1" H 3450 5150 50  0000 C CNN
F 1 "STM32F103CBTx" H 4000 3700 50  0000 C CNN
F 2 "Package_QFP:LQFP-48_7x7mm_P0.5mm" H 3400 2300 50  0001 R CNN
F 3 "http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/CD00161566.pdf" H 4000 3700 50  0001 C CNN
	1    4000 3700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	3400 3500 3150 3500
Wire Wire Line
	3400 3600 3150 3600
Wire Wire Line
	3150 3700 3400 3700
Wire Wire Line
	3400 3800 3150 3800
Wire Wire Line
	3400 3900 3150 3900
Wire Wire Line
	3400 4000 3150 4000
Wire Wire Line
	3400 4100 3150 4100
Wire Wire Line
	3400 4200 3150 4200
Text Label 3150 3500 0    50   ~ 0
DB0
Text Label 3150 3600 0    50   ~ 0
DB1
Text Label 3150 3700 0    50   ~ 0
DB2
Text Label 3150 3800 0    50   ~ 0
DB3
Text Label 3150 3900 0    50   ~ 0
DB4
Text Label 3150 4000 0    50   ~ 0
DB5
Text Label 3150 4100 0    50   ~ 0
DB6
Text Label 3150 4200 0    50   ~ 0
DB7
Wire Wire Line
	3400 5000 3150 5000
Text Label 3150 5000 0    50   ~ 0
CS
Wire Wire Line
	4700 3900 4950 3900
Text Label 4950 3900 2    50   ~ 0
RD
Wire Wire Line
	4700 4000 4950 4000
Text Label 4950 4000 2    50   ~ 0
WR
Wire Wire Line
	4700 4100 4950 4100
Text Label 4950 4100 2    50   ~ 0
RS
Wire Wire Line
	4700 4200 4950 4200
Text Label 4950 4200 2    50   ~ 0
RST
Wire Wire Line
	4700 3100 4950 3100
Text Label 4950 3100 2    50   ~ 0
BCKL
NoConn ~ 1500 4100
NoConn ~ 1500 4200
NoConn ~ 1500 4300
NoConn ~ 1500 4400
NoConn ~ 1500 4500
NoConn ~ 1500 4600
NoConn ~ 1500 4700
NoConn ~ 1500 4800
NoConn ~ 1250 5000
NoConn ~ 2250 4300
NoConn ~ 2250 4400
NoConn ~ 2250 4500
NoConn ~ 2250 4700
NoConn ~ 2250 4800
NoConn ~ 2250 4900
NoConn ~ 2250 5000
NoConn ~ 2250 5100
NoConn ~ 2250 5200
$EndSCHEMATC
