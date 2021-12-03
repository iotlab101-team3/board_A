#include <Wire.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define speaker 14

int angle = 0; //mqtt에 전달하는 값

const int MPU_ADDR = 0x68;                 // I2C통신을 위한 MPU6050의 주소
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; // 가속도(Acceleration)와 자이로(Gyro)
double angleAcX, angleAcY, angleAcZ;
double angleGyX, angleGyY, angleGyZ;
double angleFiX, angleFiY, angleFiZ;

const double RADIAN_TO_DEGREE = 180 / 3.14159;
const double DEG_PER_SEC = 32767 / 250; // 1초에 회전하는 각도
const double ALPHA = 1 / (1 + 0.04);
// GyX, GyY, GyZ 값의 범위 : -32768 ~ +32767 (16비트 정수범위)

// int Track[10] = {"CloseHH.wav", "Crash.wav", "Crash1.wav", "Floor.wav",
//                   "Kick1.wav", "Kick2.wav", "OpenHH.wav", "Ride.wav",
//                    "Scare.wav", "Splash.wav", "Tom1.wav", "Tom2.wav", "Tom3.wav";
//                    }
unsigned long now = 0;  // 현재 시간 저장용 변수
unsigned long past = 0; // 이전 시간 저장용 변수
double dt = 0;          // 한 사이클 동안 걸린 시간 변수

double averAcX, averAcY, averAcZ;
double averGyX, averGyY, averGyZ;
 
const char*         ssid ="IoT518";
const char*         password = "iot123456";
const char*         mqttServer = "3.84.34.84";
const int           mqttPort = 1883;

unsigned long       pubInterval = 5000;
unsigned long       lastPublished = - pubInterval;

WiFiClient espClient;
PubSubClient client(espClient);
const char* topic = "deviceid/jj/cmd/wav";

void initSensor()
{
    Wire.begin();
    Wire.beginTransmission(MPU_ADDR); // I2C 통신용 어드레스(주소)
    Wire.write(0x6B);                 // MPU6050과 통신을 시작하기 위해서는 0x6B번지에
    Wire.write(0);
    Wire.endTransmission(true);
}

void getData()
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B); // AcX 레지스터 위치(주소)를 지칭합니다
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 14, true); // AcX 주소 이후의 14byte의 데이터를 요청
    AcX = Wire.read() << 8 | Wire.read(); //두 개의 나뉘어진 바이트를 하나로 이어 붙여서 각 변수에 저장
    AcY = Wire.read() << 8 | Wire.read();
    AcZ = Wire.read() << 8 | Wire.read();
    Tmp = Wire.read() << 8 | Wire.read();
    GyX = Wire.read() << 8 | Wire.read();
    GyY = Wire.read() << 8 | Wire.read();
    GyZ = Wire.read() << 8 | Wire.read();
}

// loop 한 사이클동안 걸리는 시간을 알기위한 함수
void getDT()
{
    now = millis();
    dt = (now - past) / 1000.0;
    past = now;
}

// 센서의 초기값을 10회 정도 평균값으로 구하여 저장하는 함수
void caliSensor()
{
    double sumAcX = 0, sumAcY = 0, sumAcZ = 0;
    double sumGyX = 0, sumGyY = 0, sumGyZ = 0;
    getData();
    for (int i = 0; i < 10; i++)
    {
        getData();
        sumAcX += AcX;
        sumAcY += AcY;
        sumAcZ += AcZ;
        sumGyX += GyX;
        sumGyY += GyY;
        sumGyZ += GyZ;
        delay(100);
    }
    averAcX = sumAcX / 10;
    averAcY = sumAcY / 10;
    averAcZ = sumAcY / 10;
    averGyX = sumGyX / 10;
    averGyY = sumGyY / 10;
    averGyZ = sumGyZ / 10;
}

void callback(char* topic, byte* payload, unsigned int length) {
    
    int i;
    String Message = "";
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    while (i < length){
      Message += (char)payload [i++];
    } 
    Serial.println();
    Serial.println(Message);

    //wav파일에 따른 동작 작용은 여기다 넣기
}

void setup()
{
    initSensor();
    Serial.begin(115200);
    caliSensor();    //  초기 센서 캘리브레이션 함수 호출
    past = millis(); // past에 현재 시간 저장

    pinMode(speaker,OUTPUT);

    WiFi.mode(WIFI_STA); 
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("Connected to the WiFi network");

    client.setServer(mqttServer, mqttPort);
    while (!client.connected()) {
        Serial.println("Connecting to MQTT...");
        if (client.connect("jj")) {
            Serial.println("connected");  
        } 
        else {
            Serial.print("failed with state "); Serial.println(client.state());
            delay(2000);
        }
    }
    client.subscribe(topic);
    client.setCallback(callback);
}

void loop()
{
    client.loop();
    getData();
    getDT();

    angleAcX = atan(AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2)));
    angleAcX *= RADIAN_TO_DEGREE;
    angleAcY = atan(-AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2)));
    angleAcY *= RADIAN_TO_DEGREE;
    // 가속도 센서로는 Z축 회전각 계산 불가함.

    // 가속도 현재 값에서 초기평균값을 빼서 센서값에 대한 보정
    angleGyX += ((GyX - averGyX) / DEG_PER_SEC) * dt; //각속도로 변환
    angleGyY += ((GyY - averGyY) / DEG_PER_SEC) * dt;
    angleGyZ += ((GyZ - averGyZ) / DEG_PER_SEC) * dt;

    // 상보필터 처리를 위한 임시각도 저장
    double angleTmpX = angleFiX + angleGyX * dt;
    double angleTmpY = angleFiY + angleGyY * dt;
    double angleTmpZ = angleFiZ + angleGyZ * dt;

    // (상보필터 값 처리) 임시 각도에 0.96가속도 센서로 얻어진 각도 0.04의 비중을 두어 현재 각도를 구함.
    angleFiX = ALPHA * angleTmpX + (1.0 - ALPHA) * angleAcX;
    angleFiY = ALPHA * angleTmpY + (1.0 - ALPHA) * angleAcY;
    angleFiZ = ALPHA * angleTmpZ + (1.0 - ALPHA) * angleGyZ; // Z축은 자이로 센서만을 이용하열 구함.

    if (angleGyZ >= 90)
        angleGyZ = 90;
    else if (angleGyZ <= -90)
        angleGyZ = -90;
    if (angleFiZ >= 90)
        angleFiZ = 90;
    if (angleFiZ <= -90)
        angleFiZ = -90;

    Serial.print("\nAngleAcX:");
    Serial.print(angleAcX);
    Serial.print("\t FilteredX:");
    Serial.print(angleFiX);
    Serial.print("\n AngleAcY:");
    Serial.print(angleAcY);
    Serial.print("\t FilteredY:");
    Serial.print(angleFiY);
    Serial.print("\n AngleGyZ:");
    Serial.print(angleGyZ);
    Serial.print("\t FilteredZ:");
    Serial.println(angleFiZ);

    // 파일 경로는 자신의 파일 경로로  변경
    if (angleFiX <= 10)
    {
        if (angleGyZ >= -30 && angleGyZ < 0)
        {
            angle = 1;
        }

        else if (angleGyZ >= 0 && angleGyZ < 30)
        {
            angle = 2;
        }

        else if (angleGyZ >= -90 && angleGyZ < -60)
        {
            angle = 3;
        }

        else if (angleGyZ >= 60 && angleGyZ < 90)
        {
            angle = 4;
        }
    }

    else if (angleFiX <= 45)
    {
        if (angleGyZ >= -60 && angleGyZ < -30)
        {
            angle = 5;
        }

        else if (angleGyZ >= 30 && angleGyZ < 60)
        {
            angle = 6;
        }
    }

    unsigned long currentMillis = millis();
    if(currentMillis - lastPublished >= pubInterval) {
        lastPublished = currentMillis;
        char buf[10];
        sprintf(buf, "%d", angle);
        client.publish("deviceid/mj/evt/angle", buf);
    }
}

//////////////////////////////////////////////////////////////////////////////////////
/* 유튜브 링크 코드 */
// https://www.notion.so/IoT-1ce91dd2e1654c26aa90494557632e4b#9b810ee0da054a07bf10da85282f576b //

// #include <Wire.h>
// // #include "SD.h"
// // #define SD_ChipSelectPin 4
// // #include "TMRpcm.h"
// #include "SPI.h"

// // TMRpcm tmrpcm;

// // int Track[10] = {"1.wav", "2.wav", "3.wav", "4.wav", "5.wav", "6.wav", "7.wav", "8.wav", "9.wav", "10.wav"};
// int x = 5;

// long gyroX, gyroY, gyroZ;
// float rotX, rotY, rotZ, rotX1, rotY1, rotZ1;

// void setupMPU()
// {
//   Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
//   Wire.write(0x6B);                  //Accessing the register 6B - Power Management (Sec. 4.28)
//   Wire.write(0b00000000);            //Setting SLEEP register to 0. (Required; see Note on p. 9)
//   Wire.endTransmission();
//   Wire.beginTransmission(0b1101000); //I2C address of the MPU
//   Wire.write(0x1B);                  //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4)
//   Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s
//   Wire.write(0b00011000); //Setting the gyro to full scale +/- 2000deg./s
//   Wire.endTransmission();
//   Wire.beginTransmission(0b1101000); //I2C address of the MPU
//   Wire.write(0x1C);                  //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5)
//   //Wire.write(0b00000000); //Setting the accel to +/- 2g
//   Wire.write(0b00011000); //Setting the accel to +/- 16g
//   Wire.endTransmission();
// }

// void processGyroData()
// {
//   rotX = (gyroX / (131.0 * (2000 / 250))) - 0.019;
//   rotY = (gyroY / (131.0 * (2000 / 250))) + 0.022;
//   rotZ = (gyroZ / (131.0 * (2000 / 250))) - 0.015;

//   if (rotZ <= 0.01 && rotZ >= -0.01)
//   {
//     rotZ = 0;
//   }

//   if (rotX >= 3 || rotX <= -3){
//     rotZ=0;
//   }

//   rotX1 = rotX ;
//   rotY1 = rotY ;
//   rotZ1 = rotZ ;

//   if (rotX1 >= 360) {
//     rotX1 /= 360;
//   }

//   if (rotY1 >= 360) {
//     rotY1 /= 360;
//   }

//   if (rotZ1 >= 360) {
//     rotZ1 /= 360;
//   }
// }

// void recordGyroRegisters()
// {

//   Wire.beginTransmission(0b1101000);
//   Wire.write(0x43);
//   Wire.endTransmission();
//   Wire.requestFrom(0b1101000, 6);
//   while (Wire.available() < 6)
//     ;
//   gyroX = Wire.read() << 8 | Wire.read();
//   gyroY = Wire.read() << 8 | Wire.read();
//   gyroZ = Wire.read() << 8 | Wire.read();
//   processGyroData();
// }

// void printData()
// {
//   Serial.print("Gyro (deg)");
//   Serial.print(" X=");
//   Serial.print(rotX1);
//   Serial.print(" Y=");
//   Serial.print(rotY1);
//   Serial.print(" Z=");
//   Serial.println(rotZ1);
//   Serial.printf("x: %d\n", x);
// }
// void setup()
// {
//   // tmrpcm.speakerPin = 9;
//   Serial.begin(115200);
//   Wire.begin();
//   setupMPU();
//   // if (!SD.begin(SD_ChipSelectPin)) {
//   // Serial.println("SD fail");
//   // return;
//   // tmrpcm.setVolume(5);
//   //tmrpcm.play("1.wav");
//   // }
// }

// void loop()
// {
//   recordGyroRegisters();

//   if (rotZ1 >= -15 && rotZ1 <= 15)
//   {
//     x = 5;
//     delay(50);
//   }

//   if (rotZ1 >= -30 && rotZ1 <= -16)
//   {
//     x = 6;
//     delay(50);
//   }

//   if (rotZ1 >= -45 && rotZ1 <= -31)
//   {
//     x = 7;
//     delay(50);
//   }

//   if (rotZ1 >= 16 && rotZ1 <= 30)
//   {
//     x = 4;
//     delay(50);
//   }

//   if (rotZ1 >= 31 && rotZ1 <= 45)
//   {
//     x = 3;
//     delay(50);
//   }
//   if (rotX <= -5)
//   {

//     // tmrpcm.play(Track[x]);
//     delay(50);
//   }

//   printData();
//   delay(50);
// }
