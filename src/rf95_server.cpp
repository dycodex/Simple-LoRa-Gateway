#include <bcm2835.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <string>
#include <iostream>

#define BOARD_DYCODEX
#include "RasPiBoards.h"

#define RF_FREQUENCY  868.10
#define RF_NODE_ID    1
#define RFMANAGER_TIMEOUT 3000

// By default, every data that comes to the gateway will not be
// relayed to a MQTT broker with defined topic. You need to set
// the MQTT host, client ID, username and password if necessary
// and most importantly the topic that will be used.
// 
// If you want to enable MQTT, uncomment this line below
// #define MQTT_ENABLED

#ifdef MQTT_ENABLED
#include "Mqtt.h"
#endif

RH_RF95 rf95(RF_CS_PIN, RF_IRQ_PIN);
RHReliableDatagram rfManager(rf95, RF_NODE_ID);

volatile sig_atomic_t force_exit = false;

void sig_handler(int sig) {
    std::cout << "Break Received, exiting!\n";
    force_exit = true;
}

bool initRF() {
    if (!bcm2835_init()) {
        std::cerr << "bcm2835_init() Failed!\n";
        return false;
    }

    std::cout << "RF95 CS=GPIO" << RF_CS_PIN;
    pinMode(RF_LED_PIN, OUTPUT);
    digitalWrite(RF_LED_PIN, HIGH);

    std::cout << ", IRQ=GPIO" << RF_IRQ_PIN;
    pinMode(RF_IRQ_PIN, INPUT);
    bcm2835_gpio_set_pud(RF_IRQ_PIN, BCM2835_GPIO_PUD_DOWN);
    bcm2835_gpio_ren(RF_IRQ_PIN);

    std::cout << ", RST=GPIO" << RF_RST_PIN;
    pinMode(RF_RST_PIN, OUTPUT);
    digitalWrite(RF_RST_PIN, LOW);
    bcm2835_delay(150);
    digitalWrite(RF_RST_PIN, HIGH);
    bcm2835_delay(100);

    std::cout << ", LED=GPIO" << RF_LED_PIN << std::endl;
    digitalWrite(RF_LED_PIN, LOW);

    rfManager.setTimeout(RFMANAGER_TIMEOUT);
    rfManager.setRetries(0);

    if (!rfManager.init()) {
        std::cerr << "RF95 Module init failed, please verify wiring!\n";
        return false;
    }

    // This configuration must be implemented on the node as well.
    RH_RF95::ModemConfig modemConfig = {
        0x78,
        0xC4,
        0x0C
    };
    rf95.setModemRegisters(&modemConfig);
    rf95.setTxPower(23, false);
    rf95.setFrequency(RF_FREQUENCY);
    rf95.setThisAddress(RF_NODE_ID);
    rf95.setHeaderFrom(RF_NODE_ID);
    rf95.setPromiscuous(true);
    rf95.setModeRx();

    std::cout << "OK NodeID=" << RF_NODE_ID << " @ " << RF_FREQUENCY << "Mhz\n";
    std::cout << "Listening packet....\n\n";

    return true;
}

int main(int argc, char** argv) {
    unsigned long led_blink = 0;
    signal(SIGINT, sig_handler);

#ifdef MQTT_ENABLED
    // MQTT Client ID. Some server require this value to be unique among all clients
    const char* id = std::getenv("MQTT_ID");
    if (id == NULL) {
        std::cerr << "MQTT_ID must be given!\n";
        return -1; 
    }

    std::cout << "MQTT_ID: " << id << std::endl;

    // MQTT Broker address.
    const char* host = std::getenv("MQTT_HOST");
    if (host == NULL) {
        std::cerr << "MQTT_HOST must be given!\n";
        return -1;
    }
    
    std::cout << "MQTT_HOST: " << host << std::endl;

    // MQTT Username (if required)
    const char* username = std::getenv("MQTT_USER");
    if (username != NULL) {
        std::cout << "MQTT_USER: " << username << std::endl; 
    }

    // MQTT Password (if required)
    const char* password = std::getenv("MQTT_PASS");
    if (password != NULL) {
        std::cout << "MQTT_PASS: " << password << std::endl;
    }

    // MQTT topic for publishing data
    const char* topic = std::getenv("MQTT_TOPIC");
    if (topic == NULL) {
        std::cerr << "MQTT_TOPIC must be given!\n";
        return -1;
    }

    std::cout << "MQTT_TOPIC: " << topic << std::endl;

    Mqtt* mqtt = new Mqtt(id, host, 1883, username, password);
#endif

    if (!initRF()) {
        return -1;
    }

    while(!force_exit) {
        if (bcm2835_gpio_eds(RF_IRQ_PIN)) {
            bcm2835_gpio_set_eds(RF_IRQ_PIN);

            if (rfManager.available()) {
                led_blink = millis();
                digitalWrite(RF_LED_PIN, HIGH);

                uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
                uint8_t len = sizeof(buf);
                uint8_t from;
                uint8_t to;
                uint8_t id;
                uint8_t flags;
                int8_t rssi = rf95.lastRssi();

                if (rfManager.recvfromAckTimeout(buf, &len, 1000, &from, NULL, &id, &flags)) {
                    std::cout << "Packet[" << (int)len << "] *" << (int)id << " #" << (int)from << " " << (int)rssi << "dB:\n";
                    std::cout << (char*)buf << std::endl;
#ifdef MQTT_ENABLED
                    mqtt->send_message(topic, std::string((char*)buf));
#endif
                } else {
                    std::cout << "receive failed\n";
                }
            }
        }

        if (led_blink && millis() - led_blink > 200) {
            led_blink = 0;
            digitalWrite(RF_LED_PIN, LOW);
        }

        bcm2835_delay(5);
    }

    digitalWrite(RF_LED_PIN, LOW);
    std::cout << "Exiting....\n";
    
    return 0;
}
