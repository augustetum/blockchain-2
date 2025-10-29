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

#merkle_tree: src/merkleTree.o src/main.o hash/customGenerator.o 
#	g++ -std=c++17 -O3 src/merkleTree.o src/main.o hash/customGenerator.o -o merkle_tree -lssl -lcrypto -lws2_32 -lcrypt32

#src/merkleTree.o: src/merkleTree.cpp include/merkleTree.h
#	g++ -std=c++17 -O3 -Iinclude -c src/merkleTree.cpp -o src/merkleTree.o

#src/main.o: src/main.cpp include/merkleTree.h
#	g++ -std=c++17 -O3 -Iinclude -c src/main.cpp -o src/main.o

#hash/customGenerator.o: hash/customGenerator.cpp hash/customGenerator.h
#	g++ -std=c++17 -O3 -Ihash -c hash/customGenerator.cpp -o hash/customGenerator.o

#clean:
#	del src\*.o hash\*.o *.exe

#run: merkle_tree
#	./merkle_tree.exe


# Main blockchain program
blockchain: Program.o Block.o BlockHeader.o Transaction.o Functions.o merkleTree.o customGenerator.o
	g++ -std=c++17 -O3 Program.o Block.o BlockHeader.o Transaction.o Functions.o merkleTree.o customGenerator.o -o blockchain -lssl -lcrypto -lws2_32 -lcrypt32

Program.o: Program.cpp
	g++ -std=c++17 -O3 -Iinclude -Ihash -c Program.cpp -o Program.o

Block.o: Block.cpp Block.h
	g++ -std=c++17 -O3 -Iinclude -Ihash -c Block.cpp -o Block.o

BlockHeader.o: BlockHeader.cpp BlockHeader.h
	g++ -std=c++17 -O3 -c BlockHeader.cpp -o BlockHeader.o

Transaction.o: Transaction.cpp Transaction.h
	g++ -std=c++17 -O3 -Ihash -c Transaction.cpp -o Transaction.o

Functions.o: Functions.cpp
	g++ -std=c++17 -O3 -Iinclude -Ihash -c Functions.cpp -o Functions.o

merkleTree.o: merkleTree.cpp merkleTree.h
	g++ -std=c++17 -O3 -Iinclude -Ihash -c merkleTree.cpp -o merkleTree.o

customGenerator.o: customGenerator.cpp customGenerator.h
	g++ -std=c++17 -O3 -Ihash -c customGenerator.cpp -o customGenerator.o

# User generator
usergen: generators/userGenerator.o customGenerator.o
	g++ -std=c++17 -O3 generators/userGenerator.o customGenerator.o -o usergen -lssl -lcrypto -lws2_32 -lcrypt32

generators/userGenerator.o: generators/userGenerator.cpp
	g++ -std=c++17 -O3 -Ihash -c generators/userGenerator.cpp -o generators/userGenerator.o

# Transaction generator
txgen: generators/transactionGenerator.o Transaction.o customGenerator.o
	g++ -std=c++17 -O3 generators/transactionGenerator.o Transaction.o customGenerator.o -o txgen -lssl -lcrypto -lws2_32 -lcrypt32

generators/transactionGenerator.o: generators/transactionGenerator.cpp
	g++ -std=c++17 -O3 -Ihash -c generators/transactionGenerator.cpp -o generators/transactionGenerator.o

clean:
	del *.o generators\*.o *.exe

run: blockchain
	./blockchain.exe

setup: usergen txgen
	./usergen.exe
	./txgen.exe

all: usergen txgen blockchain
	./usergen.exe
	./txgen.exe
	./blockchain.exe


