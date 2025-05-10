#include "Utils.hh"
#include <ctime>   // For std::time_t, std::tm, localtime_s/localtime_r
#include <iomanip> // For std::put_time
#include <sstream> // For std::stringstream

namespace banking_system {
namespace utils {

// Implementation of getCurrentDateString
// Retrieves the current system date and formats it.
std::string getCurrentDateString() {
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm;

    // Use platform-specific thread-safe localtime functions
    #ifdef _WIN32
        localtime_s(&local_tm, &time_now);
    #else
        localtime_r(&time_now, &local_tm);
    #endif

    std::stringstream ss;
    ss << std::put_time(&local_tm, "%Y-%m-%d"); // Format: YYYY-MM-DD
    return ss.str();
}

} // namespace utils
} // namespace banking_system
