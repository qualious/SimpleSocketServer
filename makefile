CXX := gcc
CXXFLAGS := -Wall
CXXTHEAD := -lpthread
GREEN :=\033[0;32m
NC :=\033[0m

all: finale

finale: echo_server.o echo_client.o chat_server.o chat_client.o

echo_server.o: echo_server.c
	$(CXX) $(CXXFLAGS) echo_server.c -o echo_server.o

echo_client.o: echo_client.c
	$(CXX) $(CXXFLAGS) echo_client.c -o echo_client.o

chat_server.o: chat_server.c
	$(CXX) $(CXXFLAGS) $(CXXTHEAD) chat_server.c -o chat_server.o

chat_client.o: chat_client.c
	$(CXX) $(CXXFLAGS) $(CXXTHEAD) chat_client.c -o chat_client.o

finale:
	@echo "${GREEN}Success!${NC}\nExecute ./echo_server.o && ./echo_client.o to test the echo server. \n\
	Execute ./chat_server.o && ./chat_client.o (as much as you want) in order \
	to test the chat server."

clean:
	rm -f *.o *.out
