#ifndef CONFIG_H
#define CONFIG_H

using namespace std;
/* START of Including of main libs */
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <resolv.h>
#include <pthread.h>
#include <csignal>

#include <robotcontrol.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netdb.h>
/* END of including of main libs */

/* Server configurations START */
#define NUMSLOTS 5 // how many connections can server process
#define BUFFERSIZE 64 // maximum size of buffer
#define TIMERPRECISION 100000 // 100 miliseconds in microsecond (for timer) 
#define TIMESTOP 1 // 1 second befre stopping all motors. If no new command is received over TCP for 1 second, all motors must stop.
/* Server configurations END */

/* robot control ccommands START */
#define ID_GET_API_VERSION 1
#define ID_SET_MOTOR_SPEED 2
#define ID_SET_ALL_MOTORS_SPEED 3
#define ID_GET_ENCODER 4
#define ID_GET_ALL_ENCODERS 5
#define MOTOR_ID_START 0 // first motor designation
/* robot control commands END */

/* robot control configurations START */

#define MOTORS_NUMBER 4
const int SHASSIARR[MOTORS_NUMBER] = {1, 2, 3, 4}; // don't change
#define MOTOR_INVERTED 3 // don't change
#define BRAKE_TIME 100000 // 100 ms 
#define MIN_RPM 80
#define MAX_RPM 530

/* robot control configurations END */


#endif