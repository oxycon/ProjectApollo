#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <cstddef>
#include "WiFiServer.h"
#include "CLI.h"

#define MAX_TCP_CLIENTS 3

class TcpClient {
public:
  enum AcceptState { Busy, Accepted, NoConnection };
  TcpClient() : buffer_index(0) { }
  AcceptState accept(WiFiServer& server);
  inline bool isConnected() { return client.connected(); }
  void run();

protected:
  bool readLine();
  char readHex();

  WiFiClient client;
  char buffer[COMMAND_BUFFER_SIZE];
  size_t buffer_index;
  CommandLineInterpreter cli;
};


class TcpServer {
public:
  TcpServer(uint16_t port=23)  : server(port) { }
  void begin() { server.begin(); }
  void run();

protected:
  WiFiServer server;
  TcpClient tcp_clients[MAX_TCP_CLIENTS];
};

#endif // TCP_SERVER_H
