CC = gcc
CFLAGS = -Wall -Wextra
OUTPUT = conv

ifeq ($(OS),Windows_NT)
    EXT = .exe
	RM = del /Q
else
    EXT =
	RM = rm -f
endif

TARGET = $(OUTPUT)$(EXT)

all: $(TARGET)

$(TARGET): converter.c
	$(CC) $(CFLAGS) -o $(TARGET) converter.c
	@echo "Build successful"
clean:
	$(RM) *.o $(TARGET)
	@echo "Clean completed"

