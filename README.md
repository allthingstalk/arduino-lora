arduino-client
==============

This is a library for the AllThingsTalk ‘Internet of Things’ platform that provides connectivity to the cloud through [LoRa radios](https://www.lora-alliance.org/What-Is-LoRa/Technology).  

This library has been developed for the folowing hardware:
- [sodaq mbili](http://mbili.sodaq.net/) (other arduino devices should be compatible)
- LoRa modems:
	- [Microchip](http://www.microchip.com/wwwproducts/Devices.aspx?product=RN2483)
	- [Embit](http://www.embit.eu/products/wireless-modules/emb-lr1272/) (partially)
	- other to come shortly

### Installation
  1. Download the [source code](https://github.com/allthingstalk/arduino-lora/archive/master.zip)
  2. copy the content of the zip file to your arduino libraries folder (usually found at &lt;arduinosketchfolder>/libraries).
  
### Example sketches

The library includes an example sketch for each sensor that is currently supported on the proximus-LoRa network. Each example shows you how to work with the sensor.   
Some sensors require a 3th party library. For ease of use, we have included a working version of these libraries in the [download](https://github.com/allthingstalk/arduino-lora/archive/master.zip). The following library versions were used:

* [AirQuality2](https://github.com/MikeHg/AirQualitySensor/tree/d6cadaf21c6beae99fdd65bb037424ce6f855db1)
* [MMA7660 (Accelerometer)](http://www.seeedstudio.com/wiki/Grove_-_3-Axis_Digital_Accelerometer(%C2%B11.5g))
* [TPH2](http://support.sodaq.com/sodaq-one/tph-v2/) Note: this library is not included in the download and must be installed from the ide, follow the previous link for more info)

### Experiments

The download also includes a number of experiments to get you started with more features of  the AllThingsTalk ‘Internet of Things’ platform such as automation and notifications.

Want more information ?  
Check out the [documentation](http://allthingstalk.com/docs/tutorials/lora/setup).

