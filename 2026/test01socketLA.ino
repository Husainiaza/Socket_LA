/*-------------- Socket Learn And Apply ------------------------
/*------------------------------------------------------------------
  ===================  Seksyen 1 - HEADER        ===================
  ------------------------------------------------------------------*/

//-------------------------------------------------------------------
//=  A. - Library  include and define  yang diperlukan              =
//-------------------------------------------------------------------

#include <WiFi.h>
#include <MQTT.h>
#include <Wire.h>
#include <SPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//--------------------------------------------------------------------------
//-----b. - ISTIHAR Sambungan nama Pin dengan Nombor Fizikal PIN ----
//--------------------------------------------------------------------------

//----ISTIHAR GPIO PIN ---------------------------------------------------------------------------------

//---=on board------
#define oneWireBus 4  //--SENSOR SUHU DS18B20
#define Relay01 27   //relay 1
#define Relay02 26   // relay 2
#define buzzer  25     // Buzzer
#define ldrsensor  34

#define SENSOR_PIN  15 // ESP32 pin GIOP21 connected to DS18B20 sensor's DQ pin
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
//----------------------------------- end -------------------------------------------------------------
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// ~~~~~~~~~~~~~~~~~~~~ PENTING ~~~~~~~~~~~~~~~~~~~~~~~~
// ------ Sila edit nama atau ID ikut keperluan --------
#define Client_Id   "socketLAkolejKomunitiSeberangJaya0001"
#define NamaBroker  "broker.emqx.io"

//#define namaSSID    "Haza@unifi";
//#define SSIDpwd     "0135335045";

#define namaSSID    "IoT";
#define SSIDpwd     "iot@kksj2023";
// ~~~~~~~~~~~~~~~~~~~  TAMMAT   ~~~~~~~~~~~~~~~~~~~~~~~

//-----c.  - ISTIHAR  constant dan pembolehubah------------------------------
//---Penetapan nama Pembolehubah yg diumpukkan kepada satu nilai awal  --
const char ssid[] = namaSSID;
const char pass[] = SSIDpwd;

//------ ISTIHAR Pemboleh ubah dengan jenis Data yang digunakan---
unsigned long lastMillis = 0;
//---- PEMBOLEHUBAH DATA---------------------------------------------------------------------------------
float sensorAnalog1;
float sensorAnalog2;
float temperatureC;
char varPotString[8];
float dataMoisture;
long lastMsg = 0;
char msg[50];
int value = 0;
const int numSamples = 30;
//----------------------------------- end -------------------------------------------------------------


//-----d. - Cipta Objek dari Librari berkaitan------------------ ----
//--------------------------------------------------------------------------

WiFiClient net;
MQTTClient client;

//Istihar objek bagi Module OLED Display - SSD1306 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Istihar objek bagi Module Sersor Suhu Dallas 18B20
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

//DHT dht(DHTPIN, DHTTYPE);

//##################  Seksyen 1 - TAMAT #############################
//--------------FUNCTION----------------------------

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(Client_Id)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  client.subscribe("socketLA/socket01");
  client.subscribe("socketLA/socket02");
  client.subscribe("socketLA/buzzer");
  // -- tambah subcribe disini ---
 
}

void messageReceived(String &topic, String &payload) {
  //Serial.println("incoming: " + topic + " - " + payload);
   Serial.println(payload);
  
  //==============================================================

  //==========================Relay Control Socket 1 ========================
  if (String(topic) == "socketLA/socket01") {
    Serial.print("Changing output to ");
    if(payload == "on"){
      Serial.println("on");
      digitalWrite(Relay01,HIGH);
     // client.publish("fishfeederPTSB03/relay1/status", "RELAY 1 ON");
          
    }
    else if(payload == "off"){
      Serial.println("off");
      digitalWrite(Relay01,LOW);
     // client.publish("fishfeederPTSB03/relay1", "RELAY 1 OFF");
      
    }
  }

 if (String(topic) == "socketLA/socket02") {
    Serial.print("Changing output to ");
    if(payload == "on"){
      Serial.println("on");
      digitalWrite(Relay02,HIGH);
    //  client.publish("fishfeederPTSB03/motorfeeder/status", "RELAY 2 ON");
    }
    else if(payload == "off"){
      Serial.println("off");
      digitalWrite(Relay02,LOW);
    //   client.publish("fishfeederPTSB03/motorfeeder/status", "RELAY 2 OFF");
    }
  }

  if (String(topic) == "socketLA/buzzer") {
    Serial.print("Changing output to ");
    if(payload == "on"){
      Serial.println("on");
      digitalWrite(buzzer,HIGH);
    //  client.publish("fishfeederPTSB03/buzzer/status", "RELAY 2 ON");
    }
    else if(payload == "off"){
      Serial.println("off");
      digitalWrite(buzzer,LOW);
    //   client.publish("fishfeederPTSB03PTSB/buzzer/status", "RELAY 2 OFF");
    }
  }

  //   ----Tulis Kod Kawalan ( subsribe here ) -------

}


//###################################################
//==================  Seksyen 2 - Fungsi Setup ======================
//-------------------------------------------------------------------

void setup() {

  pinMode (Relay01,OUTPUT);
  pinMode (Relay02,OUTPUT);
  pinMode (buzzer,OUTPUT);
  
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  WiFi.begin(ssid, pass);
  client.begin(NamaBroker, net);
  client.onMessage(messageReceived);
  delay(100);  

  sensors.begin();

  delay(2000);
  display.clearDisplay();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("  IoT Socket Control");
  display.println("  ==================");
//  display.setCursor(40, 20);
  display.display();
  delay(500);
    
}
//##################  Seksyen 2 - TAMAT #############################


//==============  Seksyen 3 - Fungsi Utama (LOOP) ===================
//-------------------------------------------------------------------
void loop() {

 client.loop();
 delay(10);  // <- fixes some issues with WiFi stability

  if (!client.connected()) {
    connect();
  }

// publish a message roughly every second.
  if (millis() - lastMillis > 2000) {
  lastMillis = millis();

    //------SENSOR LDR-------------------------------------------------------------
    //----------- Sensor Moisture ----------------------------------

  //int dataLDR = analogRead(ldrsensor);  
  int stableLightValue = getStableLDRReadings();
  Serial.print("Stable LDR Value: ");
  Serial.println(stableLightValue);
  //Serial.print("LDR Value : ");
  //Serial.println(dataLDR);
  client.publish("socketLA/ldr", String(stableLightValue));  
    //----------------------------------------------------------------------------
    //------SENSOR 2--------------------------------------------------------------
    

  }
 //-----------end ----------------------------------
  
}

//##################  Seksyen 3 - TAMAT #############################

int getStableLDRReadings() {
  long sum = 0;

  for (int i = 0; i < numSamples; i++) {
    sum += analogRead(ldrsensor);
    delay(5); // Small delay between individual samples
  }

  return (int)(sum / numSamples);
}
