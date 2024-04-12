#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

const char* ssid     = "Telemetria T";      
const char* password = "m3d1c4lsiis12";    
const char* host = "192.168.10.30";  
const int   port = 80;            
const int   watchdog = 5000;        
unsigned long previousMillis = millis(); 

int lcdColumns=16;
int lcdRows=2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

#define DHTTYPE DHT22 
#define DHTPIN  13 

DHT dht(DHTPIN, DHTTYPE, 27); 

float temperatura; // double
float humedad;
String zona = "Farmacia";
int contador=0;
int iterador=1;
void setup() {
  Serial.begin(115200);
  
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  lcd.begin();
  lcd.backlight();
  dht.begin();
}

void loop() {
  temperatura = dht.readTemperature();
  humedad = dht.readHumidity();
  lcd.setCursor(0, 0);
  lcd.print("Temp : ");
  lcd.print(temperatura);
  lcd.print(" C");
//VALIDAR CON EL LCD
  lcd.setCursor(0,1);
  lcd.print("Hum.R: ");
  lcd.print(humedad);
  lcd.print(" %");
//VALIDAR CON EL LCD
  
  unsigned long currentMillis = millis();    
  if ( currentMillis - previousMillis > watchdog ) {
    previousMillis = currentMillis;
    WiFiClient client;
    if (!client.connect(host, port)) {
      Serial.println("Fallo al conectar");
      return;
    }
   if (contador>=20){
   iterador=1;
   contador=0;
   }else{
    iterador=0;
    }

    Serial.println(contador);

    
    String url = "/jorge/jorge.php?zona=";
    url += zona;
    url += "&temp=";
    url += temperatura;
    url += "&hum=";
    url += humedad;
    url += "&dato=";
    url += iterador;

  
  
    
    // Enviamos petición al servidor
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

               
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
  
    // Leemos la respuesta del servidor
    while(client.available()){
      String line = client.readStringUntil('\r');
    //  if (contador==20){
   //  iterador=1;
   //  contador=0;
   //   }else{
   //   iterador=0;
   //   }
      Serial.print(line);
    }


    
  }
  contador++;
  delay (5000);

}
