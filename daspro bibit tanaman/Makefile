CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic
SRC_DIR = src
INC_DIR = include
OBJ_DIR = build
TARGET = bibit
OBJECTS = $(OBJ_DIR)/main.o $(OBJ_DIR)/bibit.o $(OBJ_DIR)/fileio.o $(OBJ_DIR)/ui.o

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(INC_DIR)/bibit.h $(INC_DIR)/fileio.h $(INC_DIR)/ui.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/bibit.o: $(SRC_DIR)/bibit.cpp $(INC_DIR)/bibit.h $(INC_DIR)/ui.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/fileio.o: $(SRC_DIR)/fileio.cpp $(INC_DIR)/fileio.h $(INC_DIR)/bibit.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

$(OBJ_DIR)/ui.o: $(SRC_DIR)/ui.cpp $(INC_DIR)/ui.h $(INC_DIR)/bibit.h $(INC_DIR)/fileio.h | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
