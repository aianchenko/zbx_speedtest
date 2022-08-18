
INCLUDE_DIR = -I.

SOURCES = speedtest.cpp \
          uri.cpp \
          connection.cpp \
          timer.cpp

OBJECTS := $(notdir $(SOURCES:.cpp=.o))

all : $(OBJECTS)
	g++ $(OBJECTS) -o speedtest -lpthread -std=c++11

%.o : %.cpp
	g++ -g $(CFLAGS) $(INCLUDE_DIR) -o $@ -c $< -std=c++11

clean:
	rm -rf *.o
	rm -f speedtest
