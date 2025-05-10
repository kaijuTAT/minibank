# CMakeLists.txt

# Specify the minimum version of CMake required to build this project.
cmake_minimum_required(VERSION 3.12)

# Define the project name, version, and languages used.
# C is required for Raylib, CXX (C++) for the application logic.
project(MiniBankingSystem VERSION 1.0 LANGUAGES C CXX)

# Set the C++ standard to be used (e.g., C++17).
# CMAKE_CXX_STANDARD_REQUIRED ON ensures that the standard is enforced.
# CMAKE_CXX_EXTENSIONS OFF disables compiler-specific extensions for better portability.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Raylib Configuration ---
# This section configures how the Raylib library (added as a subdirectory) is built.

# Force Raylib to be built as a static library (.lib or .a).
# This helps in creating a more self-contained executable.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build Raylib as a static library")

# Optionally disable building Raylib examples and games to speed up the build process.
set(RAYLIB_BUILD_EXAMPLES OFF CACHE BOOL "Do not build Raylib examples")
set(RAYLIB_BUILD_GAMES OFF CACHE BOOL "Do not build Raylib games")
# set(RAYLIB_BUILD_TESTS OFF CACHE BOOL "Do not build Raylib tests") # Uncomment if tests are not needed

# Add the Raylib source directory. CMake will find Raylib's own CMakeLists.txt
# and build it as a target named 'raylib'.
add_subdirectory(external/raylib)

# --- Application Configuration ---
# This section configures the main application executable.

# Define the executable target for the banking application.
# The output executable will be named 'MiniBankingApp' (or MiniBankingApp.exe on Windows).
add_executable(MiniBankingApp)

# Specify all C++ source files for the application.
# main.cpp includes the RAYGUI_IMPLEMENTATION.
target_sources(MiniBankingApp
    PRIVATE
        src/main.cpp
        src/Account.cpp
        src/SavingsAccount.cpp
        src/CheckingAccount.cpp
        src/Transaction.cpp
        src/Customer.cpp
        src/Bank.cpp
        src/Utils.cpp
        src/UIManager.cpp
)

# Specify include directories for the application.
# This allows the compiler to find header files located in the 'include' directory.
target_include_directories(MiniBankingApp
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include # Path to the project's 'include' folder
)

# Link the application against the Raylib static library.
# CMake automatically handles finding the 'raylib' target and its dependencies.
# 'PUBLIC' ensures that if MiniBankingApp were a library, its users would also link to raylib
# and get its include directories. For an executable, PRIVATE might also work here,
# but PUBLIC is safer for propagating Raylib's usage requirements.
target_link_libraries(MiniBankingApp
    PUBLIC
        raylib
)

# Optional: Set properties for Windows executables.
# WIN32_EXECUTABLE creates a windowed application (no console window) on Windows.
if(WIN32)
    set_target_properties(MiniBankingApp PROPERTIES WIN32_EXECUTABLE ON)
endif()
