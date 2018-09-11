TARGET  = fakesyslog.so
CC     ?=cc
CFLAGS += -Wall -Wextra -std=c99 -pedantic \
				  -Wmissing-prototypes -Wstrict-prototypes \
				  -fpic -O2

all: $(TARGET)

.PHONY: all clean

clean:
	rm -f $(TARGET)

$(TARGET): fakesyslog.c
	$(CC) $(CFLAGS) -shared -o $@ $^

