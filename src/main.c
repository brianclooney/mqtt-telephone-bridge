
//#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>

#include "json-c/json.h"
#include "libwebsockets.h"
#include "mqtt_wrapper.h"
#include "serial_port.h"
#include "log.h"

serial_port_t sp;
int is_running = TRUE;

typedef struct cid_info_d
{
  char date[64];
  char time[64];
  char name[64];
  char number[64];
} cid_info_t;

void json_test()
{
    json_object *new_obj;
    new_obj = json_object_new_object();
    json_object_object_add(new_obj, "abc", json_object_new_int(12));
    json_object_object_add(new_obj, "foo", json_object_new_string("bar"));
    json_object_object_add(new_obj, "bool", json_object_new_boolean(1));
    LOGD("new_obj.to_string() = %s\n", json_object_to_json_string(new_obj));
    json_object_put(new_obj);
}

static void cleanup(int signo)
{
    LOGD("cleanup...\n");
    is_running = FALSE;
    serial_port_close(sp);
    mqtt_disconnect();
}

void send_command(serial_port_t spx, char* cmd)
{
    static char buf[100];
    serial_port_write(spx, cmd);
    serial_port_readln(spx, buf, 100);
    while (strcmp("OK", buf) != 0 && strcmp("ERROR", buf) != 0) {
        serial_port_readln(spx, buf, 100);
    }
}

void init_caller_info(cid_info_t* info)
{
    memset (info, 0, sizeof(cid_info_t));
}

void caller_info_to_string(cid_info_t* info, char* buf)
{
    json_object *new_obj;
    new_obj = json_object_new_object();

    if (strlen(info->name) > 0)
    {
        json_object_object_add(new_obj, "name", json_object_new_string(info->name));
    }

    if (strlen(info->number) > 0)
    {
        json_object_object_add(new_obj, "number", json_object_new_string(info->number));
    }

    sprintf(buf, "%s", json_object_to_json_string(new_obj));

    json_object_put(new_obj);
}

int main(int argc, char *argv[]) {

    int i;
    static char buf[100];
    static char cid_payload[400];
    static cid_info_t cid;
    int cid_active = 0;
    
    signal(SIGINT, cleanup);
    //signal(SIGKILL, cleanup);
    
    char* uri = getenv("MQTT_URI");
    mqtt_connect(uri, "tele-test");

    if (serial_port_open("/dev/ttyACM0", &sp) < 0)
    {
        LOGD("Error opening serial port\n");
	return -1;
    }

    send_command(sp, "ATZ");
    send_command(sp, "ATE0");
    send_command(sp, "ATI3");
    send_command(sp, "AT+FCLASS=8");
    send_command(sp, "AT+VIP");
    send_command(sp, "AT+VGT=120");
    send_command(sp, "AT+VCID=1");
    send_command(sp, "AT+FCLASS=0");
    send_command(sp, "AT#CID=1");

    init_caller_info(&cid);

    while (is_running) {

        serial_port_readln(sp, buf, 100);

	if (strcmp("RING", buf) == 0)
	{

	    if (cid_active == 1) {
	        caller_info_to_string(&cid, cid_payload);
	        mqtt_publish("home/status/telephone/incoming/caller-id", cid_payload);
	        init_caller_info(&cid);
	        cid_active = 0;
	    }
	  
	    mqtt_publish("home/status/telephone/incoming/ring", "RING");
	}
	else if (strncmp("NMBR=", buf, 5) == 0)
	{
	    strcpy(cid.number, &buf[5]);
	    cid_active = 1;
	}
	else if (strncmp("NAME=", buf, 5) == 0)
	{
	    strcpy(cid.name, &buf[5]);
	    cid_active = 1;
	}

    }

    LOGD("exiting...\n");
    
    return 0;    
}
