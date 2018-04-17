CFLAGS=-g -Wall -c
LDFLAGS=-lpthread -lwiringPi

SOURCES_CLIENT=GPRS client
OBJECTS_CLIENT=$(addprefix bin/,$(addsuffix .o, $(SOURCES_CLIENT)))
EXECUTABLE_CLIENT=client

SOURCES_SERVER=
OBJECTS_SERVER=$(addprefix bin/,$(addsuffix .o, $(SOURCES_SERVER)))
EXECUTABLE_SERVER=server

BINS=$(wildcard bin/*.o ${EXECUTABLE_SERVER} ${EXECUTABLE_CLIENT})

.PHONY: all
all: $(EXECUTABLE_CLIENT)

$(EXECUTABLE_SERVER): $(OBJECTS_SERVER)
	$(CC) $(OBJECTS_SERVER) -o $@ $(LDFLAGS) $(INCLUDES)

$(EXECUTABLE_CLIENT): $(OBJECTS_CLIENT)
	$(CC) $(OBJECTS_CLIENT) -o $@ $(LDFLAGS) $(INCLUDES)

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
