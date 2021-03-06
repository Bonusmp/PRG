
void ICACHE_RAM_ATTR ISRoutine ();

//#define BLYNK_PRINT Serial           // Uncomment for debugging 

#include "settings_WCWU.h"           
#include "secret.h"                  
      
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
//#include <ESP8266mDNS.h>  // For OTA with ESP8266
//#include <WiFiUdp.h>  // For OTA
//#include <ArduinoOTA.h>  // For OTA


BlynkTimer timer;

volatile long pulseCount = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
float totalLitres;
float totalLitresold;
float totalLitresoldh1;
float totalLitresoldh2;
float lh;
float lh2;

unsigned long oldTime;

volatile long pulseCount2 = 0;
float flowRate2;
unsigned int flowMilliLitres2;
unsigned long totalMilliLitres2;
float totalLitres2;
float totalLitresold2;
unsigned long oldTime2;
int wifiStrength; //used to find wifi signal strenght

WidgetLED led1(V0);
int wskrunAr = 0;
void miganievLED() //miganie vLED dla kontroli połączenia z modułem
{
  wskrunAr = !wskrunAr ;
  if (wskrunAr == 0)led1.off(); else led1.on();
}



BLYNK_CONNECTED() { // runs once at device startup, once connected to server.

  Blynk.syncVirtual(VPIN_TOTAL_LITERS); //gets last know value of V1 virtual pin
Blynk.syncVirtual(VPIN_TOTAL_LITERS2);




}


// Restores last know value of V1 virtual pin which we got it from blynk server
BLYNK_WRITE(VPIN_TOTAL_LITERS)
{
  totalLitresold = param.asFloat();

}

BLYNK_WRITE(VPIN_TOTAL_LITERS2)
{
  totalLitresold2 = param.asFloat();

}
BLYNK_WRITE(VPIN_TOTAL_LITERSh1)
{
  totalLitresoldh1 = param.asFloat();
}


  BLYNK_WRITE(VPIN_TOTAL_LITERSh2)
{
  totalLitresoldh2 = param.asFloat();

}
BLYNK_WRITE(VPIN_RESET) {  // reset all data with button in PUSH mode on virtual pin V4
  int resetdata = param.asInt();
  if (resetdata == 0) {
    Serial.println("Clearing Data");
    Blynk.virtualWrite(VPIN_TOTAL_LITERS, 0);
    Blynk.virtualWrite(VPIN_FLOW_RATE, 0);
    flowRate = 0;
    flowMilliLitres = 0;
    //totalMilliLitres = 0;
    //totalLitres = 0;
    //totalLitresold = 0;
  }
}
BLYNK_WRITE(VPIN_RESET2) {  // reset all data with button in PUSH mode on virtual pin V4
  int resetdata2 = param.asInt();
  if (resetdata2 == 0) {
    Serial.println("Clearing Data");
    Blynk.virtualWrite(VPIN_TOTAL_LITERS2, 0);
    Blynk.virtualWrite(VPIN_FLOW_RATE2, 0);
    flowRate2 = 0;
    flowMilliLitres2 = 0;
    //totalMilliLitres2 = 0;
    //totalLitres2 = 0;
    //totalLitresold2 = 0;
  }
}


void pulseCounter()
{
  pulseCount++;
}

void pulseCounter2()
{
  pulseCount2++;
}

void godzinowe(){
Blynk.syncVirtual(VPIN_TOTAL_lh);
Blynk.syncVirtual(VPIN_TOTAL_LITERSh1);
lh = totalLitresold - totalLitresoldh1;
Blynk.virtualWrite(VPIN_TOTAL_lh, lh);
totalLitresoldh1 = totalLitresold;
Blynk.virtualWrite(VPIN_TOTAL_LITERSh1, totalLitresoldh1);
}
void godzinowe2(){
Blynk.syncVirtual(VPIN_TOTAL_lh2);
Blynk.syncVirtual(VPIN_TOTAL_LITERSh2);
lh2 = totalLitresold2 - totalLitresoldh2;
Blynk.virtualWrite(VPIN_TOTAL_lh2, lh2);
totalLitresoldh2 = totalLitresold2;
Blynk.virtualWrite(VPIN_TOTAL_LITERSh2, totalLitresoldh2);
}

void flow()
{

  if ((millis() - oldTime) > 1000)   // Only process counters once per second
  {
    detachInterrupt(PULSE_PIN);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / FLOW_CALIBRATION;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    totalLitres = totalLitresold + totalMilliLitres * 0.001;
    unsigned int frac;

    // Print the flow rate for this second in liters / minute
    Serial.print("flowrate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable

    Serial.print(".");             // Print the decimal point
    frac = (flowRate - int(flowRate)) * 10; // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    Serial.print(frac, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");

    Serial.print("  Current Liquid Flowing: ");  // Print the number of liters flowed in this second
    Serial.print(flowMilliLitres);
    Serial.print("mL/Sec");

    Serial.print("  Output Liquid Quantity: ");  // Print the cumulative total of liters flowed since starting
    Serial.print(totalLitres);
    Serial.println("L");

    pulseCount = 0;  // Reset the pulse counter so we can start incrementing again

    attachInterrupt(PULSE_PIN, pulseCounter, FALLING);    // Enable the interrupt again now that we've finished sending output
  }

if ((millis() - oldTime2) > 1000)   // Only process counters once per second
  {
    detachInterrupt(PULSE_PIN2);
    flowRate2 = ((1000.0 / (millis() - oldTime2)) * pulseCount2) / FLOW_CALIBRATION2;
    oldTime2 = millis();
    flowMilliLitres2 = (flowRate2 / 60) * 1000;
    totalMilliLitres2 += flowMilliLitres2;
    totalLitres2 = totalLitresold2 + totalMilliLitres2 * 0.001;
    unsigned int frac2;

    // Print the flow rate for this second in liters / minute
    Serial.print("flowrate2: ");
    Serial.print(int(flowRate2));  // Print the integer part of the variable

    Serial.print(".");             // Print the decimal point
    frac2 = (flowRate2 - int(flowRate2)) * 10; // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
    Serial.print(frac2, DEC) ;      // Print the fractional part of the variable
    Serial.print("L/min");

    Serial.print("  Current Liquid Flowing2: ");  // Print the number of liters flowed in this second
    Serial.print(flowMilliLitres2);
    Serial.print("mL/Sec");

    Serial.print("  Output Liquid Quantity2: ");  // Print the cumulative total of liters flowed since starting
    Serial.print(totalLitres2);
    Serial.println("L");

    pulseCount2 = 0;  // Reset the pulse counter so we can start incrementing again

    attachInterrupt(PULSE_PIN2, pulseCounter2, FALLING);    // Enable the interrupt again now that we've finished sending output
  }

}
void pumpcontrol(){
 
  if (flowRate >= 0.4) {
    digitalWrite(pumpPin, HIGH);   // turn off pump //s*
    Blynk.virtualWrite(V9, "ON");
     Serial.println("Pompa ON");
  }
  if (flowRate  < 0.4) {
    digitalWrite(pumpPin, LOW);   // turn on pump //s*
    Blynk.virtualWrite(V9, "OFF");
    Serial.println("Pompa OFF");
  
}
attachInterrupt(pumpPin, pumpcontrol, CHANGE);


}
void sendtoBlynk()  // In this function we are sending values to blynk server
{
  Blynk.virtualWrite(VPIN_TOTAL_LITERS, totalLitres);          // Total water consumption in liters (L)
  Blynk.virtualWrite(VPIN_FLOW_RATE, flowRate);            // Displays the flow rate for this second in liters / minute (L/min)
  //  Blynk.virtualWrite(VPIN_FLOW_RATE, flowMilliLitres);  // Displays the number of liters flowed in second (mL/Sec)

  Blynk.virtualWrite(VPIN_TOTAL_LITERS2, totalLitres2);          // Total water consumption in liters (L)
  Blynk.virtualWrite(VPIN_FLOW_RATE2, flowRate2);            // Displays the flow rate for this second in liters / minute (L/min)
  //  Blynk.virtualWrite(VPIN_FLOW_RATE2, flowMilliLitres);  // Displays the number of liters flowed in second (mL/Sec)

}

void wifiSignalStrength() { //function to calculate wifi signal strength

  wifiStrength = WiFi.RSSI(); //here we are getting wifi signal strength

  Blynk.virtualWrite(VPIN_WIFI_SIGNAL_STRENGTH, wifiStrength); // assigning value to V3
  Serial.print("WiFi Strength: "); // to print data on serial monitor
  Serial.println(wifiStrength);

  

  if (wifiStrength > -50)
  {
    Blynk.virtualWrite(VPIN_WIFI_STRENGTH_IN_WORDS, "Excellent");
    Serial.println("WiFi Signal is Excellent");
  }

  if (wifiStrength <= -50 && wifiStrength >= -60)
  {
    Blynk.virtualWrite(VPIN_WIFI_STRENGTH_IN_WORDS, "Good");
    Serial.println("WiFi Signal is Good");
  }

  if (wifiStrength <= -60 && wifiStrength >= -70)
  {
    Blynk.virtualWrite(VPIN_WIFI_STRENGTH_IN_WORDS, "Fair");
    Serial.println("WiFi Signal is Fair");
  }

  if (wifiStrength < -70)
  {
    Blynk.virtualWrite(VPIN_WIFI_STRENGTH_IN_WORDS, "Weak");
    Serial.println("WiFi Signal is Weak");
  }
}
void wifi(){
  if(WiFi.status() == WL_CONNECTED){
Serial.println("wificonnect");
}
else{
Serial.println("failed");
WiFi.reconnect();
}
}

void setup()
{
  Serial.begin(9600);
  //Blynk.begin(AUTH, WIFI_SSID, WIFI_PASS);
WiFi.begin(WIFI_SSID, WIFI_PASS);
Blynk.config(AUTH);  // Initialise the Blynk connection settings
    Blynk.connect();                               // Attempt to connect to Blynk

  
// ArduinoOTA.setHostname(OTA_HOSTNAME);  // For OTA - Use your own device identifying name
 // ArduinoOTA.begin();  // For OTA


  pinMode(PULSE_PIN, INPUT);  // Initialization of the variable "PULSE_PIN" as INPUT (D2 pin)
pinMode(PULSE_PIN, LOW);

  attachInterrupt(PULSE_PIN, pulseCounter, FALLING);

  pinMode(PULSE_PIN2, INPUT);  // Initialization of the variable "PULSE_PIN" as INPUT (D2 pin)
pinMode(PULSE_PIN2, LOW);
  attachInterrupt(PULSE_PIN2, pulseCounter2, FALLING);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  //totalMilliLitres  = 0;
  oldTime           = 0;
 // totalLitresold = 0;
 pulseCount2        = 0;
  flowRate2          = 0.0;
  flowMilliLitres2   = 0;
  //totalMilliLitres2  = 0;
  oldTime2           = 0;
  //totalLitresold2 = 0;


pinMode(pumpPin, OUTPUT);

  digitalWrite(pumpPin, LOW);  // ACTIVE HIGH, pump relay set to OFF on restart


attachInterrupt(pumpPin, pumpcontrol, CHANGE);
timer.setInterval(1000L, pumpcontrol );
timer.setInterval(1500L, wifiSignalStrength);  
   timer.setInterval(1200L, sendtoBlynk); // send values blynk server every 1 sec
timer.setInterval(3600000L, godzinowe); // 3600000L
timer.setInterval(3600010L, godzinowe2); // 3600000L
timer.setInterval(1300L, wifi );
timer.setInterval(2100L, miganievLED );


}


void loop()
{ 
  Blynk.run();
//ArduinoOTA.handle();  // For OTA
  timer.run();
  flow();
  
}
