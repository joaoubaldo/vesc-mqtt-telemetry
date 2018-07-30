#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

#include <ESP8266VESC.h>
#include <MQTT.h>

SoftwareSerial softwareSerial = SoftwareSerial(PIN_RX, PIN_TX);
ESP8266VESC esp8266VESC = ESP8266VESC(softwareSerial);

const char ssid[] = WIFI_SSID;
const char pass[] = WIFI_PASSWORD;
const char mqtt_server[] = MQTT_SERVER;
const int mqtt_port = MQTT_PORT;

WiFiClient net;
MQTTClient mqtt(1024);
VESCValues vescValues;
bool vescValuesRead;

char key[256];
char payload[1024];


void connect()
{
#ifdef USE_SERIAL
    Serial.print("checking wifi...");
#endif

    while (WiFi.status() != WL_CONNECTED)
    {
#ifdef USE_SERIAL
        Serial.print(".");
#endif
        delay(100);
    }

#ifdef USE_SERIAL
    Serial.print("\nchecking mqtt...");
#endif

    while (!mqtt.connect(NODE_ID))
    {
#ifdef USE_SERIAL
        Serial.print(".");
#endif
        delay(1000);
    }
}

void setup()
{
#ifdef USE_SERIAL
    Serial.begin(9600);
    while (!Serial)
    {
    }
    delay(500);
    Serial.println("Device Started");
#endif
    softwareSerial.begin(VESC_BAUDRATE);
    delay(500);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    mqtt.begin(mqtt_server, mqtt_port, net);
    connect();
}

void loop()
{
    mqtt.loop();
    delay(10);
    connect();
    read_vesc();
    send_values();
    delay(SLEEP_MS);
}

void send_values()
{
    sprintf(key, "measurement/%s", NODE_ID);
    sprintf(payload, "{\"measurement\": \"nodes\", \"fields\": {\"temp_pcb\": %f, \"avg_mot_curr\": %f, \"avg_bat_curr\": %f, \"duty_cycle\": %f, \"rpm\": %d, \"bat_volt\": %f, \"mah_draw\": %f, \"mah_char\": %f}, \"tags\": {\"node_id\": \"%s\"}}",
            vescValues.temperaturePCB,
            vescValues.avgMotorCurrent,
            vescValues.avgInputCurrent,
            vescValues.dutyCycleNow,
            vescValues.rpm,
            vescValues.inputVoltage,
            vescValues.ampHours,
            vescValues.ampHoursCharged,
            NODE_ID);
    mqtt.publish(key, payload);
    if (SEND_MOSFET_TEMP)
    {
        sprintf(payload, "{\"measurement\": \"nodes\", \"fields\": {\"temp_mos1\": %f, \"temp_mos2\": %f, \"temp_mos3\": %f, \"temp_mos4\": %f, \"temp_mos5\": %f, \"temp_mos6\": %f}, \"tags\": {\"node_id\": \"%s\"}}",
                vescValues.temperatureMosfet1,
                vescValues.temperatureMosfet2,
                vescValues.temperatureMosfet3,
                vescValues.temperatureMosfet4,
                vescValues.temperatureMosfet5,
                vescValues.temperatureMosfet6,
                NODE_ID);
        mqtt.publish(key, payload);
    }
}

void read_vesc()
{
    vescValuesRead = esp8266VESC.getVESCValues(vescValues);
    if (vescValuesRead)
    {
#ifdef USE_SERIAL
        Serial.println("Temperature Mosfet #1 = " + String(vescValues.temperatureMosfet1) + " degree");
        Serial.println("Temperature Mosfet #2 = " + String(vescValues.temperatureMosfet2) + " degree");
        Serial.println("Temperature Mosfet #3 = " + String(vescValues.temperatureMosfet3) + " degree");
        Serial.println("Temperature Mosfet #4 = " + String(vescValues.temperatureMosfet4) + " degree");
        Serial.println("Temperature Mosfet #5 = " + String(vescValues.temperatureMosfet5) + " degree");
        Serial.println("Temperature Mosfet #6 = " + String(vescValues.temperatureMosfet6) + " degree");
        Serial.println("Temperature PCB = " + String(vescValues.temperaturePCB) + " degree");

        Serial.println("Average motor current = " + String(vescValues.avgMotorCurrent) + "A");
        Serial.println("Average battery current = " + String(vescValues.avgInputCurrent) + "A");
        Serial.println("Duty cycle = " + String(vescValues.dutyCycleNow) + "%");

        Serial.println("rpm = " + String(vescValues.rpm) + "rpm");
        Serial.println("Battery voltage = " + String(vescValues.inputVoltage) + "V");

        Serial.println("Drawn energy (mAh) = " + String(vescValues.ampHours) + "mAh");
        Serial.println("Charged energy (mAh) = " + String(vescValues.ampHoursCharged) + "mAh");

        Serial.println("Drawn energy (Wh) = " + String(vescValues.wattHours) + "Wh");
        Serial.println("Charged energy (Wh) = " + String(vescValues.wattHoursCharged) + "Wh");

        Serial.println("tachometer = " + String(vescValues.tachometer));
        Serial.println("tachometerAbs = " + String(vescValues.tachometerAbs));
#endif
    }
    else
    {
#ifdef USE_SERIAL
        Serial.println("The VESC values could not be read!");
#endif
    }

#ifdef USE_SERIAL
    Serial.println();
#endif
}