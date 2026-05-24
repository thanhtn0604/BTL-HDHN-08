#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

int mqtt_thread_init(void);

void mqtt_publish_status(float temp, const char* mode, const char* pump_status);

#endif
