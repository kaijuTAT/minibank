#pragma once // Header guard

#include <string>
#include <chrono> // For time-related utilities

namespace banking_system {
namespace utils { // Nested namespace for utility functions

// File: Utils.hh
// Purpose: Defines a namespace 'utils' containing miscellaneous utility functions
// that can be used across the banking system application.
// Currently, it includes a function to get the current date as a string.

    // Returns the current date formatted as a "YYYY-MM-DD" string.
    std::string getCurrentDateString();

    // Other general utility functions can be added here as needed,
    // for example, for string manipulation, complex ID generation, or input validation.

} // namespace utils
} // namespace banking_system
