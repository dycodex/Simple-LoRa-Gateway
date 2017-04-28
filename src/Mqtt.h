#ifndef __MQTT_H__
#define __MQTT_H__

#include <mosquittopp.h>
#include <string>
#include <cstdlib>

class Mqtt : public mosqpp::mosquittopp {
private:
  int reconnect_max_times;
  int reconnect_times;
public:
  Mqtt(const char* id, const char* host, int port, const char* username = NULL, const char* password = NULL);
  ~Mqtt();
  bool send_message(const char* topic, std::string message);

  void on_connect(int rc);
  void on_disconnect(int rc);
  void on_publish(int mid);
};

#endif