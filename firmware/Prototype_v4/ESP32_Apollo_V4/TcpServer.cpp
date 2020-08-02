#include "Config.h"
#include "TcpServer.h"

#include <string.h>

void TcpServer::run() {
  // Check if a connection has connected
  for (size_t i=0; i<MAX_TCP_CONNECTIONS; i++) {
    if (tcp_connections[i].accept(server) == TcpConnection::NoConnection) { break; }
  }

  // Service any connected connections
  for (size_t i=0; i<MAX_TCP_CONNECTIONS; i++) {
    if (tcp_connections[i].isConnected()) {
      tcp_connections[i].run();
    }  
  }
}

TcpConnection::AcceptState TcpConnection::accept(WiFiServer &server) {
  if (isConnected()) {return Busy;}
  connection = server.available();
  if (connection) { 
    DEBUG_println(F("Got connection"));
    return Accepted; 
  }
  return NoConnection;
}

void TcpConnection::run() {
  if (!isConnected()) {
    DEBUG_println(F("Lost TCP connection!"));
    return;
  }
  while (readLine()) {
    DEBUG_println(buffer);
    println(cmdli.execute(buffer));
    yield();
  }
}

bool TcpConnection::readLine() {
  while (isConnected() && connection.available()) {
    char c = read();
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

int TcpConnection::read() {
  if (!isConnected()) { return -1; }    
  return connection.read();
}

int TcpConnection::available() {
  if (!isConnected()) { return -1; }    
  return connection.available();
}

int TcpConnection::peek() {
  if (!isConnected()) { return -1; }    
  return connection.peek();
}

size_t TcpConnection::write(uint8_t val) {
  if (!isConnected()) { return -1; }    
  return connection.write(val);
}

void TcpConnection::flush() {
  if (!isConnected()) { return; }    
  connection.flush();
}
