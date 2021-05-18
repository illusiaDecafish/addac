#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

const char  lfchar  = 0x0A;
const char  crchar  = 0x0D;
const char  nlchar  = 0x0D;

//static char *usbtty =   "/dev/tty.URT1";
//static char *usbtty = "/dev/tty.usbmodem0000000000001";
static char *usbtty = "/dev/cu.usbmodem0000000000001";
static char pwmc    = 'w';
static char adcc    = 'a';
static char startc  = 's';
static char stopc   = 'e';
static char applc   = 'p';


static char *defaultADCSpeed = "ac000";

bool    writeString(int fd, char *s);
bool    readString(int fd, char *s, size_t len);
bool    isNewLine(char c);


int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "specify output file path\n");
        return -1;
    }
    FILE    *output = fopen(argv[argc - 1], "w");
    if (output == NULL) {
        fprintf(stderr, "can not open file %s\n", argv[argc - 1]);
        return -1;
    }
    int     usbfd = open(usbtty, O_RDWR | O_NONBLOCK | O_NOCTTY | O_SYNC);
    if (usbfd < 0) {
        fprintf(stderr, "canot open device file %s\t", usbtty);
        perror("");
        return 0;
    }
    const size_t    buflen = 64;
    char    buf[buflen];
    char    usb[buflen];
    fprintf(stderr, "start\n");
    sprintf(usb, "%c\n", applc);
    if (! writeString(usbfd, usb)) {
        fprintf(stderr, "can not write string:%s\n", usb);
    }

    writeString(usbfd, defaultADCSpeed);
    sprintf(usb, "%c\n", startc);
    writeString(usbfd, usb);

    for (int i = 0 ; i < 0x0FFF ; i ++) {
        sprintf(usb, "%c%x", pwmc, i);
        writeString(usbfd, usb);
        readString(usbfd, buf, buflen);
        readString(usbfd, buf, buflen);
        fprintf(output, "%03X\t%s\n", i, buf);
        printf("%03X\t%s\r", i, buf);
        fflush(stdout);
        readString(usbfd, buf, buflen);
    }
   
    
    close(usbfd);
    fclose(output);
    printf("\n\n");
    return 0;
}

bool    writeString(int fd, char *s)
{
    char    term = nlchar;
    ssize_t len = strlen(s);
    ssize_t alr;
    while (len > 0) {
        alr = write(fd, s, len);
        if (alr < 0)
            return false;
        len -= alr;
        s += alr;
    }
    if (write(fd, &term, 1) > 0)
        return true;
    return false;
}


bool    readString(int fd, char *s, size_t len)
{
    ssize_t alr;
    char    c;
    while ((alr = read(fd, &c, 1)) < 0)
        usleep(1);
    *(s ++) = c;
    len --;
    while ((alr = read(fd, s, 1)) > 0) {
        if (-- len == 0)
            return false;
        if (isNewLine(*s)) {
            *s = '\0';
            break;
        }
        s ++;
    }
    return true;
}


bool    isNewLine(char c)
{
    return (c == lfchar) || (c == crchar);
}

/*bool    readString0(int fd, char *s, size_t len)
{
    ssize_t alr;
    while ((alr = read(fd, s, len)) > 0) {
        
    }
        return false;
    s += alr;
    len -= alr;
    while (*(s - 1) != nlchar) {
        if ((alr = read(fd, s, len)) < 0)
            return false;
        s += alr;
        len -= alr;
    }
    *(s - 1) = '\0';
    return true;
}
*/
