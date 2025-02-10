#include "TCPServerReceiver.h"
#include <getopt.h>

using namespace std;

//#define BUFFERSIZE 1024
//#define NUMSLOTS 5

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
	TCPServer server(server_address, server_portnum);

	server.init();
	server.start();

	while (!server.stopThread) {}

	server.destroy();
	
	
	return 0;
}
