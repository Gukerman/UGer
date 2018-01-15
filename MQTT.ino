/*
    Для использования вкладки требуется добавить в заголовке скетча следуюший код
  #include <PubSubClient.h>
  WiFiClient wclient;
  PubSubClient client(wclient);
*/

void callback(const MQTT::Publish& sub) { //Есть запись в топике
  Serial.print("Get data from subscribed topic ");
  Serial.print(sub.topic());  // Название топика
  Serial.print(" => ");
  Serial.println(sub.payload_string()); // Данные из топика

  if  (sub.topic() == "/ESP8266/CONTROL/GPIO/2")  {
    if  (sub.payload_string() == "ON")  {
      digitalWrite(pinrele, LOW);
    }
    if  (sub.payload_string() == "OFF")  {
      digitalWrite(pinrele, HIGH);
    }
  }

  // echo
  MQTT::Publish newpub(pubTopic, sub.payload(), sub.payload_len());
  client.publish(newpub);
}

void initMQTT() {
  //  modulesReg("mqtt");
  MQTT_Pablush();
}

void MQTT_Pablush() {

  if ((mqttServer != "")) {
    if (WiFi.status() == WL_CONNECTED) {

      client.set_callback(callback);

      if (client.connect(MQTT::Connect(chipID).set_auth(mqttUser, mqttPass)))
      {
        Serial.println("Connected to MQTT broker");
        client.subscribe(controlTopic);
      } else {
        Serial.println("Will reset and try again...");
        // abort();
      }
    }
  }
}

/*
  if ((mqttServer != "")) {
    client.set_server(mqttServer, mqttPort.toInt());
    // подключаемся к MQTT серверу
    if (WiFi.status() == WL_CONNECTED) {
      if (!client.connected()) {
        if (client.connect(MQTT::Connect(chipID)
                           .set_auth(mqttUser, mqttPass))) {
                             Serial.print("MQTT_Pablush - ");
          Serial.println(mqttServer);

          client.set_callback(callback);
          //client.subscribe(prefix);  // Для приема получения HELLOW и подтверждения связи
          client.subscribe("/+/+/control"); // Подписываемся на топики control
          //client.subscribe("/" + chipID + "/RELE_1"); // подписываемся по топик с данными для светодиода
  //         loadnWidgets();

  Serial.print("MQTT - ");
  Serial.println(mqttServer);
        } else {
        }
      }
    }
  }
*/


void  handleMQTT() {

  if (client.connected()) client.loop();
  else {
    MQTT_Pablush();
    //Serial.println("MQTT");
  }

  if (mySwitch.available())
  {
    pubTopic = "/smart_house/" + mqttUser + "/" + SSDP_Name + "/";

    int ReceivedValue = mySwitch.getReceivedValue();
    gerkonend = ReceivedValue;
    saveConfigSetup();
    
    if  (ReceivedValue == gerkonopen.toInt())    publishMQTT(pubTopic, "open");
    if  (ReceivedValue == gerkonclose.toInt())   publishMQTT(pubTopic, "close");
    if  (ReceivedValue == gerkonbat.toInt())     publishMQTT(pubTopic, "bat");
    if  (ReceivedValue == gerkonalarm.toInt())   publishMQTT(pubTopic,  "alarm");

    mySwitch.resetAvailable();
  }
}

void  publishMQTT(String pubTopic, String cTopic)
{
  if ( mult != cTopic)
  {
    //Публикуем ReceivedValue в топике
    if (client.publish(pubTopic,  cTopic)) {
      Serial.print(" Publish ");
      Serial.println(cTopic);
    }
    else {
      Serial.println(" Publish failed");
    }
    mult = cTopic;
  }
}

//Установка параметров  http://192.168.0.101/mqtt?server=m13.cloudmqtt.com&port=15535&user=cxluynva&pass=4cje5WEkzqvR
void handle_Set_MQTT() {              //

  mqttServer = HTTP.arg("mqttServer");         // Получаем значение mqttServer из запроса сохраняем в глобальной переменной
  mqttPort = HTTP.arg("mqttPort");         // Получаем значение mqttPort из запроса сохраняем в глобальной переменной
  mqttUser = HTTP.arg("mqttUser");         // Получаем значение mqttUser из запроса сохраняем в глобальной переменной
  mqttPass = HTTP.arg("mqttPass");         // Получаем значение mqttPass из запроса сохраняем в глобальной переменной
  saveConfigSetup();                         // Функция сохранения данных во Flash
  MQTT_Pablush();
  HTTP.send(200, "text/plain", "OK");   // отправляем ответ о выполнении

  Serial.print("mqttServer - "); Serial.println(mqttServer);
  Serial.print("mqttPort - "); Serial.println(mqttPort);
  Serial.print("mqttUser - "); Serial.println(mqttUser);
  Serial.print("mqttPass - "); Serial.println(mqttPass);
}

