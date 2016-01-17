#include <SPI.h>
#include <WiFi101.h>
#include <Wire.h>


//char ssid[] = "IEEE 2.4GHz";        // your network SSID (name)
char ssid[] = "Kai_iPhone";           // your network SSID (name)
char pass[] = "JLPCJLPC";             // your network password

//char ssid[] = "OnePlus2";             // your network SSID (name)
//char pass[] = "abirdwiththeword";     // your network password
int keyIndex = 0;                     // your network key Index number (needed only for WEP)

long signal_queue[75];
int N = 75;
int currentIdx = 0;

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {

  // STS Led
  pinMode(13,OUTPUT);

  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  wifi_init();

  // Setupt PWM for volume control
  setPwmFrequency(9, 1);

  // assign 0 to array
  for (int i=0;i<N;i++){
  signal_queue[i]=0;
}
}


void wifi_init(void)
{
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
//    status = WiFi.begin(ssid);
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(5000);
  }
  
  server.begin();
  
  //  connected now, so print out the status:
  printWifiStatus();
}



void loop()
 {
  
  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");


//  int dist = Rssi2DistInMeter(rssi);
//  Serial.print("Dist: ");
//  Serial.print(dist);
//  Serial.println(" m");

  // Update Array
  signal_queue[currentIdx] = -1*rssi;
  currentIdx++;
  if (currentIdx>(N-1)) {
    currentIdx = 0;
  }

  // Take average of previous 50 data
  long sum = 0, average = 0;
  for (int i = 0; i<N;i++){
    sum += signal_queue[i];
  }
  average = sum/N;

  // Mapping
  
  //             { 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60}
  int volume[16]={220,190,160,130,100, 80, 60, 40, 30, 25, 20, 15, 10,  5,  0,  0 };
  
  int apply;
  if(average<45){
    apply=255;
  }
  else{
    if(average>60){
      apply=0;    
    }
    else apply=volume[average-45];
  }
  Serial.print("Average: ");
  Serial.println(average);
  Serial.print("Apply: ");
  Serial.println(apply);
  analogWrite(9, apply);
  // Update every 0.1 sec (ish)
  delay(20);
  
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

// conversion from decibal to distance
int Rssi2DistInMeter (long rssi) {
  int dist = 2000+33*rssi;
  return dist;
}
