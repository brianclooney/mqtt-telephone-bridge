
#ifndef __MQTT_WRAPPER_H__
#define __MQTT_WRAPPER_H__

int mqtt_connect(char* uri, char* client_id);
int mqtt_disconnect();
int mqtt_publish(char* topic, char* payload);
int mqtt_subscribe();


#endif /* __MQTT_WRAPPER_H__ */
