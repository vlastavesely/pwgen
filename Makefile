TARGET = pwgen
CC = gcc

.PHONY: all test clean

all: $(TARGET)

$(TARGET): pwgen.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -MMD -MP -c $< -o $@

test: $(TARGET)
	./$(TARGET) --lower --upper
	./$(TARGET) -n 32 -lud
	./$(TARGET) -n 32 -a

clean:
	rm -f $(TARGET) *.o *.d
