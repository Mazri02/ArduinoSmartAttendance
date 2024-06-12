/* Code Written by Rishi Tiwari
 *  Website:- https://tricksumo.com
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Update HOST URL here

#define HOST "192.168.202.23"          // Enter HOST URL without "http:// "  and "/" at the end of URL
#define WIFI_SSID "Infinix NOTE 30"            // WIFI SSID here                                   
#define WIFI_PASSWORD "19112002"        // WIFI password here

// Declare global variables which will be uploaded to server
String postData;


void setup() {     
    Serial.begin(9600); 
    Serial.println("Communication Started \n\n");  
    delay(1000);
      
    pinMode(LED_BUILTIN, OUTPUT);     // initialize built in led on the board
    
    WiFi.mode(WIFI_STA);           
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi
    Serial.print("Connecting to ");
    Serial.print(WIFI_SSID);
    while (WiFi.status() != WL_CONNECTED) 
    { Serial.print(".");
        delay(500); }

    Serial.println();
    Serial.print("Connected to ");
    Serial.println(WIFI_SSID);
    Serial.print("IP Address is : ");
    Serial.println(WiFi.localIP());    //print local IP address

    delay(30);
}



void loop() { 
    if(Serial.available() > 0){
      HTTPClient http;    // http object of class HTTPClient
      WiFiClient wclient; // wclient object of class WiFiClient  

    String myString = Serial.readString();
    String listData[3];
    int count = 0;

    char charBuf[100]; // Ensure this is large enough to hold your string
    char *token;

    myString.toCharArray(charBuf, myString.length() + 1);
    token = strtok(charBuf, ";");

    while (token != NULL) {
      listData[count] = token; // Print the token
      count++;
      token = strtok(NULL, ";"); // Get the next token
    } 

    // Convert integer variables to string
    http.begin(wclient, "http://192.168.109.23/smartAttendance/dbwrite.php");              // Connect to host where MySQL databse is hosted
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");            //Specify content-type header
    
    postData = "studentid=" + String(listData[2]) + "&firstName=" + listData[0] + "&lastName=" + listData[1];
    int httpCode = http.POST(postData);   // Send POST request to php file and store server response code in variable named httpCode

String webpage = http.getString();    // Get html webpage output and store it in a string
    Serial.println(webpage + "\n"); 
    // if connection eatablished then do this
    if (httpCode == 200) { Serial.println("Values uploaded successfully."); Serial.println(httpCode); 
    String webpage = http.getString();    // Get html webpage output and store it in a string
    Serial.println(webpage + "\n"); 
    }

    // if failed to connect then return and restart

    else { 
      Serial.println(httpCode); 
      Serial.println("Failed to upload values. \n"); 
      http.end(); 
      return; }


    delay(3000); 
    digitalWrite(LED_BUILTIN, LOW);
    delay(3000);
    digitalWrite(LED_BUILTIN, HIGH);
    }
}
