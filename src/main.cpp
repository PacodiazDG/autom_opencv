#include <iostream>
#include "include/qthreads.hxx"

int main(int argc, char **argv) {

    std::cout << "Starting the application..." << std::endl;
    qthreads threads;
    if (!threads.start()) {
        std::cerr << "Failed to start threads." << std::endl;
        return -1;
    }
    
    
    std::cout << "Application finished." << std::endl;
    return 0;

}
