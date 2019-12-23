

#include <ESP8266WiFi.h>                //wifi连接库
#include <ArduinoJson.h>                 //JSON解析库
#include "SPI.h"                        //TFT驱动库
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341esp.h"
#include "SimpleDHT.h"                   //DHT11库

//发热片相关定义
#define jidianqi 5
//led控制
#define kai 4
#define ledx 16

//TFT屏相关定义
#define TFT_DC 2
#define TFT_CS 15
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//温湿模块
int DHTpin=0;
byte temperature=0;
byte humidity=0;
SimpleDHT11 dht11(DHTpin);

//wifi
const char* AP_SSID     = "Redmi";        
const char* AP_PSK = "88888888";       
 
WiFiClient client;                       //创建一个网络对象
 
void wifi_start_connect()              //连接WIFI
{
  WiFi.mode(WIFI_STA);                 //设置esp8266 工作模式 
  Serial.println("Connecting to ");    
  Serial.println(AP_SSID);
  WiFi.begin(AP_SSID, AP_PSK);         //连接wifi
  WiFi.setAutoConnect(true);
  while (WiFi.status()!= WL_CONNECTED) //wifi连接状态检测
        {  
         delay(500);
         Serial.print(".");
        }
  Serial.println("WiFi connected IP is");
  Serial.println(WiFi.localIP()); 
}
 
void parseUserDataweather(String content)  
{
 const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 210;
 DynamicJsonBuffer jsonBuffer(capacity);
 
 JsonObject& rootw = jsonBuffer.parseObject(content);
 
 JsonObject& results_0w = rootw["results"][0];
 
 JsonObject& results_0_location = results_0w["location"];
 const char* results_0_location_name = results_0_location["name"];
 JsonObject& results_0_now = results_0w["now"];
 const char* results_0_now_text = results_0_now["text"];
 const char* results_0_now_temperature = results_0_now["temperature"];

tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(50,0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  tft.println("Hello!");
  tft.setCursor(0, 30);
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.print("Current City:");
  tft.println(results_0_location_name);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(2);
  tft.print("Current temperature: ");
  tft.println(results_0_now_temperature);
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(2);
  tft.print("condition: ");
  tft.println(results_0_now_text);
}
void parseUserDatalife(String content)
{
   const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 210;
   DynamicJsonBuffer jsonBuffer(1000);
 
 JsonObject& rootl = jsonBuffer.parseObject(content); 
 
 JsonObject& results_0l = rootl["results"][0];
 JsonObject& results_0_suggestion=results_0l["suggestion"];
 JsonObject& results_0_dressingall = results_0_suggestion["dressing"];
 const char* results_0_dress = results_0_dressingall["brief"];
 JsonObject& results_0_fluall = results_0_suggestion["flu"];
 const char* results_0_flu=results_0_fluall["brief"];
 JsonObject& results_0_sportall = results_0_suggestion["sport"];
 const char* results_0_sport = results_0_sportall["brief"];

Serial.println(results_0_dress);
  tft.setCursor(0, 90);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  tft.print("dress:");
  tft.println(results_0_dress);
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.print("flu:");
  tft.println(results_0_flu);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(2);
  tft.print("sport");
  tft.println(results_0_sport);
  tft.println();

}

void setup() 
{
 Serial.begin(115200); 
 Serial.println("print ok");
 //wifi初始设置
 wifi_start_connect(); 
 client.setTimeout(5000);//设置服务器连接超时时间
//tft初始设置 
 tft.begin();
 pinMode(jidianqi,OUTPUT);
 pinMode(kai,INPUT);
 pinMode(ledx,OUTPUT);
//TFT测试
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
  Serial.println(F("Done!"));
}
 
void loop() 
{          
  if(digitalRead(kai)==1)
  {
      digitalWrite(ledx,LOW);
  }else
  {
    digitalWrite(ledx,HIGH);
  }
  tft.setRotation(1);  
  //请求天气数据                
  if(client.connect("api.seniverse.com", 80)==1)              //连接服务器并判断是否连接成功       
  {                                           
    client.print("GET /v3/weather/now.json?key=SjBu7CM9jRJeJRAUA&location=beijing&language=en&unit=c HTTP/1.1\r\n"); //利用心知天气获取天气信息       
    client.print("Host:api.seniverse.com\r\n");
    client.print("Accept-Language:zh-cn\r\n");
    client.print("Connection:close\r\n\r\n"); 
                 
    String status_codeweather = client.readStringUntil('\r');        //读取GET数据，服务器返回的状态码，若成功则返回状态码200
    Serial.println(status_codeweather);
      
     if(client.find("\r\n\r\n")==1)                            //跳过返回的数据头，直接读取后面的JSON数据，
     {
      String json_from_serverweather=client.readStringUntil('\n');  //读取返回的JSON数据
      Serial.println(json_from_serverweather);
      parseUserDataweather(json_from_serverweather);                      //将读取的JSON数据，传送到JSON解析函数中进行显示。
     }
  }
  else                                        
  { 
    Serial.println("connection failed this time");
    delay(5000);                                            //请求失败等5秒
  }      
   client.stop();
   delay(2000);
    //请求生活指数数据
   if(client.connect("api.seniverse.com", 80)==1)              //连接服务器并判断是否连接成功      
   {                                          
      client.print("GET /v3/life/suggestion.json?key=SjBu7CM9jRJeJRAUA&location=beijing&language=en HTTP/1.1\r\n");         
      client.print("Host:api.seniverse.com\r\n");
      client.print("Accept-Language:en\r\n");
      client.print("Connection:close\r\n\r\n"); //向心知天气的服务器发送请求。
 
                 
      String status_codelife = client.readStringUntil('\r');        //读取GET数据，服务器返回的状态码，若成功则返回状态码200
      Serial.println(status_codelife);
      
   if(client.find("\r\n\r\n")==1)                            //跳过返回的数据头，直接读取后面的JSON数据，
     {
      String json_from_serverlife=client.readStringUntil('\n');  //读取返回的JSON数据
      Serial.println(json_from_serverlife);
      parseUserDatalife(json_from_serverlife);                      //将读取的JSON数据，传送到JSON解析函数中进行显示。
     }
  }
else                                        
  { 
   Serial.println("connection failed this time");
   delay(5000);                                            //请求失败等5秒
  }   
  int i;
 // for(i=0;i<5;i++)
  delay(5000);      
  dht11x(); 
         
}
void dht11x(void)
{
  int err = SimpleDHTErrSuccess;
  int dd=dht11.read(&temperature, &humidity, NULL);
  if (dd!= SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err=");
    Serial.println(err);
    delay(1000);
    return;
  }  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");

  if(temperature<28||humidity>80)           //根据温湿度情况判断是否启动发热装置
    {
        digitalWrite(jidianqi,HIGH);
    }else
    {
        digitalWrite(jidianqi,LOW);
    }
  delay(1500);
}
