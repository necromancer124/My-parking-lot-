CC = gcc
CFLAGS = -Wall -Wextra
SRC = parckingLot.c
TARGET = parking_lot

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) settings.txt carsHistory.txt
