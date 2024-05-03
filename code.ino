#define BLYNK_TEMPLATE_ID "TMPL6MZJYJaxB"
#define BLYNK_TEMPLATE_NAME "Air Detection System"
#define BLYNK_AUTH_TOKEN "L6IrxGym71G5eU0dQVLvNr8tVZsc0eTb"
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

int pinValue2=0;
int flag2=0;

//Pin Configeration
int SoilSensor = 34;
int WaterSensor1 = 23;
const int oneWireBus = 2;
volatile long pulse1;
volatile long pulse2;
float volume1;
float tot_volume;
unsigned long lastTime;
float tempC; // temperature in Celsius
float tempF; // temperature in Fahrenheit
int flag=0;
#define RELAY_PIN  4  // ESP32 pin GIOP4, which connects to the IN pin of relay

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature DS18B20(&oneWire);

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "LOL 4G";
char pass[] = "31343@$MR";

BlynkTimer timer;


BLYNK_WRITE(V0)
{
 // Set incoming value from pin V0 to a variable
 // int value = param.asInt();

 // Update state
 // Blynk.virtualWrite(V1, value);

}

BLYNK_WRITE(V4)
{
  pinValue2 = param.asInt();
if (pinValue2==1){
 digitalWrite(RELAY_PIN, HIGH);
 // Blynk.virtualWrite(V14,message2);
 }
 if (pinValue2==0){
 digitalWrite(RELAY_PIN, LOW);
// Blynk.virtualWrite(V14,message6); 
}
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{

//SoilMoisture
int SoilMoisture = analogRead(SoilSensor);
int SoilMoisture2=SoilMoisture/5;
Blynk.virtualWrite(V0,SoilMoisture2);
Serial.println(SoilMoisture2);

if (SoilMoisture2<700||pinValue2==1) {

 //do stuff if the condition is true
  digitalWrite(RELAY_PIN, HIGH); // open valve 5 seconds
  if (flag2==0){
   flag2=10; 
   flag=0;
  Blynk.logEvent("solenoid_valve_status","Water Detected Valve Opened");}
} 

if (SoilMoisture2 > 701||pinValue2==0) {
  
  digitalWrite(RELAY_PIN, LOW); // open valve 5 second 
  if(flag==0){
     flag=40;
     flag2=0;
  Blynk.logEvent("solenoid_valve_status","Air Detected Valve Closed"); }
}

//WaterFlow
volume1 = 2.663 * pulse1 / 1000 * 30;
tot_volume = 2.663 * pulse2/1000;
  if (millis() - lastTime > 2000) {
    pulse1 = 0;
    lastTime = millis();
  }
  Serial.print(volume1);
  Blynk.virtualWrite(V1,volume1);
  Serial.println(" L/m");
  Serial.print(tot_volume);
  Blynk.virtualWrite(V3,tot_volume);
  Serial.println(" L");
  delay(500);

//DS18B20
DS18B20.requestTemperatures();       // send the command to get temperatures
  tempC = DS18B20.getTempCByIndex(0);  // read temperature in °C
  tempF = tempC * 9 / 5 + 32; // convert °C to °F
  Blynk.virtualWrite(V2,tempC);
  Serial.print("Temperature: ");
  Serial.print(tempC);    // print the temperature in °C
  Serial.println("°C");

}

void setup() {

  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(RELAY_PIN, OUTPUT);
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
  pinMode(WaterSensor1, INPUT);
  attachInterrupt(digitalPinToInterrupt(WaterSensor1), increase, RISING);
  DS18B20.begin();  
 
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  timer.run();
}

void increase() {
  pulse1++;
  pulse2++;
}