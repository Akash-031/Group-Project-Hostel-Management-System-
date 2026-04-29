CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra
TARGET   = hostel_management
SRC      = hostel_management.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)
