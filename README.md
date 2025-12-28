# Connect a Grove All-in-one SEN54 dust sensor to a ESP 32c3 with LoRa SX1278 Module RA-02

The ESP will read the data from the sensor send it with Lora.

## Pin layout ESP32

![image](https://github.com/resimons/esp32s-lora-sds011/blob/main/images/esp32-pin-layout.png)
![image](https://github.com/resimons/esp32s-lora-sds011/blob/main/images/Grove_SEN54_CompleteSetup.jpeg)

## Wiring

## Where to buy

[ESP-32C3](https://www.hobbyelectronica.nl/product/esp32c3-development-board-zonder-serial-chip/)
[SEN54 Groove all-in-one](https://www.kiwi-electronics.com/nl/grove-sen54-all-in-one-environmental-sensor-voc-rh-temp-pm1-0-2-5-4-10-11382)
[SX1278](https://elektronicavoorjou.nl/product/sx1278-lora-module-433m-10-km-ra-02/)

## How to connect SEN54 to ESP32
SEN54 | COLOUR | ESP
------------ |--------| -------------
VCC | RED    | 5,1V
GND | BLACK  | GND
SDA | WHITE  | GPIO04
SCL | YELLOW | GPIO05


# Pinlayout LoRa SX1278 Module ra-02

![image](https://images.tcdn.com.br/img/img_prod/557243/sx1278_lora_433mhz_ra_02_breakout_board_10km_959_1_20191128221303.png)

## Wiring LoRa SX1278 module with ESP32
SX1278 | COLOR | ESP32
-------- |-| ----------
3.3V | RED | 3.3V
GND | BLACK | GND
SCK | ORANGE | GPIO2
MOSI | WHITE | GPIO3
DIO0 | BROWN | GPIO6
NSS | PINK | GPIO7
MISO | GREY | GPIO10
RST | YELLOW | GPIO11

[Sample code](https://how2electronics.com/esp32-lora-sx1278-76-transmitter-receiver/)
