/*

 
 RELEASED AS sonoff_release_v1  Feb2019
 
 notes;
        - arduino IDE programming with Generic ESP8266 with DOUT (important)
        1M 64K SPIFFS
        - must be on node 192.168.1.***
        - with MQTT  on 192.168.1.40

        
 Basic ESP8266 MQTT example

 
 

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

const char* ssid = "davieslan";
const char* password = "topsham01";
const char* mqtt_server = "192.168.1.40";
//Set address number of switch

//topics for MQTT server
//Sonoff1  in line  
const char* outTopic = "sonoff1State";
const char* inTopic  = "sonoff1Relay";
//Sonoff2 integrated plug
//const char* outTopic = "sonoff2State";
//const char* inTopic  = "sonoff2Relay";



int gpio13Led = 13;
int gpio12Relay = 12;
int gpio0Switch = 0;


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int Switch = 1;
char switchMsg;

char state = 1;
// 0 relay on   1 relay off



void setup() {
  pinMode(gpio13Led, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(gpio12Relay, OUTPUT);
  pinMode(gpio0Switch, INPUT);






   
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(gpio13Led, LOW);
    digitalWrite (gpio12Relay, LOW);// Turn the LED on (Note that LOW is the voltage level
    state = 1;
    // but actually the LED is on; this is because
    // it is acive low on the ESP-01)
  } else {
    digitalWrite(gpio13Led, HIGH);  // Turn the LED off by making the voltage HIGH
    digitalWrite (gpio12Relay,HIGH);  
    state = 0;
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
     // client.publish("outTopic", "hello world");
        client.publish(outTopic, "hello world");
      
      // ... and resubscribe
      //client.subscribe("inTopic");
      client.subscribe (inTopic);
    
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    Switch = digitalRead(gpio0Switch);
    if (Switch){
      switchMsg = 1;
    
    
    
    }
    else{
      switchMsg = 0;

        if (state){
      
        digitalWrite(gpio13Led, HIGH);  // Turn the LED off by making the voltage HIGH
        digitalWrite (gpio12Relay,HIGH);  
        state = 0;
        }
        
        else{
     
      digitalWrite(gpio13Led, LOW);
      digitalWrite (gpio12Relay, LOW);// Turn the LED on (Note that LOW is the voltage level
       state = 1;
    
        }
    
    }   
//
//
    
    snprintf (msg, 75, "%d:%d",Switch, state);
   
    Serial.print("Publish message: ");
    Serial.println(msg);
    //client.publish("outTopic", msg);
    client.publish(outTopic,msg);
  }
}
