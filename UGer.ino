/*


*/
#include <WiFiClient.h>
#include <ESP8266mDNS.h> 

#include <ESP8266WiFi.h>        //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step1-wifi
#include <ESP8266WebServer.h>   //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step2-webserver
#include <ESP8266SSDP.h>        //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step3-ssdp
#include <FS.h>                 //Содержится в пакете. Видео с уроком http://esp8266-arduinoide.ru/step4-fswebserver
//                    ПЕРЕДАЧА ДАННЫХ НА WEB СТРАНИЦУ. Видео с уроком http://esp8266-arduinoide.ru/step5-datapages/
//                    ПЕРЕДАЧА ДАННЫХ С WEB СТРАНИЦЫ.  Видео с уроком http://esp8266-arduinoide.ru/step6-datasend/
#include <ArduinoJson.h>        //Установить из менеджера библиотек.
//                    ЗАПИСЬ И ЧТЕНИЕ ПАРАМЕТРОВ КОНФИГУРАЦИИ В ФАЙЛ. Видео с уроком http://esp8266-arduinoide.ru/step7-fileconfig/
#include <ESP8266HTTPUpdateServer.h>  //Содержится в пакете.


#include <RCSwitch.h>

#include "PageScan.h"

RCSwitch mySwitch = RCSwitch();

// MQTT
#include <MQTT.h>
#include <PubSubClient.h>

WiFiClient espClient;
IPAddress MQTTserver(77, 79, 191, 226); // mosquitto address
PubSubClient client(espClient, MQTTserver);
char* mqtt_user = "User";
char* mqtt_pass = "Pass";
String mqtt_client = "ESP8266";
String mult = "";        // переменная хронения предыдущего сообщания (убирает дубли публикации, но это не точно)
  
// Объект для обнавления с web страницы 
ESP8266HTTPUpdateServer httpUpdater;

// Web интерфейс для устройства
ESP8266WebServer HTTP;

// Для файловой системы
File fsUploadFile;

// Определяем переменные wifi
String _ssid     = ""; // Для хранения SSID
String _password = ""; // Для хранения пароля сети
String _ssidAP = "";   // SSID AP точки доступа
String _passwordAP = ""; // пароль точки доступа
String SSDP_Name = ""; // Имя SSDP
int timezone = 5;               // часовой пояс GTM

String gerkonopen = "";       // код геркона отрыт 2866734
String gerkonclose = "";       // код геркона закрыт 2866727
String gerkonbat = "";       // код геркона батарейка  2866731
String gerkonalarm = "";       // код геркона авария 2866731


String  mqttServer = "";   // 77.79.191.226
String  mqttPort =  "";     // 
String  mqttUser =  "";     // 
String  mqttPass =  "";     // 

String pubTopic = "";       //smart_house/<номер_договора>/<код_устройства>/<статус_устройства>​ . Например: smart_house/72357995ASU/Door1/OPEN
String cTopic = "";       // состояние геркона для публикации, например OPEN
String controlTopic = "/ESP8266/CONTROL/#";

boolean secTest = true;
//StringCommand sCmd;     // The demo StringCommand object
String command = "";

String chipID = "";
String jsonConfig = "{}";           // Здесь все статусы 
//String configJson = "{}";            // разобраться одну упразнить 

String configOptions = "{}";         // Здесь опции для всех страниц
String configSetup = "{}";           // Здесь данные для setup


int port = 80;
int pin433 = 13; 

int pinrele = 5; 
int pinled = 2; 

int ReceivedValue = 0;    //Переменная хранит статус последнего полученного кода с 433М
 
void setup() {
  pinMode (pinrele, OUTPUT);
  pinMode (pinled, OUTPUT);

  digitalWrite(pinrele,LOW);
  digitalWrite(pinled,LOW);

  HTTP = ESP8266WebServer (port);
  Serial.begin(115200);
  Serial.println("");


  chipID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );

  //Запускаем файловую систему
  Serial.println("Start 1 FS");
  FS_init();         // Включаем работу с файловой системой

  // ----------------- начинаем загрузку
//  configSetup = readFile("config.save.json", 4096);
/*  configSetup = jsonWrite(configSetup, "time", "00:00:00");
  configJson = jsonWrite(configJson, "setIndex", jsonRead(configSetup, "setIndex"));
  configOptions = jsonWrite(configOptions, "setIndex", jsonRead(configSetup, "setIndex"));
  configOptions = jsonWrite(configOptions, "SSDP", jsonRead(configSetup, "SSDP"));
  String configs = jsonRead(configSetup, "configs");
  configs.toLowerCase();
 
   // ----------- запускаем необходимые всегда модули
     // ----------- Выполняем запуск кофигурации
  configSetup = jsonWrite(configSetup, "mac", WiFi.macAddress().c_str());
  configSetup = jsonWrite(configSetup, "ip", WiFi.localIP().toString());
*/
  
  Serial.println("Step 2 FileConfig");
  loadConfig();
  Serial.println("Step 3 WIFI");
  //Запускаем WIFI
  WIFIinit();
  Serial.println("Step 4 Time");
  // Получаем время из сети
  Time_init();
  //Настраиваем и запускаем SSDP интерфейс
  Serial.println("Step 5 SSDP");
  initSSDP();
  //Настраиваем и запускаем HTTP интерфейс
  Serial.println("Step 6 WebServer");
  HTTP_init();
  mySwitch.enableReceive(pin433);  // Receiver on pin #13
  Serial.println("Step 7 RCSwitch");

  initMQTT();
  Serial.println("Step 8 MQTT");

}

void loop() {
  
  handleMQTT();          
  HTTP.handleClient();
  delay(1);
}
