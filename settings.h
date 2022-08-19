// WiFi Configuration
const char* ssid = "";
const char* password = "";

// MQTT Cibfiguration
const char* mqtt_server = "192.168.1.10";
const int mqtt_port = 1883;
const char* mqtt_id = "caudalimetro";
const char* mqtt_pub_topic_waterflow = "home/caudalimetro/water";
const char* mqtt_sub_topic_healthcheck = "home/caudalimetro";
const char* mqtt_sub_topic_operation = "home/caudalimetro/operation";

// Other params
const int time_check_sensors = 10000;
const int time_max_update_sensors = 60000;
const int FlowSensorPin = 4;
