prog: SpaghettiCodeTest1.o SpaghettiCodeTest2.o NotSpaghettiCodeTest.o
	echo ""

SpaghettiCodeTest1.o: SpaghettiCodeTest1.cpp SpaghettiCodeTest1.h
	g++ -c -Wall SpaghettiCodeTest1.cpp -nostartfiles -I.

SpaghettiCodeTest2.o: SpaghettiCodeTest2.cpp SpaghettiCodeTest2.h
	g++ -c -Wall SpaghettiCodeTest2.cpp -nostartfiles -I.

NotSpaghettiCodeTest.o: NotSpaghettiCodeTest.cpp NotSpaghettiCodeTest.h
	g++ -c -Wall NotSpaghettiCodeTest.cpp -nostartfiles -I.

clean:
	rm -f *.o
