#include <iostream>      // For std::cout, std::cerr
#include <stdexcept>     // For std::exception

// Raylib and Raygui includes
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION // Define implementation BEFORE including raygui.h
#include "raygui.h"

// Project-specific headers
#include "Bank.hh"
#include "UIManager.hh"

// Main entry point of the application.
int main() {
    try {
        // 1. Create the core Bank object.
        // This object will manage all customers, accounts, and transactions.
        banking_system::Bank bank;



        // 2. Create the UI Manager, passing the Bank object to it.
        // The UIManager will handle all GUI rendering and user interactions.
        banking_system::UIManager uiManager(bank);

        // 3. Run the UI main loop.
        // This will initialize the window and start the event processing and drawing loop.
        uiManager.run();

    } catch (const std::exception& e) {
        // Catch any standard exceptions that might occur during initialization or runtime.
        std::cerr << "Critical Error: " << e.what() << std::endl;
        // For a GUI application, a simple console error might not be visible.
        // A platform-specific message box could be used here for better user feedback on Windows.
        return 1; // Indicate an error occurred
    } catch (...) {
        // Catch any other unknown exceptions.
        std::cerr << "Unknown Critical Error occurred." << std::endl;
        return 1; // Indicate an error occurred
    }

    return 0; // Program exited successfully
}
