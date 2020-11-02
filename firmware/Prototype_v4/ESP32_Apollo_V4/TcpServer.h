/*
 *   ESP32 Oxygen Concentrator
 *  ===========================
 * 
 * This software is provided "as is" for educational purposes only. 
 * No claims are made regarding its fitness for medical or any other purposes. 
 * The authors are not liable for any injuries, damages or financial losses.
 * 
 * Use at your own risk!
 * 
 * License: MIT https://github.com/oxycon/ProjectApollo/blob/master/LICENSE.txt
 * For more information see: https://github.com/oxycon/ProjectApollo
 */

#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <cstddef>
#include "WiFiServer.h"
#include "CLI.h"

#ifndef MAX_TCP_CONNECTIONS
#define MAX_TCP_CONNECTIONS 3
#endif

class TcpConnection : public Stream {
public:
  enum AcceptState { Busy, Accepted, NoConnection };
  TcpConnection() :  buffer_index(0), cmdli(this) { }
  AcceptState accept(WiFiServer& server);
  inline bool isConnected() { return connection.connected(); }
  void run();

protected:

  // Stream implementation
  int read();
  int available();
  int peek();
  virtual size_t write(uint8_t val);
  using Print::write; // pull in write(str) and write(buf, size) from Print
  virtual void flush();

  bool readLine();
  char readHex();

  WiFiClient connection;
  char buffer[COMMAND_BUFFER_SIZE];
  size_t buffer_index;
  CommandLineInterpreter cmdli;
};


class TcpServer {
public:
  TcpServer(uint16_t port=23)  : server(port) { }
  void begin() { server.begin(); }
  void run();

protected:
  WiFiServer server;
  TcpConnection tcp_connections[MAX_TCP_CONNECTIONS];
};

#endif // TCP_SERVER_H
