merkle_tree: src/merkleTree.o src/main.o
	g++ -std=c++17 -O3 src/merkleTree.o src/main.o -o merkle_tree -lssl -lcrypto -lws2_32 -lcrypt32

src/merkleTree.o: src/merkleTree.cpp include/merkleTree.h
	g++ -std=c++17 -O3 -Iinclude -c src/merkleTree.cpp -o src/merkleTree.o

src/main.o: src/main.cpp include/merkleTree.h
	g++ -std=c++17 -O3 -Iinclude -c src/main.cpp -o src/main.o

clean:
	del src\*.o *.exe
	
run: merkle_tree
	./merkle_tree.exe