#include <iostream>   
#include <stdexcept> 

// Raylib and Raygui includes
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION 
#include "raygui.h"
#include "Bank.hh"
#include "UIManager.hh"

int main() {
    try {
        // 1. Create the core Bank object. This object will manage all customers, accounts, and transactions.
        banking_system::Bank bank;



        // 2. Create the UI Manager, passing the Bank object to it. The UIManager will handle all GUI rendering and user interactions.
        banking_system::UIManager uiManager(bank);

        // 3. Run the UI main loop. This will initialize the window and start the event processing and drawing loop.
        uiManager.run();

    } catch (const std::exception& e) {
        std::cerr << "Critical Error: " << e.what() << std::endl;
        
        return 1;
        // Catch any other unknown exceptions.
        std::cerr << "Unknown Critical Error occurred." << std::endl;
        return 1;
    }

    return 0; 
}
