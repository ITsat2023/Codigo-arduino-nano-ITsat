//Kodea hau ITsat2023ko taldeak idatzi du Cansat 2023 txapelketan parte artzeko.

//ITsat2023ko talde parte artzaileak:
//Anartz Balbuena (Kodea)
//Ibai Elizondo (Komunikazioa)
//Ainhoa Gozalez (Lata eta Paratxutearen diseinatzalea)
//Iñaki Mutilva (Grafikak)
//Izaro Echeverria (Comunity manager)

#include <SoftwareSerial.h>
//Denbora
unsigned long denbora1 = 0;
unsigned long denbora2 = 0;
unsigned long denboraSegundutan = 0;

//BME280 Liburutegiak
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> // Liburutegiak eraintsiko ditugu.
#define SEALEVELPRESSURE_HPA (1013.25) // Itsas mailan dagoen presioa definituko dugu.
Adafruit_BME280 bme; // Adafruit_BME280, bme izena jarriko diogu.

//GPS Liburutegiak
#include <TinyGPS.h>
TinyGPS gps;
SoftwareSerial ss(4, 3);

static void smartdelay(unsigned long ms);
static void print_float(float val, float invalid, int len, int prec);
static void print_int(unsigned long val, unsigned long invalid, int len);
static void print_date(TinyGPS &gps);
static void print_str(const char *str, int len);

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

void setup() {
Serial.begin(9600);// Serial asi

//serial idatzi
  Serial.println("Denbora(s) ,Metano(ppb) ,Temperatura(C) ,Presioa(hPa) ,Altuera(BME280)(m) ,Sateliteak ,Latitudea ,Longitudea ,Altuera(GPS)(m)");
ss.begin(9600);

//Denbora
denbora1 = millis();

//BME280
if (!bme.begin(0x76)) {
  Serial.println("Ez da aurkitu BME280!"); //Idatzi "Ez da aurkitu BME280!"
  while (1); 
  }

}
//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

void loop() {
//Denbora
  denbora2 = millis();
  if(denbora2 > (denbora1+1000)){  //segundu 1 pasa baldin bada if asi
    denbora1 = millis(); //denbora aktualizatu
    denboraSegundutan = denbora1/1000;
    Serial.print(denboraSegundutan);
  }

 Serial.print (", ");

//Metanoa
Serial.print(analogRead(A0));// Bidali MQ4aren jasan dituen datuak


 Serial.print (", ");

//BME280
  Serial.print(bme.readTemperature());
  Serial.print (", ");
  Serial.print(bme.readPressure() / 100.0F);
 Serial.print(", ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
 Serial.print(", ");

//GPS
 float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  static const double LONDON_LAT = 51.508131, LONDON_LON = -0.128002;
 
  print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
  Serial.print(", ");
  gps.f_get_position(&flat, &flon, &age);
  print_float(flat, TinyGPS::GPS_INVALID_F_ANGLE, 10, 6);
  Serial.print(", ");
  print_float(flon, TinyGPS::GPS_INVALID_F_ANGLE, 11, 6);
  Serial.print(", ");
  print_float(gps.f_altitude(), TinyGPS::GPS_INVALID_F_ALTITUDE, 7, 2);
Serial.println();

delay (1000); //Itxaron 1s
}

//--------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------

//GPS ezarpenak
static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec)
{
  if (val == invalid)
  {
    while (len-- > 1)
      Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len)
{
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len-1] = ' ';
  Serial.print(sz);
  smartdelay(0);
}

static void print_date(TinyGPS &gps)
{
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  smartdelay(0);
}

static void print_str(const char *str, int len)
{
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
    Serial.print(i<slen ? str[i] : ' ');
  smartdelay(0);
}
