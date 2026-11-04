CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Iinclude
SOURCES := src/main.cpp src/Account.cpp src/Authentication.cpp src/Admin.cpp src/Bank.cpp src/Customer.cpp
TARGET := OnlineBankingSystem

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
