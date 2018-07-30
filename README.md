# VESC-ESP8266-MQTT

This is a ESP8266 project used to read data from a VESC and publish it to a 
MQTT server. Tested on a ESP8266 mini D1 and VESC hw 4.12, fw 2.18.

## Usage
Usage is straightforward without the need for code changes.  
Everything that changes between scenarios (with a few exceptions) is configured 
as compile-time properties, such as:
- node id
- wifi access
- mqtt access

See all available properties in `build_flags` section inside 
`platform.ini` file.

### Install dependencies
    virtualenv pyenv && \
    pyenv/bin/pip install platformio

### Compile firmware for sensor nodes
    PLATFORMIO_BUILD_FLAGS='-DNODE_ID=\"my_board1\" -DWIFI_SSID=\"hotspot\" \
    -DWIFI_PASSWORD=\"securepasswd\" -DMQTT_SERVER=\"my.mqttserver.net\" \
    -DMQTT_PORT=1883' pyenv/bin/platformio run

### Upload firmware to a sensor node
    PLATFORMIO_BUILD_FLAGS='-DNODE_ID=\"my_board1\" -DWIFI_SSID=\"hotspot\" \
    -DWIFI_PASSWORD=\"securepasswd\" -DMQTT_SERVER=\"my.mqttserver.net\" \
    -DMQTT_PORT=1883' pyenv/bin/platformio run -t upload