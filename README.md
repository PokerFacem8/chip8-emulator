# CHIP8-Emulator
This is my attempt at creating a simple and lightweight CHIP-8 emulator. The emulator is designed to run classic CHIP-8 games and applications, providing an environment to simulate the CHIP-8 virtual machine.

## Tested ROMS

To verify my Chip8 emulator I used the [Chip8 Test Suite from Timendus](https://github.com/Timendus/chip8-test-suite) which is an excellent resource for validating emulator implementations.

| **ROM Name**              | **Description**                        | **Status**          |
|---------------------------|----------------------------------------|---------------------|
| CHIP-8 Splash Screen      | Displays the Chip-8 logo/splash screen.| Passed              |
| IBM Logo                  | Displays the IBM logo.                 | Passed              |
| Corax+ Opcode Test        | A ROM to test various Chip-8 opcodes.  | Passed              |
| Flags Test                | A ROM to test math operations opcodes. | Not yet             |

## Features
- [x] **Emulates CHIP-8 Instructions**: Fully supports the CHIP-8 instruction set.
- [x] **Graphics Rendering**: Renders CHIP-8 graphics in a window using [Graphics Library].
- [x] **Sound Support**: Plays sound (if applicable).
- [x] **Keyboard Input**: Standard CHIP-8 key mapping for user input.
- [ ] **ImGui Interface**: Uses ImGui for an interactive graphical user interface for managing settings and interacting with the emulator.
- [ ] **Debugger**: Built-in debugger for step-by-step execution and memory inspection.

### 📌 Summary of Required Dependencies

| Dependency     | Purpose                     |
|----------------|-----------------------------|
| **MinGW-w64**  | C++ Compiler (`g++`)        |
| **SDL2**       | Graphics & Input Handling   |
| **Dear ImGui** | GUI Framework               |

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/PokerFacem8/chip8-emulator.git
   cd chip8-emulator/src
   
2. Change paths of cpp_files_list:
   ```bash
   path_to_project\\src\\chip8\\chip8.cpp
   path_to_project\\src\\chip8\\graphics.cpp
   path_to_project\\src\\imgui\\imgui.cpp
   path_to_project\\src\\imgui\\imgui_demo.cpp
   path_to_project\\src\\imgui\\imgui_draw.cpp
   path_to_project\\src\\imgui\\imgui_widgets.cpp
   path_to_project\\src\\imgui\\imgui_impl_sdl2.cpp
   path_to_project\\src\\imgui\\imgui_impl_sdlrenderer2.cpp
   path_to_project\\src\\imgui\\imgui_tables.cpp

3. Compile application:
   ```bash
   cd ../
   g++ -g -std=c++17 -Ipath_to_project/src/include/SDl2 -Ipath_to_project/src/include/imgui -Ipath_to_project/src/include/chip8 -Lpath_to_project/src/lib @path_to_project/src/cpp_files_list.txt path_to_project/src/main.cpp -lmingw32 -lSDL2main -lSDL2 -o path_to_project/chip8-emulator.exe

