
LIBRARIES := -lpthread
TARGET := server

build: $(TARGET)

$(TARGET): server.cpp
	g++ server.cpp -o $(TARGET) $(LIBRARIES)

clean: $(TARGET)

	rm -rf $(TARGET)
