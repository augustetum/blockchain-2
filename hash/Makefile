hash: customGenerator.o functions.o hash.o
	g++ -std=c++17 -O3 customGenerator.o functions.o hash.o -o hash

customGenerator.o: customGenerator.cpp
	g++ -std=c++17 -O3 -c customGenerator.cpp

functions.o: functions.cpp
	g++ -std=c++17 -O3 -c functions.cpp

hash.o: hash.cpp
	g++ -std=c++17 -O3 -c hash.cpp

clean:
	del *.o *.exe

run: hash
	./hash.exe