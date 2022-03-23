// Load Wi-Fi library
#include <WiFi.h>

#define RXD2 16
#define TXD2 17

// Replace with your network credentials
const char* ssid = "Home";//"OnePlus 8 Pro";//"LAPTOP-PLPLNI3L 7241";
const char* password = "09021964";//"31080616a22d";//"jinxjinx";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String response, ip_address;

// Auxiliar variables to store the current output state
String output26State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
int wait30 = 30000; // time to reconnect when connection is lost.

// This is your Static IP
IPAddress local_IP(192, 168, 27, 189);
// Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);

  //  //Configure Static IP
  //  if(!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  //  {
  //    Serial.println("Static IP failed to configure");
  //  }

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  ip_address = WiFi.localIP().toString();
  Serial.println(ip_address);
  server.begin();
}

void UART_SEND(unsigned long signal) {
  digitalWrite(output26, HIGH);
  Serial2.write(signal);
}

void loop() {

  // If disconnected, try to reconnect every 30 seconds.
  if ((WiFi.status() != WL_CONNECTED) && (millis() > wait30)) {
    Serial.println("Trying to reconnect WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    wait30 = millis() + 30000;
  }
  // Check if a client has connected..
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.print("New client: ");
  Serial.println(client.remoteIP());

  // Espera hasta que el cliente envíe datos.
  // while(!client.available()){ delay(1); }

  /////////////////////////////////////////////////////
  // Read the information sent by the client.
  String req = client.readStringUntil('\r');
  Serial.println(req);

  // Make the client's request.
  if (req.indexOf("status") != -1)
  {
    response = "WiFi Connected: " + ip_address;
  }

  // new
  // TODO: Change to switch if possible

  if (req.indexOf("forward/1") != -1) {
    UART_SEND(0x41);
    response = "FORWARD 1";
  } else if (req.indexOf("forward/2") != -1) {
    UART_SEND(0x42);
    response = "FORWARD 2";
  } else if (req.indexOf("forward/3") != -1) {
    UART_SEND(0x43);
    response = "FORWARD 3";
  } else if (req.indexOf("forward/4") != -1) {
    UART_SEND(0x44);
    response = "FORWARD 4";
  } else if (req.indexOf("forward/5") != -1) {
    UART_SEND(0x45);
    response = "FORWARD 5";
  } else if (req.indexOf("backward/1") != -1) {
    UART_SEND(0x51);
    response = "BACKWARD 1";
  } else if (req.indexOf("backward/2") != -1) {
    UART_SEND(0x52);
    response = "BACKWARD 2";
  } else if (req.indexOf("backward/3") != -1) {
    UART_SEND(0x53);
    response = "BACKWARD 3";
  } else if (req.indexOf("backward/4") != -1) {
    UART_SEND(0x54);
    response = "BACKWARD 4";
  } else if (req.indexOf("backward/5") != -1) {
    UART_SEND(0x55);
    response = "BACKWARD 5";
  } else if (req.indexOf("left/1") != -1) {
    UART_SEND(0x61);
    response = "LEFT 1";
  } else if (req.indexOf("left/2") != -1) {
    UART_SEND(0x62);
    response = "LEFT 2";
  } else if (req.indexOf("left/3") != -1) {
    UART_SEND(0x63);
    response = "LEFT 3";
  } else if (req.indexOf("left/4") != -1) {
    UART_SEND(0x64);
    response = "LEFT 4";
  } else if (req.indexOf("left/5") != -1) {
    UART_SEND(0x65);
    response = "LEFT 5";
  } else if (req.indexOf("right/1") != -1) {
    UART_SEND(0x71);
    response = "RIGHT 1";
  } else if (req.indexOf("right/2") != -1) {
    UART_SEND(0x72);
    response = "RIGHT 2";
  } else if (req.indexOf("right/3") != -1) {
    UART_SEND(0x73);
    response = "RIGHT 3";
  } else if (req.indexOf("right/4") != -1) {
    UART_SEND(0x74);
    response = "RIGHT 4";
  } else if (req.indexOf("right/5") != -1) {
    UART_SEND(0x75);
    response = "RIGHT 5";
  } else if (req.indexOf("stop") != -1)
  {
    UART_SEND(0x40);
    response = "STOP";
  } else if (req.indexOf("straight") != -1)
  {
    UART_SEND(0x60);
    response = "STRAIGHT";
  }


  /*
       if (req.indexOf("on12") != -1) {digitalWrite(LED12, HIGH); estado = "LED12 ON";}
       if (req.indexOf("off12") != -1){digitalWrite(LED12, LOW); estado = "LED12 OFF";}
       if (req.indexOf("on14") != -1) {digitalWrite(LED14, HIGH); estado = "LED14 ON";}
       if (req.indexOf("off14") != -1){digitalWrite(LED14, LOW); estado = "LED14 OFF";}
       if (req.indexOf("consulta") != -1){
           estado ="";
           if (digitalRead(LED12) == HIGH) {estado = "LED12 ON,";} else {estado = "LED12 OFF,";}
           if (digitalRead(LED14) == HIGH) {estado = estado + "LED14 ON";} else {estado = estado + "LED14 OFF";}
           }*/


  client.println("HTTP/1.1 200 OK");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Content-Type: text/html");
  client.println("");
  client.println(response); //  Return status.

  client.flush();
  client.stop();
  Serial.println("Client disconnected.");
}
