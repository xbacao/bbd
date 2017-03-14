CXX=g++
CXXFLAGS=-g -Wall -c -std=c++11

LDFLAGS=$(shell mysql_config --libs)
INCLUDES=-I"include"
SOURCES=daemon socket_data db log main
OBJECTS=$(addsuffix .o, $(SOURCES))
O_OBJECTS=$(addprefix bin/, $(OBJECTS))
EXECUTABLE=bbda_server
BINS=$(wildcard bin/*.o) $(wildcard $(EXECUTABLE))

SOURCES_T=test db log socket_data
OBJECTS_T=$(addsuffix .o, $(SOURCES_T))
O_OBJECTS_T=$(addprefix bin/, $(OBJECTS_T))
EXECUTABLE_T=test


.PHONY: all
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(O_OBJECTS) -o $@ $(LDFLAGS) $(INCLUDES)
	sudo service/setup_systemd.sh

%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -o bin/$@ $< $(INCLUDES)

db.o main.o test.o: CXXFLAGS += $(shell mysql_config --cflags)

.PHONY: clean
ifneq	"$(BINS)" "  "
clean:
	@echo $(BINS)
	rm $(BINS)
else
clean:;@echo 'Nothing to clean!'
endif



$(EXECUTABLE_T): $(OBJECTS_T)
	$(CXX) $(O_OBJECTS_T) -o $@ $(LDFLAGS) $(INCLUDES)
