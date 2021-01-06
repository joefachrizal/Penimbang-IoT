// hasil dari data sensor yang dikirimkan ke nodemcu
// *0,800,800,800,70#
// kiriman dari ndemcu intruksi box terbuka
// *0,1,1,1#

#include <Q2HX711.h>

unsigned long previousMillis = 0;
const long inter_60 = 3600000;
const long inter_30 = 1800000;
const long inter_15 = 900000;
const long inter_10 = 600000;
const long interval = 1000;

// Komunikasi Serial dengan NodeMCU
String dataIn;
String dt[10];
int i;

int kunci1, kunci2, kunci3;
String readSensor = "000";
boolean parsing = false;

// Sensor Load cell (Berat)
const byte box_d_1 = 18;
const byte box_c_1 = 16;
Q2HX711 scale1(box_d_1, box_c_1);

const byte box_d_2 = 28;
const byte box_c_2 = 30;
Q2HX711 scale2(box_d_2, box_c_2);

const byte box_d_3 = 26;
const byte box_c_3 = 24;
Q2HX711 scale3(box_d_3, box_c_3);

float value = 1000.0;                           // calibrated mass to be added
long box_1_x1 = 8801641L;
long box_1_x0 = 8433449L;
long box_2_x1 = 8966302L;
long box_2_x0 = 8590655L;
long box_3_x1 = 8988981L;
long box_3_x0 = 8597655L;
float avg_size = 10.0;                          // amount of averages for each mass measurement

int looping = 0;
long reading_b1 = 0;
long reading_b2 = 0;
long reading_b3 = 0;

// Selenoid (Kunci)
#define kunci_1 7
#define kunci_2 5
#define kunci_3 3

// LED (Indikator)
#define notif_full_1_1 27
#define notif_med_1_2 29
#define notif_low_1_3 31

#define notif_full_2_1 15
#define notif_med_2_2 17
#define notif_low_2_3 19

#define notif_full_3_1 9
#define notif_med_3_2 11
#define notif_low_3_3 13

void setup() {
  Serial.begin(9600);

  pinMode(kunci_1, OUTPUT);
  pinMode(kunci_2, OUTPUT);
  pinMode(kunci_3, OUTPUT);

  pinMode(notif_full_1_1, OUTPUT);
  pinMode(notif_med_1_2, OUTPUT);
  pinMode(notif_low_1_3, OUTPUT);

  pinMode(notif_full_2_1, OUTPUT);
  pinMode(notif_med_2_2, OUTPUT);
  pinMode(notif_low_2_3, OUTPUT);

  pinMode(notif_full_3_1, OUTPUT);
  pinMode(notif_med_3_2, OUTPUT);
  pinMode(notif_low_3_3, OUTPUT);
}

void loop() {
  getdata();
  loadCell();
}

// =========================================== ambil data dari serial ====================================
// =======================================================================================================
void getdata() {
  if (Serial.available() > 0)
  {
    char inChar = (char)Serial.read();
    dataIn += inChar;
    if (inChar == '\n') {
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

// =========================================== kirim data arduino ke serial ==============================
// =======================================================================================================
// *0,1,1,1#
void parsingData() {
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
  kunci1 = dt[1].toInt();
  kunci2 = dt[2].toInt();
  kunci3 = dt[3].toInt();

  readSensor = String ("") + kunci1  + kunci2 + kunci3 + ("");
  //  Serial.println(readSensor);

  // Kunci Box
  //*0,1,0,0#
  if (kunci1 == 0) {
    digitalWrite(kunci_1, LOW);
    //    Serial.println("Box 1 Tertutup");
  }
  if (kunci1 == 1) {
    digitalWrite(kunci_1, HIGH);
    //    Serial.println("Box 1 Terbuka");
  }
  //*0,0,1,0#
  if (kunci2 == 0) {
    digitalWrite(kunci_2, LOW);
    //    Serial.println("Box 2 Tertutup");
  }
  if (kunci2 == 1) {
    digitalWrite(kunci_2, HIGH);
    //    Serial.println("Box 2 Terbuka");
  }
  //*0,0,0,1#
  if (kunci3 == 0) {
    digitalWrite(kunci_3, LOW);
    //    Serial.println("Box 3 Tertutup");
  }
  if (kunci3 == 1) {
    digitalWrite(kunci_3, HIGH);
    //    Serial.println("Box 3 Terbuka");
  }
  delay(100);
}

void loadCell() {
  reading_b1 += scale1.read();
  reading_b2 += scale2.read();
  reading_b3 += scale3.read();

  if (looping == avg_size) {
    reading_b1 /= long(avg_size);
    reading_b2 /= long(avg_size);
    reading_b3 /= long(avg_size);

    // box Satu
    float ratio_1_b1 = (float) (reading_b1 - box_1_x0);
    float ratio_2_b1 = (float) (box_1_x1 - box_1_x0);
    float ratio_b1 = ratio_1_b1 / ratio_2_b1;
    int berat_1 = value * ratio_b1;

    // box Dua
    float ratio_1_b2 = (float) (reading_b2 - box_2_x0);
    float ratio_2_b2 = (float) (box_2_x1 - box_2_x0);
    float ratio_b2 = ratio_1_b2 / ratio_2_b2;
    int berat_2 = value * ratio_b2;

    // box Tiga
    float ratio_1_b3 = (float) (reading_b3 - box_3_x0);
    float ratio_2_b3 = (float) (box_3_x1 - box_3_x0);
    float ratio_b3 = ratio_1_b3 / ratio_2_b3;
    int berat_3 = value * ratio_b3;

    //    Serial.print("Status Box 1 :");
    //    Serial.print(berat_1);
    //    Serial.print("\t Box 2 :");
    //    Serial.print(berat_2);
    //    Serial.print("\t Box 3 :");
    //    Serial.print(berat_3);
    //    Serial.println(".");

    cek_box_off_1(berat_1);
    cek_box_off_2(berat_2);
    cek_box_off_3(berat_3);

    // Berat Di Box
    if (readSensor == "100") {
      //*0,0,0,0,1,0,0#
      Serial.println(String ("*0,") + berat_1 + (",0,0#"));
    }
    if (readSensor == "010") {
      //*0,0,0,0,0,1,0#
      Serial.println(String ("*0,0,") + berat_2 + (",0#"));
    }
    if (readSensor == "001") {
      //*0,0,0,0,0,0,1#
      Serial.println(String ("*0,0,0,") + berat_3 + ("#"));
    }
    if (readSensor == "110") {
      //*0,0,0,0,1,1,0#
      Serial.println(String ("*0,") + berat_1 + (",") + berat_2 + (",0#"));
    }
    if (readSensor == "101") {
      //*0,0,0,0,1,0,1#
      Serial.println(String ("*0,") + berat_1 + (",0,") + berat_3 + ("#"));
    }
    if (readSensor == "011") {
      //*0,0,0,0,0,1,1#
      Serial.println(String ("*0,0,") + berat_2 + (",") + berat_3 + ("#"));;
    }
    if (readSensor == "111") {
      //*0,0,0,0,1,1,1#
      Serial.println(String ("*0,") + berat_1 + (",") + berat_2 + (",") + berat_3 + ("#"));
    }

    looping = 0;
    reading_b1 = 0;
    reading_b2 = 0;
    reading_b3 = 0;
  }
  looping++;
}

// ========================================= cek isi box offline =========================================
// =======================================================================================================
// cek isi box 1
void cek_box_off_1(int berat) {
  if (berat < 1500) {
    if (berat > -10 && berat < 500) {
      digitalWrite(notif_full_1_1, LOW);
      digitalWrite(notif_med_1_2, LOW);
      digitalWrite(notif_low_1_3, HIGH);
      //      Serial.println("Status = Low");
    }
    if (berat > 500 && berat < 700) {
      digitalWrite(notif_full_1_1, LOW);
      digitalWrite(notif_med_1_2, HIGH);
      digitalWrite(notif_low_1_3, HIGH);
      //      Serial.println("Status = Med");
    }
    if (berat > 700 && berat < 1500 ) {
      digitalWrite(notif_full_1_1, HIGH);
      digitalWrite(notif_med_1_2, HIGH);
      digitalWrite(notif_low_1_3, HIGH);
      //      Serial.println("Status = Full");
    }
  }
}

// cek isi box 2
void cek_box_off_2(int berat) {
  if (berat < 1500) {
    if (berat > -10 && berat < 500) {
      digitalWrite(notif_full_2_1, LOW);
      digitalWrite(notif_med_2_2, LOW);
      digitalWrite(notif_low_2_3, HIGH);
    }
    if (berat > 500 && berat < 700) {
      digitalWrite(notif_full_2_1, LOW);
      digitalWrite(notif_med_2_2, HIGH);
      digitalWrite(notif_low_2_3, HIGH);
    }
    if (berat > 700 && berat < 1500 ) {
      digitalWrite(notif_full_2_1, HIGH);
      digitalWrite(notif_med_2_2, HIGH);
      digitalWrite(notif_low_2_3, HIGH);
    }
  }
}

// cek isi box 3
void cek_box_off_3(int berat) {
  if (berat < 1500) {
    if (berat > -10 && berat < 500) {
      digitalWrite(notif_full_1_1, LOW);
      digitalWrite(notif_med_3_2, LOW);
      digitalWrite(notif_low_3_3, HIGH);
    }
    if (berat > 500 && berat < 700) {
      digitalWrite(notif_full_3_1, LOW);
      digitalWrite(notif_med_3_2, HIGH);
      digitalWrite(notif_low_3_3, HIGH);
    }
    if (berat > 700 && berat < 1500 ) {
      digitalWrite(notif_full_3_1, HIGH);
      digitalWrite(notif_med_3_2, HIGH);
      digitalWrite(notif_low_3_3, HIGH);
    }
  }
}

//void calibration_load_cell() {
//    // tare procedure
//    for (int ii = 0; ii < int(avg_size); ii++) {
//      delay(10);
//      x0 += hx711.read();
//    }
//    x0 /= long(avg_size);
//    Serial.println("Add Calibrated Mass");
//    // calibration procedure (mass should be added equal to value)
//    int ii = 1;
//    while (true) {
//      if (hx711.read() < x0 + 10000) {
//      } else {
//        ii++;
//        delay(2000);
//        for (int jj = 0; jj < int(avg_size); jj++) {
//          x1 += hx711.read();
//        }
//        x1 /= long(avg_size);
//        break;
//      }
//    }
//    Serial.println("Calibration Complete");
//}
