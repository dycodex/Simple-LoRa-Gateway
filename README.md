Simple LoRa Gateway
===================

Simple LoRa gateway implementation for DycodeX's [LoRa Raspberry Pi Hat](https://github.com/dycodex/LoRa-Raspberry-Pi-Hat).

## Getting Started

### Installing Required Libraries

Make sure you have [wiringPi](http://wiringpi.com) and [bcm2835 library](http://www.airspayce.com/mikem/bcm2835/) installed on your Pi. If you don't install it this way:


**bcm2835**

```bash
wget -c http://www.airspayce.com/mikem/bcm2835/bcm2835-1.52.tar.gz
tar zvxf bcm2835-1.52.tar.gz
./configure
make
sudo make check
sudo make install
```

For detailed instruction, please see [this link](http://www.airspayce.com/mikem/bcm2835/).


**wiringPi**

On most Pi, this library is installed by default. Check the installation by running:

```bash
gpio -v
```

If there's no error, then wiringPi is already installed. You can skip the installation section below.

To install wiringPi, you must have git installed on your system.

Then excute these commands:

```bash
git clone git://git.drogon.net/wiringPi
cd wiringPi
./build
```

Then, check your installation

```bash
gpio -v
```

### Building the source

**Enabling or Disabling MQTT support**

Before building the source, you need to decide whether you want to publish every received data to a MQTT broker or not.

By default, publishing to MQTT is disabled. If you want to enable the feature you need to edit the `src/rf95_server.cpp` file.

You need to find and uncomment the line below:
```c++
//#define MQTT_ENABLED
```

so that it becomes:
```c++
#define MQTT_ENABLED
```


**Building the source**

Clone this repository, and build it by simply running:

```bash
git clone https://github.com/dycodex/Simple-LoRa-Gateway
cd Simple-LoRa-Gateway
make
```

If everything works fine, an executable namely `rf95_server` will be created.

### Running the gateway

If you disable the MQTT publish feature, simply run the command below to start the gateway:
```bash
sudo ./rf95_server
```

If you enable the MQTT publish feature, you need to provide the following information to the environment variable.

* MQTT client ID as `MQTT_ID` environment variable. Note that some server requires this value to be unique among every clients.

* MQTT host as `MQTT_HOST` environment variable.
* MQTT topic as `MQTT_TOPIC` environment varibale. This topic will be used in publish message.
* MQTT username as `MQTT_USER` environment variable. This value is not required if the server disable the authentication mechanism.
* MQTT password as `MQTT_PASS` environment variable. This value is not required if the server disable the authentication mechanism.

You can provide values for these environment variables by editing the `start` file.

Then, start the gateway by executing the following command:

```bash
./start
```

**Important Note**: The jumper on the board should be at NSS and GPIO 25.

## License

MIT
