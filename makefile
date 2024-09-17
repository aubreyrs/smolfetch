CC = gcc
CFLAGS = -O3 -march=native -mtune=native -flto -ffast-math
LDFLAGS = -s
TARGET = smolfetch
SRCS = smolfetch.c
INSTALL_PATH = /usr/local/bin

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)

install: $(TARGET)
	@echo "innstaling $(TARGET) to $(INSTALL_PATH)"
	@sudo install -m 755 $(TARGET) $(INSTALL_PATH)
	@echo "instannlaton complte"

uninstall:
	@echo "unstianling $(TARGET) from $(INSTALL_PATH)"
	@sudo rm -f $(INSTALL_PATH)/$(TARGET)
	@echo "uninstal compete"

.PHONY: all clean install uninstall