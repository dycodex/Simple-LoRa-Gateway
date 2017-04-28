#include <string>
#include <iostream>
#include "Mqtt.h"

Mqtt::Mqtt(const char* id, const char* host, int port, const char* username, const char* password) : mosquittopp(id) {
  mosqpp::lib_init();

  std::cout << ">> Mqtt - Initializing..." << std::endl;

  if (username != NULL && password != NULL) {
    username_pw_set(username, password);
  }

  reconnect_times = 0;
  reconnect_max_times = 15;

  connect_async(host, port, 120);
  reconnect_delay_set(3, 5, true);
  loop_start();
}

Mqtt::~Mqtt() {
  loop_stop();
  mosqpp::lib_cleanup();
}

bool Mqtt::send_message(const char* topic, std::string message) {
  int ret = publish(NULL, topic, message.length(), message.c_str(), 0, false);

  return ret == MOSQ_ERR_SUCCESS;
}

void Mqtt::on_connect(int rc) {
  if (rc == 0) {
    std::cout << ">> Mqtt - connected with server" << std::endl;
  } else {
    std::cout << ">> Mqtt - Impossible to connect with server(" << rc << ")" << std::endl;
  }
}

void Mqtt::on_publish(int mid) {
  std::cout << ">> Mqtt - Message (" << mid << ") succeed to be published " << std::endl;
}

void Mqtt::on_disconnect(int rc) {
  std::cout << ">> Mqtt - disconnection(" << rc << ")" << std::endl;

  reconnect_times += 1;

  if (reconnect_times == reconnect_max_times) {
    throw std::exception();
  }

  std::cout << "Reconnecting..." << std::endl;

  reconnect_async();
}
