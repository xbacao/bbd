CXX=g++
CXXFLAGS=-g -Wall -c -std=c++11

LD_FLAGS=
INCLUDES=-I"include"
SOURCES=daemon socket_data main
OBJECTS=$(addsuffix .o, $(SOURCES))
O_OBJECTS=$(addprefix bin/, $(OBJECTS))
EXECUTABLE=bbda_server
BINS=$(wildcard bin/*.o) $(wildcard $(EXECUTABLE))

.PHONY: all
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(O_OBJECTS) -o $@ $(LDFLAGS) $(INCLUDES)
	#sudo service/setup_client_service.sh

%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -o bin/$@ $< $(INCLUDES)

.PHONY: clean
ifneq	"$(BINS)" "  "
clean:
	@echo $(BINS)
	rm $(BINS)
else
clean:;@echo 'Nothing to clean!'
endif