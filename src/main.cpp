#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "K.A.O.S.";
const char* password = "f7IaQYs#%Aa";

int state = 0;

const char* host = "http://api.tracey-island.co.uk/esp8266.php"; //edit the host adress, ip address etc.
String url = "/post/"; int adcvalue=0;

const byte numChars = 200;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;

StaticJsonDocument<200> doc;

// HTTP server will listen at port 80
ESP8266WebServer server(80);

const int led = 2;

void handle_led() {
  // get the value of request argument "state" and convert it to an int
  String passedstate = server.arg("state");
  doc["Received"] = passedstate;
  serializeJson(doc, Serial);
  Serial.println("");
  //Serial.println("{\"Received\" : \"" + passedstate + "\"}");
  //Serial.println("");
  //Serial.print("Value passed in ");
  //Serial.println(passedstate);

  if (passedstate=="on") {
      state = 1;
  } else {
      state = 0;
  }

  //int state = server.arg("state").toInt();

  digitalWrite(led, state);
  //server.send(200, "text/plain", String("LED is now ") + ((state)?"on":"off"));
  server.send(200, "text/plain", String("LED is now ") + passedstate);

  //This is the start of the code that was working for http
  Serial.println("{\"URL\" : \"" + url + "\"}");
  //Serial.print("Requesting URL: ");
  //Serial.println(url); //Post Data
  String postData = "{\"action\":\"" + passedstate + "\"}";
  String address = host;
  //String address = host + url;

  //Serial.print("The address is ");
  //Serial.println(address);

  HTTPClient http;
  http.begin(address);
  http.addHeader("Content-Type", "application/json");
  auto httpCode = http.POST(postData);
  Serial.print("{\"Return code\" : \"");
  Serial.print(httpCode);
  Serial.println("\"}");
  //Serial.print("The httpCode is ");
  //Serial.println(httpCode); //Print HTTP return code
  //Serial.println("I am at the payload call");
  String payload = http.getString();
  //Serial.println(payload); //Print request response payload
  http.end(); //Close connection Serial.println();
  //This is the end of the code that was working for http

  //Write out the change of the LED to the software serial port to be picked up by the Arduino.
  //mySerial.write("LED Switched");

  //Now update the OpenHAB dashboard
  //Serial.print("Update the dashboard - ");
  //Serial.println(passedstate);

  //HTTPClient http;
  //http.begin("https://mark%40fox-smith.co.uk:bY&Ex1Ui6bbn@home.myopenhab.org/rest/items/ESPLight");
  //http.addHeader("Content-Type", "text/plain");
  //int httpCode = http.POST("ON");
  //Serial.print("The http code is ");
  //Serial.println(httpCode);

  //String payload = http.getString();

  //Serial.print("The payload is - ");
  //Serial.println(payload);

  //http.end();

  //if (passedstate=="on") {
      //int httpCode = http.POST("ON");
      //Serial.print("The http code is ");
      //Serial.println(httpCode);
  //} else {
      //int httpCode = http.POST("OFF");
      //Serial.print("The http code is ");
      //Serial.println(httpCode);
  //}

}

//void handle_image() {
  //server.send(200, "image/png", "");
  //WiFiClient client = server.client();
  //client.write(image, sizeof(image));
//}

//void handle_webpage_with_image() {
  //server.send(200, "text/html", imagepage);
//}

void setup(void) {
  //Serial.begin(115200);
  Serial.begin(9600);
  //Serial.println("");
  pinMode(led, OUTPUT);

  //Set a host name to identify the device on the network
  //WiFi.hostname("ESPTest");

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

 //MDNS.begin("ESPTest");

  // Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  Serial.println("{\"WiFi Status\" : \"Connected\"}");
  Serial.print("{\"IP Address\" : \"");
  Serial.print(WiFi.localIP());
  Serial.println("\"}");
  //Serial.println("");
  //Serial.print("Connected to ");
  //Serial.println(ssid);
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());

  // Set up the endpoints for HTTP server
  //
  // Endpoints can be written as inline functions:
  //server.on("/", [](){
   //server.send(200, "text/html", form);
  //});

  // And as regular external functions:
  //server.on("/adc", handle_adc);
  server.on("/led", handle_led);
  //server.on("/led.png", handle_image);
  //server.on("/showled", handle_webpage_with_image);

  // Start the server
  server.begin();  // Add this back in
  //Serial.println("HTTP server started");
}

void loop(void) {
  // check for incomming client connections frequently in the main loop:
  static byte ndx = 0;
  char endMarker = '}';
  char rc;

  //StaticJsonDocument<200> doc;

  while (Serial.available() > 0 && newData == false) {
      //Read the next character on the serial bus
      rc = Serial.read();
      Serial.print(rc);  //This has been left in for debugging of the ESP8266 if needed.

      if (rc != endMarker) {
        //The have not received '{' as a charcter which donotes the end of the code that we are intrested in.
        //Add the character which has been read in to those previously read in and increment the counter
        //to get the next character
        receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
                ndx = numChars - 1;
            }
      }
      else {
          //We have received a '(' donating that we have received a full JSON String
          //Make sure that a '(' is added to the string to be processed and
          receivedChars[ndx] = rc;
          ndx++;
          receivedChars[ndx] = '\0'; // terminate the string
          ndx = 0;
          newData = true;

          Serial.print("Received characters *");  //This has been left in for debugging of the ESP8266 if needed.
          Serial.print(receivedChars);            //This has been left in for debugging of the ESP8266 if needed.
          Serial.println("*");                    //This has been left in for debugging of the ESP8266 if needed.
          //Serial.flush();                       //This has been left in for debugging of the ESP8266 if needed.
      }
  }

  if (newData == true) {
      //We have a new JSON string to process
      Serial.print(" New data has been receiced ");  //This has been left in for debugging of the ESP8266 if needed.
      Serial.println(receivedChars);                 //This has been left in for debugging of the ESP8266 if needed.

      char *json = receivedChars;

      DeserializationError error = deserializeJson(doc, json);

      if (error) {
          Serial.print("deserializeJson() failed with code ");
          Serial.println(error.c_str());
          return;
      }

      String item = doc["item"];
      String action = doc["action"];
      Serial.print("Item to be controlled ");
      Serial.println(item);
      Serial.print("Action to be taken ");
      Serial.println(action);

      if (item == "led") {
          if (action == "on") {
              digitalWrite(led, HIGH);
          } else if (action == "off") {
              digitalWrite(led, LOW);
          }
      }

      newData = false;
  }

  server.handleClient();
}
