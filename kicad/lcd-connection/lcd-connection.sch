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
L Connector_Generic:Conn_01x14 J12
U 1 1 60397DCA
P 2350 3050
F 0 "J12" H 2350 3750 50  0000 C CNN
F 1 "3.2 TFT SPI 240X320 V1.0" V 2450 3000 50  0000 C CNN
F 2 "" H 2350 3050 50  0001 C CNN
F 3 "~" H 2350 3050 50  0001 C CNN
	1    2350 3050
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2550 2450 2950 2450
Wire Wire Line
	2550 2550 2950 2550
Wire Wire Line
	2550 2650 2950 2650
Wire Wire Line
	2550 2750 2950 2750
Wire Wire Line
	2550 2850 2950 2850
Wire Wire Line
	2550 2950 2950 2950
Wire Wire Line
	2550 3050 2950 3050
Wire Wire Line
	2550 3150 2950 3150
Wire Wire Line
	2550 3250 2950 3250
Text Label 2950 2450 2    50   ~ 0
VCC
Text Label 2950 2550 2    50   ~ 0
GND
Text Label 2950 2650 2    50   ~ 0
CS
Text Label 2950 2750 2    50   ~ 0
RESET
Text Label 2950 2850 2    50   ~ 0
DC
Text Label 2950 2950 2    50   ~ 0
SDI(MOSI)
Text Label 2950 3050 2    50   ~ 0
SCK
Text Label 2950 3150 2    50   ~ 0
LED
Text Label 2950 3250 2    50   ~ 0
SDO(MISO)
NoConn ~ 2550 3350
NoConn ~ 2550 3450
NoConn ~ 2550 3550
NoConn ~ 2550 3650
Wire Notes Line
	2050 2150 2050 3950
Wire Notes Line
	2050 3950 3550 3950
Wire Notes Line
	3550 3950 3550 2150
Wire Notes Line
	3550 2150 2050 2150
Text Notes 2950 2250 2    50   ~ 0
SPI display connection
Text Notes 3000 3250 0    50   ~ 0
VCC pulled
Text Notes 3000 2750 0    50   ~ 0
VCC pulled
$Comp
L Connector_Generic:Conn_01x08 J21
U 1 1 603A2A1A
P 5900 3700
F 0 "J21" H 5900 4100 50  0000 C CNN
F 1 "Arduino_Digital_Low" V 6000 3650 50  0000 C CNN
F 2 "" H 5900 3700 50  0001 C CNN
F 3 "~" H 5900 3700 50  0001 C CNN
	1    5900 3700
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6100 3400 6500 3400
Wire Wire Line
	6500 3500 6100 3500
Wire Wire Line
	6100 3600 6500 3600
Wire Wire Line
	6500 3700 6100 3700
Wire Wire Line
	6100 3800 6500 3800
Wire Wire Line
	6500 3900 6100 3900
Text Label 6500 3400 2    50   ~ 0
LCD_D7
Text Label 6500 3500 2    50   ~ 0
LCD_D6
Text Label 6500 3600 2    50   ~ 0
LCD_D5
Text Label 6500 3700 2    50   ~ 0
LCD_D4
Text Label 6500 3800 2    50   ~ 0
LCD_D3
Text Label 6500 3900 2    50   ~ 0
LCD_D2
$Comp
L Connector_Generic:Conn_01x10 J22
U 1 1 603A6B52
P 5900 2550
F 0 "J22" H 5900 3050 50  0000 C CNN
F 1 "Arduino_Digital_High" V 6000 2450 50  0000 C CNN
F 2 "" H 5900 2550 50  0001 C CNN
F 3 "~" H 5900 2550 50  0001 C CNN
	1    5900 2550
	-1   0    0    -1  
$EndComp
NoConn ~ 6100 4000
NoConn ~ 6100 4100
Wire Wire Line
	6100 2950 6500 2950
Wire Wire Line
	6500 3050 6100 3050
Text Label 6500 2950 2    50   ~ 0
LCC_D1
Text Label 6500 3050 2    50   ~ 0
LCD_D0
$Comp
L Connector_Generic:Conn_01x08 J20
U 1 1 603AD155
P 4550 3700
F 0 "J20" H 4550 4100 50  0000 C CNN
F 1 "Arduino_Power" V 4650 3650 50  0000 C CNN
F 2 "" H 4550 3700 50  0001 C CNN
F 3 "~" H 4550 3700 50  0001 C CNN
	1    4550 3700
	-1   0    0    -1  
$EndComp
NoConn ~ 6100 2850
NoConn ~ 6100 2750
NoConn ~ 6100 2650
NoConn ~ 6100 2550
NoConn ~ 6100 2450
NoConn ~ 6100 2350
NoConn ~ 6100 2250
NoConn ~ 6100 2150
Wire Wire Line
	5150 4000 4750 4000
Text Label 5150 4000 2    50   ~ 0
GND
Wire Wire Line
	4750 3800 5150 3800
Wire Wire Line
	5150 3700 4750 3700
Text Label 5150 3700 2    50   ~ 0
3V3
Text Label 5150 3800 2    50   ~ 0
5V
NoConn ~ 4750 3400
NoConn ~ 4750 3500
NoConn ~ 4750 3600
NoConn ~ 4750 4100
$Comp
L Connector_Generic:Conn_01x06 J19
U 1 1 603B6A8F
P 4600 2350
F 0 "J19" H 4600 2650 50  0000 C CNN
F 1 "Arduino_Analog" V 4700 2300 50  0000 C CNN
F 2 "" H 4600 2350 50  0001 C CNN
F 3 "~" H 4600 2350 50  0001 C CNN
	1    4600 2350
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4800 2150 5200 2150
Wire Wire Line
	5200 2250 4800 2250
Wire Wire Line
	4800 2350 5200 2350
Wire Wire Line
	5200 2450 4800 2450
Wire Wire Line
	4800 2550 5200 2550
Text Label 5200 2150 2    50   ~ 0
LCD_RD
Text Label 5200 2250 2    50   ~ 0
LCD_WR
Text Label 5200 2350 2    50   ~ 0
LCD_RS
Text Label 5200 2450 2    50   ~ 0
LCD_CS
Text Label 5200 2550 2    50   ~ 0
LCD_RST
NoConn ~ 4750 3900
NoConn ~ 4800 2650
Text Notes 5500 1950 2    50   ~ 0
8-bit display connection
Wire Notes Line
	4350 1800 4350 4300
Wire Notes Line
	4350 4300 6600 4300
Wire Notes Line
	6600 4300 6600 1800
Wire Notes Line
	6600 1800 4350 1800
Text Notes 5250 2150 0    50   ~ 0
VCC pulled
Text Notes 5250 2550 0    50   ~ 0
VCC pulled
$EndSCHEMATC
