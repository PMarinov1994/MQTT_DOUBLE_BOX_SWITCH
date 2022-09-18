# esp8266_mqttSwitch
A firmware for the esp8266 that can switch stuff on and off via MQTT

## TODO
- Remove Print Lines from final build.
- Report error code with build in LED by flashing or something.

## Visual Micro settings
- Builtin Led: 2
- Upload Speed: 115200
- CPU Frequency: 80MHz
- Crystal Frequency: 26MHz
- Flash Size: 512KB (FS: 32KB) [Can be wilth 1Mb as well]
- Flash Mode: DIO
- Flash Frequency: 40MHz
- Reset Method: no dtr (aka ck)
- Debug port: Disabled
- Debug Level: None
- iwIP Variant: v2 Lower Memory
- VTables: Flash
- Exceptions: Legacy (new can return nullptr)
- Erase Flash: Only Sketch
- Espressif FW: nonos-sdk 2.2.1+100 (190703) [Not important]