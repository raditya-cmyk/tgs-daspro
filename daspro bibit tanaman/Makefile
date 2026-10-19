# Menentukan compiler C++ yang digunakan
# -Wall: menampilkan warning saat kompilasi
# -std=c++11: menggunakan standar C++11
# -Iinclude: lokasi file header (.h)
# OBJ: daftar file object

CXX      = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -Iinclude
OBJ      = main.o bibit.o fileio.o ui.o

# Target utama (linking)
ifeq ($(OS),Windows_NT)
TARGET = bibit.exe
else
TARGET = bibit
endif

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Aturan kompilasi file objek
main.o: src/main.cpp include/bibit.h include/fileio.h include/ui.h
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o main.o

bibit.o: src/bibit.cpp include/bibit.h include/ui.h
	$(CXX) $(CXXFLAGS) -c src/bibit.cpp -o bibit.o

fileio.o: src/fileio.cpp include/fileio.h include/bibit.h
	$(CXX) $(CXXFLAGS) -c src/fileio.cpp -o fileio.o

ui.o: src/ui.cpp include/ui.h include/bibit.h include/fileio.h
	$(CXX) $(CXXFLAGS) -c src/ui.cpp -o ui.o

run: $(TARGET)
	./$(TARGET)

ifeq ($(OS),Windows_NT)
clean:
	-del /Q /F $(OBJ) $(TARGET) 2>NUL
else
clean:
	rm -f $(OBJ) $(TARGET)
endif
