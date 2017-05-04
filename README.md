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

Clone this repository, and build it by simply running:

```bash
git clone https://github.com/dycodex/Simple-LoRa-Gateway
cd Simple-LoRa-Gateway
make
```

If everything works fine, an executable namely `rf95_server` will be created.

### Running the gateway

Simply run
```bash
sudo ./rf95_server
```

to start the gateway.

**Important Note**: The jumper on the board should be at NSS and GPIO 25.

## License

MIT
