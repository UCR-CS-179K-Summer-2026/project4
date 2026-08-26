CC = g++
CFLAGS = -std=c++11 -Wall

all: main

main: main.o Customer.o Cashier.o Chef.o Pizza.o Shop.o Drink.o
	$(CC) $(CFLAGS) -o main main.o Customer.o Cashier.o Chef.o Pizza.o Shop.o Drink.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

Customer.o: Customer.cpp Customer.h
	$(CC) $(CFLAGS) -c Customer.cpp

Cashier.o: Cashier.cpp Cashier.h
	$(CC) $(CFLAGS) -c Cashier.cpp

Chef.o: Chef.cpp Chef.h
	$(CC) $(CFLAGS) -c Chef.cpp

Pizza.o: Pizza.cpp Pizza.h
	$(CC) $(CFLAGS) -c Pizza.cpp

Shop.o: Shop.cpp Shop.h
	$(CC) $(CFLAGS) -c Shop.cpp

Drink.o: Drink.cpp Drink.h
	$(CC) $(CFLAGS) -c Drink.cpp

clean:
	rm -f *.o main
