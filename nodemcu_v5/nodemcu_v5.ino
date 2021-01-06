// data yang dikirimkan dari mikon lain untuk set ke firebase
// *0,800,800,800,70#
// untuk membaca data yang ada di firebase dan mengirimkan ke mikon lain.
// *0,1,1,1#

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Setting untuk firebase, ssid wifi, dan password.
#define FIREBASE_HOST "xxxxxxxxxxx.firebaseio.com"
#define FIREBASE_AUTH "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define WIFI_SSID "xxxxxx"
#define WIFI_PASSWORD "xxxxxxxxx"

unsigned long previousMillis = 0;
const long interval = 60000;
int n = 0;

// untuk data komunikasi serial.
String dataIn;
String dt[10];
int i;

// untuk parameter sensor pada alat lainnya.
String berat1 = "";
String berat2 = "";
String berat3 = "";
String suhu = "";

// untuk parameter control pada alat lainnya.
int door1 = 0;
int door2 = 0;
int door3 = 0;
int doorTemp1 = 0;
int doorTemp2 = 0;
int doorTemp3 = 0;
boolean parsing = false;

// untuk mereset pengaturan ke semula alat
void onSetFirebase()
{
  Firebase.setInt("control/door1", 0);
  Firebase.setInt("control/door2", 0);
  Firebase.setInt("control/door3", 0);
}

void setup()
{
  // untuk inisialisasi bit komunikasi serial.
  Serial.begin(9600);

  // untuk setup konesifitas dan memvalidasi koneksi ke server.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  onSetFirebase();
}

// untuk tempat intruksi utama pada arduino.
void loop()
{
  getdata();
  readFirebase();
}

void readFirebase()
{
  door1 = Firebase.getInt("control/door1");
  door2 = Firebase.getInt("control/door2");
  door3 = Firebase.getInt("control/door3");

  //  Serial.println(String("") + door1 + (",") + doorTemp1 + (""));
  //  Serial.println(String("") + door2 + (",") + doorTemp2 + (""));
  //  Serial.println(String("") + door3 + (",") + doorTemp3 + (""));

  if (door1 != doorTemp1) {
    Serial.println(String("*0,") + door1 + (",") + door2 + (",") + door3 + ("#"));
    doorTemp1 = door1;
  }
  if (door2 != doorTemp2) {
    Serial.println(String("*0,") + door1 + (",") + door2 + (",") + door3 + ("#"));
    doorTemp2 = door2;
  }
  if (door3 != doorTemp3) {
    Serial.println(String("*0,") + door1 + (",") + door2 + (",") + door3 + ("#"));
    doorTemp3 = door3;
  }

  delay(1000);
}


// untuk membaca koomunikisi serial yang dikirim dari mikom lain.
void getdata()
{
  while (Serial.available() > 0)
  {
    char inChar = (char)Serial.read();
    dataIn += inChar;
    if (inChar == '\n')
    {
      parsing = true;
    }
  }
  if (parsing)
  {
    parsingData();
    parsing = false;
    dataIn = "";
  }
}

void parsingData()
{
  int j = 0;
  int z = 0;
  dt[j] = "";
  for (i = 1; i < dataIn.length(); i++)
  {
    //pengecekan tiap karakter dengan karakter (#) dan (,)
    if ((dataIn[i] == '#') || (dataIn[i] == ','))
    {
      j++;
      dt[j] = "";
      z++;
    }
    else
    {
      dt[j] = dt[j] + dataIn[i];
    }
  }
  berat1 = dt[1];
  berat2 = dt[2];
  berat3 = dt[3];

  int brt1 =  berat1.toInt();
  int brt2 =  berat2.toInt();
  int brt3 =  berat3.toInt();
  delay(250);

  if (brt1 > 20)
  {
    //    Serial.println(brt1);
    upload_box1(berat1);
  }
  if (brt2 > 20)
  {
    //    Serial.println(brt2);
    upload_box2(berat2);
  }
  if (brt3 > 20)
  {
    //    Serial.println(brt3);
    upload_box3(berat3);
  }

  delay(100);
}

// untuk mengaktifkan sensor berat pada box 1.
void upload_box1(String berat)
{
  Firebase.setString("sensors/LoadCell/box1", berat);
  if (Firebase.failed())
  {
    Serial.print("update /logs failed:");
    Serial.println(Firebase.error());
    return;
  }
  delay(500);
}

// untuk mengaktifkan sensor berat pada box 2.
void upload_box2(String berat)
{
  Firebase.setString("sensors/LoadCell/box2", berat);
  if (Firebase.failed())
  {
    Serial.print("update /logs failed:");
    Serial.println(Firebase.error());
    return;
  }
  delay(500);
}

// untuk mengaktifkan sensor berat pada box 3.
void upload_box3(String berat)
{
  Firebase.setString("sensors/LoadCell/box3", berat);
  if (Firebase.failed())
  {
    Serial.print("update /logs failed:");
    Serial.println(Firebase.error());
    return;
  }
  delay(500);
}
