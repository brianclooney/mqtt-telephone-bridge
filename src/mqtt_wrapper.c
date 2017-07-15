
//#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "MQTTClient.h"

#include "mqtt_wrapper.h"
#include "log.h"

#define ADDRESS     "tcp://automation.home.lan:1883"
//#define CLIENTID    ""
#define QOS         0
#define TIMEOUT     10000L

MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;


void mqtt_connectionLost(void *context, char *cause)
{
    LOGD("MQTT connection lost: %s\n", cause);
}

void mqtt_deliveryComplete(void *context, MQTTClient_deliveryToken dt)
{
    LOGD("MQTT delivery complete: %d\n", dt);
}

int mqtt_messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    LOGD("MQTT message arrived: ??\n");
    return 1;
}

int mqtt_connect(char* uri, char* client_id)
{
    int rc;

    LOGD("mqtt_connect\n");

    MQTTClient_create(&client, uri, client_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        LOGD("Failed to connect to MQTT server %s, return code %d\n", uri, rc);
	return -1;
    }

    rc = MQTTClient_setCallbacks(client,
				 NULL,
				 mqtt_connectionLost,
				 mqtt_messageArrived,
				 mqtt_deliveryComplete);

    LOGD("mqtt_connect out %d\n",rc);

    return 0;
}

int mqtt_disconnect()
{
    LOGD("mqtt_disconnect\n");
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return 0;
}

int mqtt_publish(char* topic, char* payload)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;
    
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    
    rc = MQTTClient_publishMessage(client, topic, &pubmsg, &token);

    if (rc == -3)
    {
        if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
	    LOGD("Failed to connect to MQTT server, return code %d\n", rc);
	    return -1;
	}
        rc = MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    }

    if (rc == MQTTCLIENT_SUCCESS)
    {
      // LOGD("Waiting for up to %d seconds for publication of %s\non topic %s for client with ClientID: %s\n", (int)(TIMEOUT/1000), payload, topic, CLIENTID);
      // rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
      if (rc == MQTTCLIENT_SUCCESS)
      {
	  LOGD("MQTT PUBLISH: [%d] %s %s\n", token, topic, payload);
	  LOGD("Message with delivery token %d delivered\n", token);
      }
      else
      {
          LOGD("ERROR: MQTTClient_waitForCompletion returned %d\n", rc);
      }
    }
    else
    {
        LOGD("ERROR: MQTTClient_publishMessage returned %d\n", rc);
    }

    return 0;
}

int mqtt_subscribe()
{
    return 0;
}
