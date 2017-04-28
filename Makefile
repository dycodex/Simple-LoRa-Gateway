CC = g++
CFLAGS = -DRASPBERRY_PI -DBCM2835_NO_DELAY_COMPATIBILITY -D__BASEFILE__=\"$*\" -std=c++11
LIBS = -lbcm2835 -lmosquittopp
RADIOHEADBASE = ./deps/RadioHead
INCLUDE = -I$(RADIOHEADBASE)
INSTALLATION_PREFIX = /usr/local/bin

all: rf95_server

RasPi.o: $(RADIOHEADBASE)/RHutil/RasPi.cpp
	$(CC) $(CFLAGS) -c $(RADIOHEADBASE)/RHutil/RasPi.cpp $(INCLUDE)

rf95_server.o: src/rf95_server.cpp
	$(CC) $(CFLAGS) -c $(INCLUDE) $<

RH_RF95.o: $(RADIOHEADBASE)/RH_RF95.cpp
	$(CC) $(CFLAGS) -c $(INCLUDE) $<

RHHardwareSPI.o: $(RADIOHEADBASE)/RHHardwareSPI.cpp
	$(CC) $(CFLAGS) -c $(INCLUDE) $<

RHSPIDriver.o: $(RADIOHEADBASE)/RHSPIDriver.cpp
	$(CC) $(CFLAGS) -c $(INCLUDE) $<

RHGenericDriver.o: $(RADIOHEADBASE)/RHGenericDriver.cpp
	$(CC) $(CFLAGS) -c $(INCLUDE) $<

RHGenericSPI.o: $(RADIOHEADBASE)/RHGenericSPI.cpp
	$(CC) $(CFLAGS) -c $(INCLUDE) $<

RHDatagram.o: $(RADIOHEADBASE)/RHDatagram.cpp
	$(CC) $(CFLAGS) -c $(INCLUDE) $<

RHReliableDatagram.o: $(RADIOHEADBASE)/RHReliableDatagram.cpp
	$(CC) $(CFLAGS) -c $(INCLUDE) $<

Mqtt.o: src/Mqtt.cpp
	$(CC) $(CFLAGS) -c $(INCLUDE) $<

rf95_server: rf95_server.o RH_RF95.o RasPi.o RHHardwareSPI.o RHGenericDriver.o RHGenericSPI.o RHSPIDriver.o RHDatagram.o RHReliableDatagram.o Mqtt.o
	$(CC) $^ $(LIBS) -o rf95_server

clean:
	rm -rf *.o rf95_server

install: rf95_server
	cp rf95_server $(INSTALLATION_PREFIX)/rf95_server

install_systemd: install
	cp systemd/* -r /etc/systemd/system/

uninstall:
	rm $(INSTALLATION_PREFIX)/rf95_server

uninstall_systemd:
	systemctl stop rf95_server.service
	systemctl mask rf95_server.service
