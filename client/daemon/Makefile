CC=gcc
CFLAGS=-g -Wall -c -std=c11

LDFLAGS=-pthread
INCLUDES=-I"src"

TEST_EXECUTABLE=test_server
TEST_SOURCES=client_controller \
															socket_data/socket_data \
															log/log \
															schedule_manager/schedule_manager \
															valve_manager/valve_manager \
															$(TEST_EXECUTABLE)

TEST_OBJECTS=$(addprefix $(BIN_DIR)/, $(addsuffix .o, $(TEST_SOURCES)))

INSTALL=install
INSTALL_DIR=/opt/bbd
BINS=$(wildcard $(BIN_DIR)/*.o $(BIN_DIR)/*/*.o $(EXECUTABLE) $(TEST_EXECUTABLE))
BIN_DIR=bin
BIN_SUBDIRS=$(filter-out $(BIN_DIR)/,$(dir $(wildcard bin/*/)))

.PHONY: start
start:
	$(info no target selected)
	$(info make $(TEST_EXECUTABLE))


.PHONY: all
all: $(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(TEST_OBJECTS)
	$(CC) $(TEST_OBJECTS) -o $@ $(LDFLAGS) $(INCLUDES)

$(BIN_DIR)/%.o: src/%.c
	@if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
	$(CC) $(CFLAGS) -o $@ $< $(INCLUDES)


.PHONY: clean
ifneq	"$(BINS)" ""
clean:
	@echo $(BINS)
	rm $(BINS)
	rmdir $(BIN_SUBDIRS)
	rmdir $(BIN_DIR)
else
clean:;@echo 'Nothing to clean!'
endif
