#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

extern "C" {
  #include "gpio.h"
}

extern "C" {
  #include "user_interface.h"
}

#define CONNECT_TIMEOUT 25 // / 2

const char* ssid = "FoxNet 2.4";
const char* password = "98345458";

//Your Domain name with URL path or IP address with path
String serverName = "http://solarmonitor20.000webhostapp.com/";

void initWifi() {
  WiFi.begin(ssid, password);
  bool success = true;
  int i = 0; 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    
    if(i >= CONNECT_TIMEOUT)
    {
      Serial.print("Error 1^^");
      success = false;
      break;
    }
    i++;
  }
   
  if(success == true)
    Serial.print("Connected^^");
}

String sendRequest(String resourcePath, bool needPayload)
{
  WiFiClient client;
  HTTPClient http;

  String serverPath = serverName + resourcePath;
  
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverPath.c_str());
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();

  String response = "";
  if (httpResponseCode>0) {
    if(!needPayload)
      response += ("OK^^");
    else
    {
      response += http.getString() + "^^";
    }
  }
  else {
    response += "Error 3^^";
  }
  // Free resources
  http.end();  
  return response;
}

//void light_sleep() {
//  gpio_pin_wakeup_enable(GPIO_ID_PIN(2), GPIO_PIN_INTR_LOLEVEL);
//  wifi_set_opmode(NULL_MODE);
//  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
//  wifi_fpm_open();
//  wifi_fpm_do_sleep(0xFFFFFFF);
//  delay(1000);
//
//  Serial.print("Wakeup^^");
//  initWifi();
//  wifi_set_sleep_type(NONE_SLEEP_T);
//}

void light_sleep()
{
   wifi_station_disconnect();
   gpio_pin_wakeup_enable(GPIO_ID_PIN(2), GPIO_PIN_INTR_LOLEVEL); // GPIO_ID_PIN(2) corresponds to GPIO2 on ESP8266-01 , GPIO_PIN_INTR_LOLEVEL for a logic low, can also do other interrupts, see gpio.h above
   wifi_set_opmode_current(NULL_MODE);
   wifi_fpm_set_sleep_type(LIGHT_SLEEP_T); // set sleep type, the above    posters wifi_set_sleep_type() didnt seem to work for me although it did let me compile and upload with no errors 
   wifi_fpm_open(); // Enables force sleep
   wifi_fpm_do_sleep(0xFFFFFFF); // Sleep for longest possible time
   delay(1000);
   
   // on Wakeup
   Serial.print("Wakeup^^");
   initWifi();
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  gpio_init(); // Initilise GPIO pins
  //pinMode(2, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  initWifi();
}

void loop() {
  String command = "";
  String payload = "";
  
  if (Serial.available() > 0) {
    // read the incoming string:
    command = Serial.readString();
  
    payload = command.substring(2);
    command = command.substring(0, 1);
  }

  if(command == "1")
  {
    Serial.print(sendRequest("\getRealTime.php", true));
  }
  else if(command == "2")
  {
    Serial.print(sendRequest("loadData.php?data=" + payload, false));
  }
  else if(command == "3")
  {
    
  }
  else if(command == "4")
  {
    Serial.print("OK^^");
    light_sleep();
  }
}
