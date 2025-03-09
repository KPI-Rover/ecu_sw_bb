#include "TCPServerReceiver.h"
#include "config.h"
#include "motorsController.h"
#include "protocolHandler.h"
using namespace std;


sem_t stopProgramSem; /// global value of semaphor that is ckecked to stop rpogrma

void interruptSignalHandler(int signal); 

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
    

    MotorController motors_processor;
    motors_processor.init(SHASSIARR);
    ProtocolHanlder protocolHandler_(&motors_processor);
    TCPServer server(server_address, server_portnum, &protocolHandler_, &stopProgramSem);

    if (server.init()  == -1) {
        cout << "[ERROR] Error creating socket" << endl;
        server.destroy();
        return 1;
    }

    if (server.start() == -1) {
        cout << "[ERROR] Error strting of receiving messages" << endl;
        server.destroy();
        return 1;
    }

    signal(SIGINT, interruptSignalHandler); // initializing of custom signal handlers
    signal(SIGTERM, interruptSignalHandler); // initializing of custom signal handlers

    sem_wait(&stopProgramSem);
    cout << "Stopping ..." << endl;

    server.destroy();
    motors_processor.destroy();
    
    return 0;
}

void interruptSignalHandler(int signal) {
    cout << "Get signal to stop program" << endl;
    sem_post(&stopProgramSem);
    //exit(0);
    
}
