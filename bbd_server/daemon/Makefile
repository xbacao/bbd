CXX=g++
CXXFLAGS=-g -Wall -c -std=c++14

LDFLAGS=$(shell pkg-config --libs libpqxx)
INCLUDES=-I"src"
SOURCES=main schedule/schedule daemon/daemon socket_data/socket_data db/db log/log
OBJECTS=$(addprefix bin/, $(addsuffix .o, $(SOURCES)))
EXECUTABLE=bbd_daemon
BINS=$(wildcard bin/*.o) $(wildcard $(EXECUTABLE))

SOURCES_T=test log socket_data schedule
OBJECTS_T=$(addsuffix .o, $(SOURCES_T))
O_OBJECTS_T=$(addprefix $(BIN_DIR)/, $(OBJECTS_T))
EXECUTABLE_T=test

INSTALL=install
INSTALL_DIR=/opt/bbd
BIN_DIR=bin

.PHONY: start
start:
	$(info no target selected)
	$(info make $(EXECUTABLE))
	$(info make $(EXECUTABLE_T))
	$(info make $(INSTALL))


.PHONY: all
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS) $(INCLUDES)

$(INSTALL):
	@if [ ! -d $(INSTALL_DIR) ]; then mkdir -p $(INSTALL_DIR); fi
	cp $(EXECUTABLE) $(INSTALL_DIR)

$(EXECUTABLE_T): $(OBJECTS_T)
	$(CXX) $(O_OBJECTS_T) -o $@ $(LDFLAGS) $(INCLUDES)

db.o: CXXFLAGS += $(shell pkg-config --cflags libpqxx)

$(BIN_DIR)/%.o: src/%.cpp
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(CXX) $(CXXFLAGS) -o $@ $< $(INCLUDES)


.PHONY: clean
ifneq	"$(BINS)" "  "
clean:
	@echo $(BINS)
	rm $(BINS)
else
clean:;@echo 'Nothing to clean!'
endif
