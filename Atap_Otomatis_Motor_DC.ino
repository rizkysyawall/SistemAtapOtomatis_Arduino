#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(A3, A2); // RX, TX
#endif

char ssid[] = "Smartroof";            // your network SSID (name)
char pass[] = "12345678";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "atapotomatis.000webhostapp.com";

// Initialize the Ethernet client object
WiFiEspClient client;

// inialisasi masing2 pin
const int pinMotorA = 2;
const int pinMotorB = 3;
const int pinSpeed = 6;
const int pinHujan = A0;
const int pinLdr = A1;

// inialisasin sensor kan pengkondisian
int hujan, cahaya, kondisi;

// ----------------- program default awal ------------------- //

void setup()
{
    // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're Connected to network");
  
//  Serial.begin(9600);

  // inialisasi status I/O pin
  pinMode(pinHujan, INPUT);
  pinMode(pinLdr, INPUT);
  pinMode(pinMotorA, OUTPUT);
  pinMode(pinMotorB, OUTPUT);
  pinMode(pinSpeed, OUTPUT);

  // inialisasi kondisi awal
  kondisi = 0;
}

// -------------------- program utama --------------------- //
void loop()
{
  String statusAtap;
  // setting kecepatan motor dc, range dari 0 - 255
  analogWrite(pinSpeed, 100);

  // inialisasi pembacaan sensor
  hujan = analogRead(pinHujan);
  cahaya = analogRead(pinLdr);

  // kondisi cerah tanpa hujan --- Atap Terbuka
  if (kondisi == 0 && hujan >= 930 && cahaya <= 699)
  {
    Serial.println("Kondisi Cerah Tanpa Hujan");
    digitalWrite(pinMotorA, LOW);
    digitalWrite(pinMotorB, LOW);
    statusAtap="Terbuka";
    Serial.println("Atap Terbuka");
    kondisi = 0;
    delay(100);
  }

  // kondisi cerah tapi hujan --- Atap Tertutup
  else if (kondisi == 0 && hujan <= 929 && cahaya <= 699)
  {
    Serial.println("Kondisi Cerah Tapi Hujan");
    digitalWrite(pinMotorA, HIGH);
    digitalWrite(pinMotorB, LOW);
    statusAtap="Tertutup";
    kondisi = 1;
        Serial.println("Atap Tertutup");
    // ubah delay untuk menetukan lamanya motor dc saat berputar
    delay(100);
  }

  // kondisi gelap tanpa hujan --- Atap Tertutup
  else if (kondisi == 0 && hujan >= 930 && cahaya >= 700)
  {
    Serial.println("Kondisi Gelap Tanpa Hujan");
    digitalWrite(pinMotorA, HIGH);
    digitalWrite(pinMotorB, LOW);
    statusAtap="Tertutup";
    kondisi = 1;
    Serial.println("Atap Tertutup");
    delay(100);
  }

  // kondisi gelap dan hujan -- Atap Tertutup
  else if (kondisi == 0 && hujan <= 929 && cahaya >= 700)
  {
    Serial.println("Kondisi Gelap Tapi Hujan");
    digitalWrite(pinMotorA, HIGH);
    digitalWrite(pinMotorB, LOW);
    statusAtap="Tertutup";
    kondisi = 1;
    Serial.println("Atap Tertutup");
    delay(100);
  }

  // kondisi cerah tanpa hujan --- Atap Terbuka
  else if (kondisi == 1 && hujan >= 930 && cahaya <= 699)
  {
    Serial.println("Kondisi Cerah Tanpa Hujan");
    digitalWrite(pinMotorA, LOW);
    digitalWrite(pinMotorB, HIGH);
    statusAtap="Terbuka";
    kondisi = 0;
    Serial.println("Atap Terbuka");
    delay(100);
  }

  // kondisi cerah tapi hujan --- Atap Tertutup
  else if (kondisi == 1 && hujan <= 929 && cahaya <= 699)
  {
    Serial.println("Kondisi Cerah Tapi Hujan");
    digitalWrite(pinMotorA, LOW);
    digitalWrite(pinMotorB, LOW);
    statusAtap="Tertutup";
    kondisi = 1;
    Serial.println("Atap Tertutup");
    delay(100);
  }

  // kondisi gelap tanpa hujan --- Atap Tertutup
  else if (kondisi == 1 && hujan >= 930 && cahaya >= 700)
  {
    Serial.println("Kondisi Gelap Tanpa Hujan");
    digitalWrite(pinMotorA, LOW);
    digitalWrite(pinMotorB, LOW);
    statusAtap="Tertutup";
    kondisi = 1;
    Serial.println("Atap Tertutup");
    delay(100);
  }

  // kondisi gelap serta hujan -- Atap Tertutup
  else if (kondisi == 1 && hujan <= 929 && cahaya >= 700)
  {
    Serial.println("Kondisi Gelap Disertai Hujan");
    digitalWrite(pinMotorA, LOW);
    digitalWrite(pinMotorB, LOW);
    statusAtap="Tertutup";
    kondisi = 1;
    Serial.println("Atap Tertutup");
    delay(100);
  }
    String resultcahaya = String(cahaya);
    Serial.println(resultcahaya);
    String resulthujan = String(hujan);
    Serial.println(resulthujan);
      
      client.stop();
      delay(500);
      if (client.connect(server, 80)) {
        Serial.println("Connecting...");
        String url = "/api.php/insert/" + resultcahaya + "/" + resulthujan + "/" + statusAtap;
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + server + "\r\n" +
                     "Connection: close\r\n\r\n");
        Serial.println("Sukses...."); 
      }
      else {
        // if you couldn't make a connection
        Serial.println("Connection failed");
 
      }
 
      boolean incomingData = false;
      while (client.available() && status == WL_CONNECTED) {
 
        if (incomingData == false)
        {
 
          incomingData = true;
        }
        char c = client.read();
        Serial.write(c);
 
      }
      
}

