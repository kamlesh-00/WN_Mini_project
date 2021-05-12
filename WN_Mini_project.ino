#include <LiquidCrystal.h>
#include <IRremote.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

LiquidCrystal lcd(12,10,7,6,5,4);

int RECV_PIN = 11;
IRrecv irrecv(RECV_PIN);
decode_results results;

int app1 = 3;
int app2 = 2;
int status[] = {0,0,0,0};

const char* ssid = "yourNetworkName";
const char* password =  "yourNetworkPassword";

void sendInformation(){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;

    http.begin("here_api_url_where_post_will_happen");  //Specify destination for HTTP request
    http.addHeader("Content-Type", "text/json");
    String data = "{\"fan\":"+status[1]+",\"bulb\":"+status[2]+"}"
    int httpResponseCode = http.POST(data);
    Serial.println(httpResponseCode);
    Serial.println(http.getString());
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  sendInformation();
  
  lcd.begin(16,2);
  irrecv.enableIRIn();
  
  pinMode(app1, OUTPUT);
  pinMode(app2, OUTPUT);
  
  lcd.setCursor(0,0);
  lcd.print("(1) Fan : OFF");
  lcd.setCursor(0,1);
  lcd.print("(2) Bulb : OFF");
}

void loop(){
  if (irrecv.decode(&results)){
    unsigned int value = results.value;
    switch(value){
       case 2295:
          if(status[1] == 1) {
            digitalWrite(app1, LOW);
            status[1] = 0;
            lcd.setCursor(10,0);
            lcd.print("OFF");
          }
          else{
              digitalWrite(app1, HIGH);
              status[1] = 1;
              lcd.setCursor(10,0);
              lcd.print("ON ");
          }
          sendInformation();
          break;
       case 34935:
          if(status[2] == 1) {
            digitalWrite(app2, LOW);
            status[2] = 0;
            lcd.setCursor(11,1);
            lcd.print("OFF");
          } 
          else {
            digitalWrite(app2, HIGH);
            status[2] = 1;
            lcd.setCursor(11,1);
            lcd.print("ON ");
          }
          sendInformation();
          break;
              
       case 255:
         if(status[1] == 1 && status[2] == 1) {
            digitalWrite(app1, LOW);      
            status[1] = 0;              
            lcd.setCursor(10,0);      
            lcd.print("OFF");

            digitalWrite(app2, LOW);
            status[2] = 0;
            lcd.setCursor(11,1);      
            lcd.print("OFF");
         }
         else{
            digitalWrite(app1, HIGH);   
            status[1] = 1;            
            lcd.setCursor(10,0);    
            lcd.print("ON ");

            digitalWrite(app2, HIGH);
            status[2] = 1;
            lcd.setCursor(11,1);      
            lcd.print("ON ");
         }
         sendInformation();
    }
    Serial.println(value);      
    irrecv.resume();
  }
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
 
    http.begin("here_url_of_new_status_api");
    int httpCode = http.GET();
    if (httpCode > 0) {
 
      String payload = http.getString();
      String fan_and_bulb_on_status = "{\"fan\":\"on\",\"bulb\":\"on\"}";
      String fan_on_and_bulb_off_status = "{\"fan\":\"on\",\"bulb\":\"off\"}";
      String fan_off_and_bulb_on_status = "{\"fan\":\"off\",\"bulb\":\"on\"}";
      String fan_off_and_bulb_off_status = "{\"fan\":\"off\",\"bulb\":\"off\"}";
      if(payload==fan_and_bulb_on_status){
        digitalWrite(app1, HIGH);
        digitalWrite(app2, HIGH);
        status[1]=1;
        status[2]=1;
      }else if(payload==fan_on_and_bulb_off_status){
        digitalWrite(app1, HIGH);
        digitalWrite(app2, LOW);
        status[1]=1;
        status[2]=0;
      }else if(payload==fan_off_and_bulb_on_status){
        digitalWrite(app1, LOW);
        digitalWrite(app2, HIGH);
        status[1]=0;
        status[2]=1;
      }else{
        digitalWrite(app1, LOW);
        digitalWrite(app2, LOW);
        status[1]=0;
        status[2]=0;
      }
      Serial.println(payload);
    }
    http.end();
  }
}
