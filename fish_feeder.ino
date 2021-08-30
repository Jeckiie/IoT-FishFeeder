#include <NTPClient.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FirebaseArduino.h>

// Set these to run example.
#define FIREBASE_HOST "fish-feeder-a0d9e-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "zBKXtUbDIeKDX0R5SOOllujMNSYV6w0UHsCFVdk7"
#define WIFI_SSID "Jerko"
#define WIFI_PASSWORD "jerko54321"
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 

long feedTime = 2000; // 12 hours between feeding
long endtime; 
long timeNow;
int feedNow = 0;
int feedTimeChanged = 0;

//TIME
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
String days;
String hours;
String minutes;
int feedIndex = 0;

void setup() {
  Serial.begin(9600);
  delay(15);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  delay(15);
  timeClient.begin();
  
  myservo.attach(13);  // attaches the servo on pin 9 to the servo object 
  myservo.write(0);
  delay(15);
}
void loop() {

  
  
  delay(1000);
  Serial.print("Get time: ");
  Serial.println(Firebase.getInt("feedTime"));
  feedTime = Firebase.getInt("feedTime");
  feedTimeChanged = 0;
  Firebase.setInt("feedTimeChanged", 0);
  delay(1000);

  timeNow = millis();
  endtime = timeNow + feedTime;
  while(timeNow < endtime && feedTimeChanged == 0) {
   //myservo.write(0);
   //delay(1000);
   feedNow = Firebase.getInt("feedNow");
   delay(15);
   feedTimeChanged = Firebase.getInt("feedTimeChanged");
   if (feedNow == 1) {
    feedFish();
    Firebase.setInt("feedNow", 0);
   }
   timeNow = millis();   
  }

  if(feedTimeChanged == 0) {
    feedFish();
  }
}

void feedFish() {
  for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  }
  timeClient.update();
  days = daysOfTheWeek[timeClient.getDay()];
  hours = timeClient.getHours();
  minutes = timeClient.getMinutes();
  String path = "lastFeedingTime/";
  path.concat(feedIndex);
  Firebase.setString(path, days + ", " + hours + ":" + minutes);
  if(feedIndex == 4) {
    feedIndex = 0;
  } else {
    feedIndex++;
  }
}
/*
void loop() { 
  now = millis();
  endtime = now + FISHFEEDER;
  while(now < endtime) {
   myservo.write(0);
   delay(1000);
   now = millis();   
  }

  for(pos = 0; pos < 300; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 300; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(15);                       // waits 15ms for the servo to reach the position 
  }
}*/
