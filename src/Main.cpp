#include <iostream>
#include <thread>  // For std::this_thread
#include <chrono>  // For std::chrono

int main() {
    std::cout << "Hello World!\n";
    
    // Pause for 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}