
#include <stdio.h>
#include <stdarg.h>
//#include <unistd.h>

#include "json-c/json.h"

#include "log.h"

static char buffer[1024];

void log_debug(int dbg_lvl, char* file, int line, char* fmt, ...)
{
    json_object *new_obj;
    json_object *file_obj;
    json_object *app_obj;
    va_list args;
    int ret;  

    va_start(args, fmt);
    ret = vsprintf(buffer, fmt, args);
    va_end(args);

    //file_obj = json_object_new_object();
    //json_object_object_add(file_obj, "name", json_object_new_string(file));
    //json_object_object_add(file_obj, "line", json_object_new_int(line));

    //app_obj = json_object_new_object();
    //json_object_object_add(app_obj, "name", json_object_new_string("telephone-app"));
    //json_object_object_add(app_obj, "version", json_object_new_string("1.0.0"));

    new_obj = json_object_new_object();
    json_object_object_add(new_obj, "log_level", json_object_new_string("DEBUG"));
    json_object_object_add(new_obj, "message", json_object_new_string(buffer));
    //json_object_object_add(new_obj, "file", file_obj);
    //json_object_object_add(new_obj, "file", file_obj);
    //json_object_object_add(new_obj, "app", app_obj);

    printf("%s\n", json_object_to_json_string(new_obj));

    json_object_put(new_obj);
    //json_object_put(app_obj);
    //json_object_put(file_obj);

    fflush(stdout);
    usleep(2000);
}
