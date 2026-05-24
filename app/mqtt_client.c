#include "mqtt_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <mosquitto.h>
#include "uart.h"
#include "led.h"
#include <unistd.h>

typedef enum { MODE_AUTO = 0, MODE_MANUAL = 1 } sys_mode_t;
typedef struct {
    sys_mode_t current_mode;
    int pump_status;
    float last_temperature;
    pthread_mutex_t lock;
} system_state_t;

extern system_state_t sys_state;

#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT   1883
#define TOPIC_PUB_TEMP   "thanh/pump/temperature"
#define TOPIC_PUB_STATUS "thanh/pump/status"
#define TOPIC_PUB_MODE   "thanh/pump/mode"
#define TOPIC_SUB_CTRL   "thanh/pump/control"

struct mosquitto *mosq = NULL;

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    char payload[256];
    snprintf(payload, sizeof(payload), "%s", (char *)msg->payload);
    
    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg), "Nhan lenh tu mang: %s", payload);
    uart_send_log("MQTT_RX", log_msg);

    pthread_mutex_lock(&sys_state.lock);
    
    if (sys_state.current_mode == MODE_MANUAL) {
        if (strncmp(payload, "ON", 2) == 0) {
            sys_state.pump_status = 1;
            led_on();
            uart_send_log("MQTT_CMD", "Đa BAT may bom tu xa!");
            printf("[MQTT] Đã BAT may bom tu xa!\n");
        } 
        else if (strncmp(payload, "OFF", 3) == 0) {
            sys_state.pump_status = 0;
            led_off();
            uart_send_log("MQTT_CMD", "Đa TAT may bom tu xa!");
            printf("[MQTT] Đã TAT may bom tu xa!\n");
        }
    } else {
        uart_send_log("MQTT_REJECT", "Tu choi lenh mang vi dang o che do AUTO");
        printf("[MQTT] Tu choi lenh vi he thong dang o che do AUTO!\n");
    }
    
    pthread_mutex_unlock(&sys_state.lock);
}

void* mqtt_network_thread(void* arg) {
    mosquitto_lib_init();
    mosq = mosquitto_new("Thanh_BBB_Client", true, NULL);
    
    if (!mosq) {
        printf("Loi khoi tao Mosquitto Client\n");
        return NULL;
    }

    mosquitto_message_callback_set(mosq, on_message);

    printf("Dang ket noi toi HiveMQ Broker...\n");
    while (mosquitto_connect(mosq, MQTT_BROKER, MQTT_PORT, 60) != MOSQ_ERR_SUCCESS) {
        uart_send_log("MQTT_WAIT", "Mang chua san sang, dang thu lai sau 3 giay...");
        printf("Mang chua san sang, đang thu lai sau 3 giay...\n");
        sleep(3); 
    }

    mosquitto_subscribe(mosq, NULL, TOPIC_SUB_CTRL, 0);

    mosquitto_subscribe(mosq, NULL, TOPIC_SUB_CTRL, 0);
    uart_send_log("MQTT", "Da ket noi Broker thanh cong!");

    mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return NULL;
}

int mqtt_thread_init(void) {
    pthread_t net_thread;
    if (pthread_create(&net_thread, NULL, mqtt_network_thread, NULL) != 0) {
        return -1;
    }
    return 0; 
}

void mqtt_publish_status(float temp, const char* mode, const char* pump_status) {
    if (!mosq) return;
    
    char temp_str[16];
    snprintf(temp_str, sizeof(temp_str), "%.2f", temp);
    
    mosquitto_publish(mosq, NULL, TOPIC_PUB_TEMP, strlen(temp_str), temp_str, 0, false);
    mosquitto_publish(mosq, NULL, TOPIC_PUB_MODE, strlen(mode), mode, 0, false);
    mosquitto_publish(mosq, NULL, TOPIC_PUB_STATUS, strlen(pump_status), pump_status, 0, false);
}
