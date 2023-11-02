CC = gcc

SRCS = scheduler.c
TARGET = scheduler.out

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS)


clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
