CXX=g++
CXXFLAGS=-g -Wall -c -std=c++14

LDFLAGS=$(shell pkg-config --libs libpqxx)
INCLUDES=-I"include"
SOURCES=schedule daemon socket_data db log main
OBJECTS=$(addsuffix .o, $(SOURCES))
O_OBJECTS=$(addprefix bin/, $(OBJECTS))
EXECUTABLE=bbd_daemon
BINS=$(wildcard bin/*.o) $(wildcard $(EXECUTABLE))

SOURCES_T=test log socket_data schedule
OBJECTS_T=$(addsuffix .o, $(SOURCES_T))
O_OBJECTS_T=$(addprefix bin/, $(OBJECTS_T))
EXECUTABLE_T=test

INSTALL=install
INSTALL_DIR=/opt/bbd

.PHONY: start
start:
	$(info no target selected)
	$(info make $(EXECUTABLE))
	$(info make $(EXECUTABLE_T))
	$(info make $(INSTALL))


.PHONY: all
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(O_OBJECTS) -o $@ $(LDFLAGS) $(INCLUDES)

$(INSTALL):
	@if [ ! -d $(INSTALL_DIR) ]; then mkdir -p $(INSTALL_DIR); fi
	cp $(EXECUTABLE) $(INSTALL_DIR)

$(EXECUTABLE_T): $(OBJECTS_T)
	$(CXX) $(O_OBJECTS_T) -o $@ $(LDFLAGS) $(INCLUDES)

db.o: CXXFLAGS += $(shell pkg-config --cflags libpqxx)

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
