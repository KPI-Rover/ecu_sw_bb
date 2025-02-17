#include "TCPServerReceiver.h"
#include <getopt.h>
#include "motorsProcessor.h"
#include "config.h"
using namespace std;

//#define BUFFERSIZE 1024
//#define NUMSLOTS 5

sem_t stopProgramSem;

void interruptSignalHandler(int signal) {
	cout << "get sighandelererere" << endl;
	sem_post(&stopProgramSem);
	//exit(0);
	
}

int main(int argc, char* argv[]) {
	char *server_address = get_primary_ip();
	int server_portnum = 5500;

	// Command-line options
	int opt;
	while ((opt = getopt(argc, argv, "a:p:")) != -1) {
		switch (opt) {
		    case 'a':
			server_address = optarg;
			break;
		    case 'p':
			server_portnum = atoi(optarg);
			break;
		    default:
			cerr << "Usage: " << argv[0] << endl;
		       	cerr << " [-a server_address] " << endl; 
			cerr << " [-p server_portnum]" << endl;
			return EXIT_FAILURE;
		}
	}
	sem_init(&stopProgramSem, 0, 0);
	

	MotorProcessor motors_processor;
	TCPServer server(server_address, server_portnum, &motors_processor, &stopProgramSem);

	server.init();
	server.start();

	signal(SIGINT, interruptSignalHandler);
	//while (0) {}
	//while (!server.stopThread) {}
	sem_wait(&stopProgramSem);
	cout << "start destroying" << endl;

	server.destroy();
	
	
	return 0;
}
