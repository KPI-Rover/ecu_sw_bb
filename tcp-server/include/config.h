#ifndef CONFIG_H
#define CONFIG_H

/* Server configurations START */
#define NUMSLOTS 5 // how many connections can server process
#define BUFFERSIZE 64 // maximum size of buffer
#define TIMERPRECISION 100000 // 100 miliseconds in microsecond (for timer) 
#define TIMESTOP 1 // 1 second befre stopping all motors. If no new command is received over TCP for 1 second, all motors must stop.
/* Server configurations END */

/* robot control configurations START */
#define ID_GET_API_VERSION 1
#define ID_SET_MOTOR_SPEED 2
#define ID_SET_ALL_MOTORS_SPEED 3
#define ID_GET_ENCODER 4
#define ID_GET_ALL_ENCODERS 5
/* robot control configuraitons END */

/*  */

/*  */


#endif