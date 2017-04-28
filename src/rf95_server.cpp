#include <bcm2835.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <cstdlib>
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <string>
#include <iostream>
#include "Mqtt.h"

#define BOARD_DYCODEX
#include "RasPiBoards.h"

#define RF_FREQUENCY  868.10
#define RF_NODE_ID    1
#define RFMANAGER_TIMEOUT 3000

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
        return 1;
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
    std::cout << ", LED=GPIO" << RF_LED_PIN;
    digitalWrite(RF_LED_PIN, LOW);

    rfManager.setTimeout(RFMANAGER_TIMEOUT);
    rfManager.setRetries(0);

    if (!rfManager.init()) {
        std::cerr << "RF95 Module init failed, please verify wiring!\n";
        return false;
    }

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

    if (!initRF()) {
        return -1;
    }

    const char* id = std::getenv("MQTT_ID");
    const char* host = std::getenv("MQTT_HOST");
    const char* username = std::getenv("MQTT_USER");
    const char* password = std::getenv("MQTT_PASS");
    const char* topic = std::getenv("MQTT_TOPIC");

    Mqtt* mqtt = new Mqtt(id, host, 1883, username, password);

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

                if (rfManager.recvfromAckTimeout(buf, &len, RFMANAGER_TIMEOUT, &from, NULL, &id, &flags)) {
                    std::cout << "Packet[" << len << "] *" << id << " #" << from << " " << rssi << "dB:\n";
                    std::cout << (char*)buf << std::endl;
                } else {
                    std::cout << "receive failed\n";
                }
            }

            if (led_blink && millis() - led_blink > 200) {
                led_blink = 0;
                digitalWrite(RF_LED_PIN, LOW);
            }

            bcm2835_delay(5);
        }
    }

    digitalWrite(RF_LED_PIN, LOW);
    std::cout << "Exiting....\n";
    
    return 0;
}
