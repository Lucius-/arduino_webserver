/*
  Simple Web server, pmulder

    * Modified from Tom Igoe's Server example
    * Took idea from http://arduino.stackexchange.com/questions/13388/arduino-webserver-faster-alternative-to-indexof-for-parsing-get-requests
    

 */

#include <SPI.h>
#include <Ethernet.h>
#include "http.h"


#define MAX_INPUT 50


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0x20, 0xF2
};

IPAddress ip(192, 168, 2, 188);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

int parseHeader(char header[]) {
  int verb=-1;
  
  Serial.println(header);
  
  if (strlen(header) < 4) {
    verb = 0;
  }

  if (memcmp(header, "GET ", 4) == 0) {
    verb = 1;
  }

  if (memcmp(header, "POST ", 5) == 0) {
    verb = 2;
  }

   return verb;
}

Req processHeader(byte incoming) {
  Req req;
  int temp_verb=-1;
  
  static char header[MAX_INPUT];
  static int verb;
  
  static unsigned int idx = 0;

  req.done = false;
  switch (incoming) {
    // header end detected
    case '\n':
      // terminate with null
      header[idx] = 0;

      // CRLF found = End of Header
      if (idx == 0) {
        req.done = true;
        req.verb = verb;
        verb = 0;
        return req;
      }

      // update HTTP verb if found
      if ((temp_verb = parseHeader(header)) > 0) {
        verb = temp_verb;
      }
      
      // reset buffer for next time
      idx = 0;
      break;

    // discard carriage return
    case '\r':  
      break;

    default:
      if (idx < (MAX_INPUT - 1))
        header[idx++] = incoming;
      break;
  }
  return req;
}

int i=0;
void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  
  if (client) {
    Serial.println("... incoming HTTP request");
    Req req;
    req.done = false;
    while (client.connected() && !req.done) {
      while (client.available () > 0 && !req.done)
        req = processHeader(client.read());
      }

      // route request
      if (req.verb == 1) {
        client.println("HTTP/1.1 200 OK");
        client.println();
        client.println(i);
        client.stop();
      } else if (req.verb == 2) {
        i++;
        client.println("HTTP/1.1 201 OK");
        client.println();
        client.stop();
      }
  }
}

