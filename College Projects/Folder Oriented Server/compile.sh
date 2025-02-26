#!/bin/bash
#faz a compilação do parser e em seguida do servidor usando threads
bison -d parser.y
flex scanner.l
gcc server.c lex.yy.c parser.tab.c -o server -lfl -pthread -lcrypt

if [ $? -eq 0 ]; then
    echo "Compilation complete!"
    echo "To use the server run: ./server <webspace> <log.txt> <port> <thread_limit>"
else
    echo "Compilation Error!"
    exit 1
fi