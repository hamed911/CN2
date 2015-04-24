all: client server switch service_provider
client: client.c utils
	gcc client.c utilities.o -o Client
service_provider: service_provider.c utils
	gcc service_provider.c utilities.o -o Service_provider
switch: switch.c utils
	gcc switch.c utilities.o -o Switch
server: server.c utils
	gcc server.c utilities.o -o Server
utils: utilities.c utilities.h
	gcc -c  utilities.c -o utilities.o
.PHONY: clean
clean:
	rm *.o Client Server Switch Service_provider
