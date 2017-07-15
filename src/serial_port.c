
//#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

#include "serial_port.h"
#include "log.h"

// modem_open
int serial_port_open(char* device, serial_port_t* sp)
{
    struct termios tty;
    // int fd = open(device, O_RDWR | O_NOCTTY | O_SYNC);
    int fd = open(device, O_RDWR | O_NOCTTY);
    int n;
    int len;
    
    if (fd < 0)
    {
        LOGD("error %d opening %s: %s", errno, device, strerror (errno));
	sp->fd = 0;
        return -1;
    }
    fcntl(fd, F_SETFL, 0);
    
    memset (&tty, 0, sizeof tty);

    /* Error Handling */
    if (tcgetattr(fd, &tty) != 0)
    {
        LOGD("Error configuring serial port: [%d] %s", errno, strerror(errno));
    }

    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;        // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;
    tty.c_cflag     |=  CRTSCTS;
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    //tty.c_iflag     |= IGNPAR | IGNCR;
    //tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);  // turn off s/w flow ctrl

    //tty.c_lflag     |=   ICANON;
    tty.c_lflag       &= ~(ICANON | ECHO |ECHOE | ISIG);
    
    tty.c_oflag     &=  ~OPOST;              // make raw

    tty.c_cc[VMIN]      =   80;                  // read doesn't block
    tty.c_cc[VTIME]     =   1;                  // 0.5 seconds read timeout

    /* Flush Port, then applies attributes */
    // tcflush(fd, TCIFLUSH);

    /* Set Baud Rate */
    cfsetospeed(&tty, B1200);
    cfsetispeed(&tty, B1200);

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        LOGD("Error configuring serial port: x[%d] %s", errno, strerror(errno));
    }

    sp->fd = fd;
    return 0;
}

// modem_close
int serial_port_close(serial_port_t sp)
{
    close(sp.fd);
    return 0;
}

// modem_send_command
int serial_port_write(serial_port_t sp, char* buf)
{
    static char tmp_buf[100];
    int len = strlen(buf);
    
    //LOGD("serial_write 1");

    memset(tmp_buf, 0, 100);
    strcpy(tmp_buf, buf);
    tmp_buf[len] = '\r';

    int tlen = strlen(tmp_buf);

    LOGD("MODEM_WRITE: %s", buf);
    int n = write(sp.fd, tmp_buf, tlen);

    //LOGD("serial_write 2: %d", n);

    //write(fd, "AT\n", 3);
    usleep(200000);

    // LOGD("serial_write 3");

    return 0;
}


int serial_waitfor(serial_port_t sp, char** strs, int timeout)
{
}
  

static int serial_read(serial_port_t sp, char* buf, int len)
{
    char tmp = '\0';
    int i, n;

    //LOGD("serial_read");
    
    i = 0;
    do {
        n = read(sp.fd, &tmp, 1);
	if (n >= 0) {
	  // LOGD("READ: (i=%d,n=%d) %c  0x%X ", i, n, tmp, tmp);
	}
	else {
	  LOGD("ERROR: %d %s", errno, strerror(errno));
	}
	if (tmp != '\n') {
	    buf[i] = tmp;
	    i++;
	}
    } while (tmp != '\n' && n > 0 && i < (len-1));
    
    buf[i] = '\0';
  
    return i;
}

int serial_port_readln(serial_port_t sp, char* buf, int len)
{
    int n = 0;

    //while (n == 0) {
        n = serial_read(sp, buf, len);
    //}

    LOGD("MODEM_READ %s", buf);

    return n;
}


