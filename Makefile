CC = gcc
CFLAGS = -Wall -Wextra

all: converter

converter: converter.c
	$(CC) $(CFLAGS) -o converter converter.c
	@echo "Проект собран\n"
clean:
	rm -rf *.o converter
	@echo "Проект очищен\n"
