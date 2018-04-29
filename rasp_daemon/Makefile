CFLAGS=-g -Wall -c

SOURCES_CLIENT=serial GPRS client
OBJECTS_CLIENT=$(addprefix bin/,$(addsuffix .o, $(SOURCES_CLIENT)))
EXECUTABLE_CLIENT=client
LDFLAGS_CLIENT=-lpthread -lwiringPi

SOURCES_TEST_SERVER=
OBJECTS_TEST_SERVER=$(addprefix bin/,$(addsuffix .o, $(SOURCES_TEST_SERVER)))
EXECUTABLE_TEST_SERVER=test_server
LDFLAGS_TEST_SERVER=

BINS=$(wildcard bin/*.o ${OBJECTS_TEST_SERVER} ${EXECUTABLE_CLIENT})

.PHONY: all
all: $(EXECUTABLE_CLIENT)

$(EXECUTABLE_CLIENT): $(OBJECTS_CLIENT)
	$(CC) $(OBJECTS_CLIENT) -o $@ $(LDFLAGS_CLIENT) $(INCLUDES)

$(EXECUTABLE_TEST_SERVER): $(OBJECTS_TEST_SERVER)
	$(CC) $(OBJECTS_TEST_SERVER) -o $@ $(LDFLAGS_TEST_SERVER) $(INCLUDES)
	
bin/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $< $(INCLUDES)

.PHONY: clean
ifneq	"$(BINS)" ""
clean:
	rm $(BINS)
else
clean:;@echo 'Nothing to clean!'
endif
