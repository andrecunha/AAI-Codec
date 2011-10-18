#!/bin/bash

make encode

valgrind --leak-check=full --track-origins=yes --show-reachable=yes ../bin/encode -d jad0003b.wav jad0003b.bin >> saida.txt
valgrind --leak-check=full --track-origins=yes --show-reachable=yes ../bin/encode -c jad0003b.wav jad0003b.bin >> saida.txt
valgrind --leak-check=full --track-origins=yes --show-reachable=yes ../bin/encode -h jad0003b.wav jad0003b.bin >> saida.txt
valgrind --leak-check=full --track-origins=yes --show-reachable=yes ../bin/encode -dc jad0003b.wav jad0003b.bin >> saida.txt
valgrind --leak-check=full --track-origins=yes --show-reachable=yes ../bin/encode -cd jad0003b.wav jad0003b.bin >> saida.txt
valgrind --leak-check=full --track-origins=yes --show-reachable=yes ../bin/encode -dh jad0003b.wav jad0003b.bin >> saida.txt
valgrind --leak-check=full --track-origins=yes --show-reachable=yes ../bin/encode -ch jad0003b.wav jad0003b.bin >> saida.txt
valgrind --leak-check=full --track-origins=yes --show-reachable=yes ../bin/encode -cdh jad0003b.wav jad0003b.bin >> saida.txt
valgrind --leak-check=full --track-origins=yes --show-reachable=yes ../bin/encode -dch jad0003b.wav jad0003b.bin >> saida.txt
