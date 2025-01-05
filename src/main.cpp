#include "SerialReceiver.h"
#include "CommandProcessor.h"
#include <robotcontrol.h>

#define ECU_VERSION "(Version: 1.0.0)"
#define ECU_BAUDRATE B115200

int main(int argc, char *argv[]) {
    std::cout << "Motor controller based on BeagleBone Blue. " << ECU_VERSION << std::endl;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <serial_port>" << std::endl;
        return EXIT_FAILURE;
    }

    if (rc_kill_existing_process(2.0) < -2) {
        perror("ERROR: Not able to kill existing process of robocontrol library: ");
        return EXIT_FAILURE; 
    }
    
    if (rc_enable_signal_handler() == -1) {
        perror("ERROR: failed to start signal handler ");
        return EXIT_FAILURE;
    }
    rc_make_pid_file();
    rc_motor_init();

    const char *serialPort = argv[1];

    CommandProcessor commandProcessor;
    SerialReceiver sReceiver(commandProcessor);
    sReceiver.init(serialPort, ECU_BAUDRATE);
    sReceiver.start();

    rc_set_state(RUNNING);
    while (rc_get_state()!=EXITING);

    std::cout << "Exit" << std::endl;
    sReceiver.destroy();
    rc_motor_cleanup();
    rc_remove_pid_file();

    return EXIT_SUCCESS;
}