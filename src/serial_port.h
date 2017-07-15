
#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

typedef struct serial_port_d
{
    int fd;
} serial_port_t;

int serial_port_open(char* port, serial_port_t* sp);
int serial_port_close(serial_port_t sp);
int serial_port_write(serial_port_t sp, char* buf);
int serial_port_readln(serial_port_t sp, char* buf, int len);

#endif /* __SERIAL_PORT_H__ */
