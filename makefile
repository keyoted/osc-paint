CODE_D = OSCPbyKTD
EXEC_NAME = exe.x86
BIN_D = bin
RSR_D = $(CODE_D)/resources
SRC_D = $(CODE_D)/src
HDR_D = $(CODE_D)/hdr
FLAGS = -std=c++17 -I$(HDR_D) -I$(RSR_D)
MAIN_SOURCES = $(SRC_D)/ResourcePath.cpp $(SRC_D)/main.cpp $(SRC_D)/nuklear_GUI.cpp $(SRC_D)/OSC_Reciever.cpp $(SRC_D)/surface.cpp $(SRC_D)/window.cpp
MAIN_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lGL -ldl -lGLEW -lSDL2_net

# make sure you have sfml installed
# make sure you have sdl2_net installed
# make sure you have glew installed

build:
	$(CXX) $(FLAGS) $(MAIN_SOURCES) -o $(BIN_D)/$(EXEC_NAME) $(MAIN_FLAGS)
	cp -r $(RSR_D) $(BIN_D)

run:
	$(BIN_D)/$(EXEC_NAME)