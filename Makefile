#merkle_tree: src/merkleTree.o src/main.o hash/customGenerator.o hash/functions.o hash/hash.o
#	g++ -std=c++17 -O3 src/merkleTree.o src/main.o hash/customGenerator.o hash/functions.o hash/hash.o -o merkle_tree -lssl -lcrypto -lws2_32 -lcrypt32

#src/merkleTree.o: src/merkleTree.cpp include/merkleTree.h
#	g++ -std=c++17 -O3 -Iinclude -c src/merkleTree.cpp -o src/merkleTree.o

#src/main.o: src/main.cpp include/merkleTree.h
#	g++ -std=c++17 -O3 -Iinclude -c src/main.cpp -o src/main.o

#hash/customGenerator.o: hash/customGenerator.cpp hash/customGenerator.h
#	g++ -std=c++17 -O3 -Ihash -c hash/customGenerator.cpp -o hash/customGenerator.o

#hash/functions.o: hash/functions.cpp hash/functions.h
#	g++ -std=c++17 -O3 -Ihash -c hash/functions.cpp -o hash/functions.o

#hash/hash.o: hash/hash.cpp hash/customGenerator.h hash/functions.h
#	g++ -std=c++17 -O3 -Ihash -c hash/hash.cpp -o hash/hash.o

#clean:
#	del src\*.o hash\*.o *.exe

#run: merkle_tree
#	./merkle_tree.exe




##nuo cia bandymas make file normalu daryt 

merkle_tree: src/merkleTree.o src/main.o hash/customGenerator.o 
	g++ -std=c++17 -O3 src/merkleTree.o src/main.o hash/customGenerator.o -o merkle_tree -lssl -lcrypto -lws2_32 -lcrypt32

src/merkleTree.o: src/merkleTree.cpp include/merkleTree.h
	g++ -std=c++17 -O3 -Iinclude -c src/merkleTree.cpp -o src/merkleTree.o

src/main.o: src/main.cpp include/merkleTree.h
	g++ -std=c++17 -O3 -Iinclude -c src/main.cpp -o src/main.o

hash/customGenerator.o: hash/customGenerator.cpp hash/customGenerator.h
	g++ -std=c++17 -O3 -Ihash -c hash/customGenerator.cpp -o hash/customGenerator.o

clean:
	del src\*.o hash\*.o *.exe

run: merkle_tree
	./merkle_tree.exe