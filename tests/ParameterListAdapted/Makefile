prog: LongParameterListTest.o NotLongParameterListTest1.o NotLongParameterListTest2.o NotLongParameterListTest3.o NotLongParameterListTest4.o
	echo ""

LongParameterListTest.o: LongParameterListTest.cpp LongParameterListTest.h
	g++ -c -Wall LongParameterListTest.cpp -nostartfiles -I.

NotLongParameterListTest1.o: NotLongParameterListTest1.cpp NotLongParameterListTest1.h
	g++ -c -Wall NotLongParameterListTest1.cpp -nostartfiles -I.

NotLongParameterListTest2.o: NotLongParameterListTest2.cpp NotLongParameterListTest2.h
	g++ -c -Wall NotLongParameterListTest2.cpp -nostartfiles -I.

NotLongParameterListTest3.o: NotLongParameterListTest3.cpp NotLongParameterListTest3.h
	g++ -c -Wall NotLongParameterListTest3.cpp -nostartfiles -I.

NotLongParameterListTest4.o: NotLongParameterListTest4.cpp NotLongParameterListTest4.h
	g++ -c -Wall NotLongParameterListTest4.cpp -nostartfiles -I.

clean:
	rm -f *.o
