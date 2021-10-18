
//Tangerine Tech IR controller over WiFi 


#include <WiFi.h>
#include "PinDefinitionsAndMore.h"
#include <IRremote.h>

//###########################################################################//
//###########################################################################//

//                 ENTER YOUR OWN DETAILS IN THIS SECTION

const char* ssid     = "ENTER_YOUR_SSID";
const char* password = "ENTER_YOUR_PASSWORD";

uint16_t sAddress = 0x; //Enter the signal address for your button
uint8_t sCommand = 0x;  //Enter the signal command for your button
uint8_t sRepeats = 0;

//###########################################################################//
//###########################################################################//


WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    pinMode(2, OUTPUT);      // set the LED pin mode

    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));
    IrSender.begin(IR_SEND_PIN, ENABLE_LED_FEEDBACK); // Specify send pin and enable feedback LED at default feedback LED pin

    Serial.print(F("Ready to send IR signals at pin "));
    Serial.println(IR_SEND_PIN);

    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

int value = 0;

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the TV on.<br>");
            client.print("Click <a href=\"/L\">here</a> to do nothing.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {

          //for (int i = 0; i <= 3; i++) {           //Send the IR data 4 times if you include this for loop
          Serial.println();
          Serial.print(F("Send now: address=0x"));
          Serial.print(sAddress, HEX);
          Serial.print(F(" command=0x"));
          Serial.print(sCommand, HEX);
          Serial.print(F(" repeats=")); 
          Serial.print(sRepeats);
          Serial.println();
      
          Serial.println(F("Send NEC with 16 bit address"));
          Serial.flush();
      
          // Results for the first loop to: Protocol=NEC Address=0x102 Command=0x34 Raw-Data=0xCB340102 (32 bits)
          IrSender.sendNEC(sAddress, sCommand, sRepeats);
           //}

        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(2, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
