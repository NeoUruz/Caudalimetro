// WiFi Configuration
const char* WIFI_SSID = "";
const char* WIFI_PASSWORD = "";

// MQTT Cibfiguration
const char* MQTT_SERVER = "172.21.2.10";
const int   MQTT_PORT   = 1883;
const char* MQTT_ID     = "water_flow";
const char* MQTT_USER   = "";
const char* MQTT_PASS   = "";
const char* MQTT_PUB_TOPIC_WATERFLOW = "home/water_flow/water";
const char* MQTT_PUB_TOPIC_HEALTCHECK = "home/water_flow";
const char* MQTT_PUB_TOPIC_OPERATION = "home/water_flow/operation";

// Other params
const int TIME_DELAY_CHECK = 10000;
const int TIME_FORCE_UPDATE = 60000;
const int SENSOR_PIN = 4;
const int SENSOR_INTERVAL = 2500;

// YF-S201
//const float factorK = 7.5;
// FS300A
//const float factorK = 5.5;
// FS400A
//const float factorK = 3.5;
const float factorK = 4.8;
