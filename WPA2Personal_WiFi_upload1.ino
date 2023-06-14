#include <esp_wpa2.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>  
#include <addons/TokenHelper.h>
#include "Arduino.h"
#include "addons/RTDBHelper.h"
#include "DHT.h"
#include <Wire.h>
#include <MPU6050.h>


#define FLAMA_PIN 17   // Pin input para el puerto DO . Sensor flama  
#define DHTPIN 15 


// Se define el tipo de sensor a utilizar
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);


//Variables
int MIC = 34; //El pin que detecta el microfono       //Variables para el sensor de sonido KY-038
int sonido ; //Variable en la que se guarda la señal
int isFlame = HIGH;             // Sensor flama. HIGH significa NO FLAME
bool flama;
bool movimiento;                //Sensor sonido

float h;                        //Sensor DHT
float t;                        //Sensor DHT
float f;                        //Sensor DHT
String numero;                  //Numero de Firebase /App_Inventor/numero
int num;

MPU6050 mpu;
int16_t accX, accY, accZ;

// Pin input para el acelerometro
Wire.begin(21, 22); // Inicializar la comunicación I2C en los pines 21 (SDA) y 22 (SCL) del ESP32
mpu.initialize();
  
  // Configurar el acelerómetro con los parámetros deseados
mpu.setAccelerometerRange(MPU6050_ACCEL_FS_2);


// Credenciales para conectarse a la red bajo protocolo WPA2-Personal
const char* ssid = "HUAWEIY5p";
const char* password = "12345678";

// Credenciales para el escritura de datos en Firebase
#define USER_EMAIL "a01733320@tec.mx"
#define USER_PASSWORD "123456"

// API Key de la DB en Firebase
#define API_KEY "AIzaSyBs8i0oqsu-hM0ffjXYoAdmdu7zj3PxtIo"//AIzaSyAjjTHMIV0y394tayvijhU-aVVcKdkIZxU

// Definimos el URL de la base de datos
#define DATABASE_URL "https://prueba1-e14aa-default-rtdb.firebaseio.com"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
// Valor booleano para evaluar el inicio de sesion exitoso
bool signupOK = false;

void setup() {
  Serial.begin(115200);       // Se incializa el puerto serial con tasa de informacion en 115200 bits/segundos
  dht.begin();                // Se inicializa la lectura del sensor DHT11
  delay(10);                  // Delay


  Serial.println();
  Serial.print("Connecting to ");
  // Se imprime el ssid de la red declarada
  Serial.println(ssid);
  // Se inicializa la conexion a Internet con las credenciales declaradas
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Se asigna la variable API_KEY a config.api_key
  config.api_key = API_KEY;
  // Se asigna la variable DATABASE_URL a config.database_url
  config.database_url = DATABASE_URL;
  // Se verifica que el loggeo con la DB haya sido exitoso
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  // Se inicializa Firebase con las credenciales declaradas anteriormente
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

int value = 0;


void sensorTempHum(){
  // Se lee humedad
  h = dht.readHumidity();
  // Se lee temperatura en grados Celsius, magnitud predeterminada
  t = dht.readTemperature();
  // Se lee temperatura en grados Fahrenheit, parametro true
  f = dht.readTemperature(true);
}

void sensorFlama(){
  isFlame = digitalRead(FLAMA_PIN);   // Se le asigna a isFlame el valor recibido por el sensor de flama
  if(isFlame==HIGH){
    flama=false;
  }
  else{
    flama=true;
  }
}

void sensorSonido(){
    //Detecta el sonido más alto (nunca se usa delay)  KY-038
  if (sonido < analogRead(MIC)){
    sonido = analogRead(MIC);
  }
}


void sensorMovimiento(){
    // Leer los valores de aceleración en los ejes X, Y y Z
  mpu.getAcceleration(&accX, &accY, &accZ);
  
  // Calcular la magnitud de la aceleración
  float magnitud = sqrt(pow(accX, 2) + pow(accY, 2) + pow(accZ, 2));
  
  // Definir un umbral de movimiento para detectar el movimiento
  float umbralMovimiento = 1000.0; // Ajusta este valor según tus necesidades
  
    // Verificar si se ha superado el umbral de movimiento
  if (magnitud > umbralMovimiento) {
    // Se ha detectado movimiento
    movimiento=true;
  }
  else{
    movimiento=false;
  }
}

void loop() {
  delay(2000);
  // Se llaman a la funciones dedicadas a las mediciones
  sensorTempHum();
  sensorFlama();
  sensorSonido();
  sensorMovimiento();

  // Se validan diversas cuestiones para determinar que la conexion wifi y el logging in a la base de datos haya sido exitoso
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    // Para todas las escrituras de variables, se valida que la escritura haya sido exitosa
    // En caso de que no se pueda escribir la lectura, se imprime la razon del error
    // Se manda la variable t a la database en path App_Inventor/temperaturaC
    if (Firebase.RTDB.setFloat(&fbdo, "App_Inventor/temperaturaC", t)){
      Serial.println("PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    // Se manda la variable f a la database en path App_Inventor/temperaturaF
    if (Firebase.RTDB.setFloat(&fbdo, "App_Inventor/temperaturaF", f)){
      Serial.println("PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    // Se manda la variable f a la database en path App_Inventor/humedad
    if (Firebase.RTDB.setFloat(&fbdo, "App_Inventor/humedad", h)){
      Serial.println("PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    //Se manda la variable flama a la database en path App_Inventor/flama
    if (Firebase.RTDB.setBool(&fbdo, "App_Inventor/flama", flama)){
      Serial.println("PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    //Se manda la variable movimiento a la database en path App_Inventor/movement
    if (Firebase.RTDB.setBool(&fbdo, "App_Inventor/movimiento", movimiento)){
      Serial.println("PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Se manda la variable sonido a la database en App_Inventor/distance
    if (Firebase.RTDB.setFloat(&fbdo, "App_Inventor/sonido", sonido)){
      Serial.println("PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
  }
}
