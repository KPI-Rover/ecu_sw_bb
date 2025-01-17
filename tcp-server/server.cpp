#include "TCPServerReceiver.h"

using namespace std;

//#define BUFFERSIZE 1024
//#define NUMSLOTS 5

int main() {
	char *server_address = "192.168.1.14";
	int server_portnum = 5500;

	TCPServer server("192.168.1.14", 5500);

	server.init();
	server.start();

	while (!server.stopThread) {}

	server.destroy();
	
	
	return 0;
}
