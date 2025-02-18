#ifndef CONFIG_H
#define CONFIG_H


/* START of Including of main libs */
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <resolv.h>
#include <pthread.h>
#include <csignal>
#include <getopt.h>

#include <robotcontrol.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <semaphore.h>
/* END of including of main libs */

using namespace std;




/* Server configurations START */
#define NUMSLOTS 5 // how many connections can server process
#define BUFFERSIZE 64 // maximum size of buffer
#define TIMERPRECISION 100000 // 100 miliseconds in microsecond (for timer) 
#define TIMESTOP 1 // 1 second befre stopping all motors. If no new command is received over TCP for 1 second, all motors must stop.
/* Server configurations END */

/* robot control ccommands START */
#define ID_GET_API_VERSION 0x01
#define ID_SET_MOTOR_SPEED 0x02
#define ID_SET_ALL_MOTORS_SPEED 0x03
#define ID_GET_ENCODER 0x04
#define ID_GET_ALL_ENCODERS 0x05

/* robot control commands END */

/* robot control configurations START */
#define MOTOR_ID_START 0 // first motor designation
#define MOTORS_NUMBER 4
const int SHASSIARR[MOTORS_NUMBER] = {1, 2, 3, 4}; // don't change
#define MOTOR_INVERTED 3 // don't change
#define BRAKE_TIME 100000 // 100 ms 
#define MIN_RPM 80
#define MAX_RPM 530

/* robot control configurations END */


#endif