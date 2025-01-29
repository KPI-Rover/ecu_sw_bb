
LIBRARIES := -lpthread
TARGET := server

build: $(TARGET)

$(TARGET): server.cpp TCPServerReceiver.cpp
	g++ $^ -o $(TARGET) $(LIBRARIES)

clean: 

	rm -rf $(TARGET)
