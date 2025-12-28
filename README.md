# Connect a Grove All-in-one dust sensor to a ESP 32c3 with LoRa SX1278 Module RA-02

The ESP will read the data from the sensor send it with Lora.

## Pin layout ESP32

![image](https://github.com/resimons/esp32s-lora-sds011/blob/main/images/esp32-pin-layout.png)
![image](https://www.tinytronics.nl/image/cache/catalog/products_2022/nova-sds011-high-precision-laser-dust-sensor-1200x1200.jpg)

## Wiring

## Where to buy

[ESP-32S](https://elektronicavoorjou.nl/product/esp32-development-board-wifi-bluetooth)
[SDS011](https://www.tinytronics.nl/nl/sensoren/lucht/stof/nova-sds011-hoge-precisie-laser-stofsensor)
[SX1278](https://elektronicavoorjou.nl/product/sx1278-lora-module-433m-10-km-ra-02/)

## How to connect SDS011 to ESP32
SDS011 | COLOUR | ESP
------------ | ---------- | -------------
VCC | WHITE | 5,1V
GND | ORANGE | GND
TX | BLUE | GPIO16
RX | GREEN | GPIO17

# Pinlayout LoRa SX1278 Module ra-02

![image](https://images.tcdn.com.br/img/img_prod/557243/sx1278_lora_433mhz_ra_02_breakout_board_10km_959_1_20191128221303.png)

## Wiring LoRa SX1278 module with ESP32
SX1278 | COLOR | ESP32
-------- |-| ----------
VCC | RED | 3.3V
GND | BLACK | GND
SCK | ORANGE | GPIO18
MOSI | WHITE | GPIO23
DIO0 | BROWN | GPIO4
NSS | BLUE | GPIO5
MISO | GREEN | GPIO19
RST | YELLOW | GPIO2

[Wiring and more](https://www.circuitstate.com/tutorials/interfacing-ra-01-ra-02-sx1278-lora-modules-with-esp32-using-arduino/)
[Sample code](https://how2electronics.com/esp32-lora-sx1278-76-transmitter-receiver/)
