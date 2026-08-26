prog: DataClassTest1.o DataClassTest2.o NotDataClassTest1.o NotDataClassTest2.o NotDataClassTest3.o
	echo ""

DataClassTest1.o: DataClassTest1.cpp DataClassTest1.h
	g++ -c -Wall DataClassTest1.cpp -nostartfiles -I.

DataClassTest2.o: DataClassTest2.cpp DataClassTest2.h
	g++ -c -Wall DataClassTest2.cpp -nostartfiles -I.

NotDataClassTest1.o: NotDataClassTest1.cpp NotDataClassTest1.h
	g++ -c -Wall NotDataClassTest1.cpp -nostartfiles -I.

NotDataClassTest2.o: NotDataClassTest2.cpp NotDataClassTest2.h
	g++ -c -Wall NotDataClassTest2.cpp -nostartfiles -I.

NotDataClassTest3.o: NotDataClassTest3.cpp NotDataClassTest3.h
	g++ -c -Wall NotDataClassTest3.cpp -nostartfiles -I.

clean:
	rm -f *.o
