#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#ifdef __AVR__
 #include <avr/power.h>
#endif

#include "settings.h"

/* Configuración cliente WiFi */
WiFiClient espClient;

/* Configuración MQTT */
PubSubClient clientMqtt(espClient);
char msg[50];
String mqttcommand = String(14);

/* Configuración sensor de flujo */
volatile int pulseCounter;
float flow_Lmin = 0;
long  frecuency = 0;
long  time_lastcheck = 0;
long  time_lastNotify = 0;
long  last_frequency = 0;

/* -- Funciones ------------------------------------------------- */
ICACHE_RAM_ATTR void ISRCountPulse()
{
   pulseCounter++;
}

int GetFrequency()
{
  int output;
  pulseCounter = 0;
  interrupts();
  delay(SENSOR_INTERVAL);
  noInterrupts();
  output = pulseCounter;
  return output;
}

void setup_wifi() {
  delay(10);

  // Comienza el proceso de conexión a la red WiFi
  Serial.println();
  Serial.print("[WIFI]Conectando a ");
  Serial.println(WIFI_SSID);

  // Modo estación
  WiFi.mode(WIFI_STA);
  // Inicio WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("[WIFI]WiFi conectada (");
  Serial.print(WIFI_SSID);
  Serial.println(")");
  Serial.print("[WIFI]IP: ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("[MQTT]Mensaje recibido (");
  Serial.print(topic);
  Serial.print(") ");
  mqttcommand = "";
  for (int i = 0; i < length; i++) {
    mqttcommand += (char)payload[i];
  }
  Serial.print(mqttcommand);
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if (mqttcommand == "command") {
    Serial.println("don");
  } 
}

void reconnect() {
  Serial.print("[MQTT]Intentando conectar a servidor MQTT (");
  Serial.print(MQTT_SERVER);
  Serial.print(":");
  Serial.print(MQTT_PORT);
  Serial.print(") ...");
  
  // Bucle hasta conseguir conexión
  while (!clientMqtt.connected()) {
    Serial.print(".");
    // Intento de conexión
    if (clientMqtt.connect(MQTT_ID, MQTT_USER, MQTT_PASS)) {
      Serial.println("");
      Serial.println("[MQTT]Conectado al servidor MQTT");
      Serial.print("[MQTT]Publicando en ");
      Serial.println(MQTT_PUB_TOPIC_WATERFLOW);
      // Once connected, publish an announcement...
      clientMqtt.publish(MQTT_PUB_TOPIC_HEALTCHECK, "starting");
      // ... and subscribe
      clientMqtt.subscribe(MQTT_PUB_TOPIC_OPERATION);
    } else {
      Serial.print("[MQTT]Error, rc=");
      Serial.print(clientMqtt.state());
      Serial.println("[MQTT]Intentando conexión en 5 segundos");
      delay(5000);
    }
  }
}

/* -------------------------------------------------------------- */

void setup() {
  Serial.begin(115200);
  digitalWrite(LED_BUILTIN, HIGH);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
    
  /* Iniciar wifi */
  setup_wifi();
  clientMqtt.setServer(MQTT_SERVER, MQTT_PORT);
  clientMqtt.setCallback(callback);

  /* Iiniciar sensor */
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  //attachInterrupt(SENSOR_PIN, my_isr, RISING);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), ISRCountPulse, RISING);
}


void loop() {
  if (!clientMqtt.connected()) {
    reconnect();
  }
  clientMqtt.loop();

  long time_now = millis();
  
  if (time_now - time_lastcheck > TIME_DELAY_CHECK) {
    Serial.println("Resume--------------------------------------");
    
    // obtener frecuencia en Hz
    float frequency = GetFrequency();
    
    if (frequency == 0) {
      if ((time_now - time_lastNotify > TIME_FORCE_UPDATE) || (last_frequency > 0)) {
        clientMqtt.publish(MQTT_PUB_TOPIC_WATERFLOW, "0");
        time_lastNotify = time_now;
        last_frequency = frequency;
      }
      
    } else {
      //Flow pulse characteristics (K*L/Min)
      flow_Lmin = frequency / factorK;
      
      snprintf (msg, 10, "%6.2f", flow_Lmin);
      clientMqtt.publish(MQTT_PUB_TOPIC_WATERFLOW, msg);
      
      time_lastNotify = time_now;
      last_frequency = frequency;
      Serial.print(msg);
      Serial.println(" liters");
    }
    
    Serial.print("Frecuency: ");
    Serial.println(frequency);
    Serial.print("Flow: ");
    Serial.println(flow_Lmin);
    Serial.println("");
    time_lastcheck = time_now;
    frequency = 0;
    flow_Lmin = 0;
  }  
}
