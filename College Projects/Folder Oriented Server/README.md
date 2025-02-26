Objectives:
Using a lexical (flex) and syntactic (bison) analyzer in conjunction with the C language, create a server that is capable of receiving multiple requests, reading them with the analayzer, processing them and then sending a response that is compatible with what was requested and what was processed.
How to use the system:
You need to run the "compile.sh" program by doing:
./compile.sh
Then run the server, defining which webspace, which log file, which port will be used in the connection and what is the thread limit to be used:
./server <webspace> <log.txt> <port> <thread_limit>
Finally, open the browser and type:
http://localhost:<port>/<file-path>
or
http://<machine-IP>:<port>/<file-path>    (if you are accessing from another machine)