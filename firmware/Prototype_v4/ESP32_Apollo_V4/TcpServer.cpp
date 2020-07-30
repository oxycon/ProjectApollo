#include "config.h"
#include "TcpServer.h"

#include <string.h>

void TcpServer::run() {
  // Check if a client has connected
  for (size_t i=0; i<MAX_TCP_CLIENTS; i++) {
    if (tcp_clients[i].accept(server) == TcpClient::NoConnection) { break; }
  }

  // Service any connected clients
  for (size_t i=0; i<MAX_TCP_CLIENTS; i++) {
    if (tcp_clients[i].isConnected()) {
      tcp_clients[i].run();
    }  
  }
}

TcpClient::AcceptState TcpClient::accept(WiFiServer &server) {
  if (isConnected()) {return Busy;}
  client = server.available();
  if (client) { 
    DEBUG_println(F("Got client"));
    return Accepted; 
  }
  return NoConnection;
}

void TcpClient::run() {
  if (!client.connected()) {
    DEBUG_println(F("Lost WIFI client!"));
    return;
  }
  while (readLine()) {
    DEBUG_println(buffer);
    client.println(cli.execute(buffer));
    yield();
  }
}


bool TcpClient::readLine() {
  while (client.available()) {
    char c = client.read();
    buffer[buffer_index++] = c;
    if (c == '\n' || c == '\r' || c == '\0') { 
      buffer[buffer_index-1] = '\0';
      buffer_index = 0;
      if (buffer[0] != '\0') return true;
    }
    if (buffer_index > COMMAND_BUFFER_SIZE-1) {
      buffer_index = COMMAND_BUFFER_SIZE-1;
    }
  }
  return false;
}
