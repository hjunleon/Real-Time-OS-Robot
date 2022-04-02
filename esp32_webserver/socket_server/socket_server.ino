/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "Home";
const char* password = "09021964";

String response;
bool ledState = 0;
const int ledPin = 17;
#define RXD2 16
#define TXD2 17
const int output26 = 26;

// Create AsyncWebServer object on port 80
AsyncWebServer server(9300);   //80
AsyncWebSocket ws("/ws");

void UART_SEND(unsigned long signal) {
  digitalWrite(output26, HIGH);
  Serial2.write(signal);
}


void notifyClients() {
  ws.textAll(response);
}

bool match_string(uint8_t *data, const char* query){
  if (strcmp((char*)data, query) == 0) {
    return true;  
  }
  return false;
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    /*
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      notifyClients();
    }
    */
    if (match_string(data,"f/5")) {
      UART_SEND(0x45);
      response = "FORWARD 5";
    } else if (match_string(data,"b/5")) {
      UART_SEND(0x55);
      response = "BACKWARD 5";
    } else if (match_string(data,"l/5")) {
      UART_SEND(0x65);
      response = "LEFT 5";
    } else if (match_string(data,"r/5")) {
      UART_SEND(0x75);
      response = "RIGHT 5";
    }  else if (match_string(data,"s1"))
    {
      UART_SEND(0x40); // or i just send level 0
      response = "STOP";
    } else if (match_string(data,"s2"))
    {
      UART_SEND(0x60);
      response = "STRAIGHT";
    } else if (match_string(data,"isAuto")) {
      UART_SEND(0x80);
      response = "IS_AUTO";
     } else if (match_string(data,"notAuto")) {
      UART_SEND(0x81);
      response = "NOT_AUTO";
     } else {
      UART_SEND(0x40); // or i just send level 0
      response = "STOP";
    }
    
    Serial.println(response);
    notifyClients();
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  
}


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();
  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
}
