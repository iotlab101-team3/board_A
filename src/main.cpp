#include <Wire.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define speaker 14

int loopcount = 0;

int angle = 0; // mqtt에 전달하는 값
int action = 0;

const int MPU_ADDR = 0x68;                 // I2C통신을 위한 MPU6050의 주소
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ; // 가속도(Acceleration)와 자이로(Gyro)
double angleAcX, angleAcY, angleAcZ;
double angleGyX, angleGyY, angleGyZ;
double angleFiX, angleFiY, angleFiZ;

const double RADIAN_TO_DEGREE = 180 / 3.14159;
const double DEG_PER_SEC = 32767 / 250; // 1초에 회전하는 각도
const double ALPHA = 1 / (1 + 0.04);

unsigned long now = 0;  // 현재 시간 저장용 변수
unsigned long past = 0; // 이전 시간 저장용 변수
double dt = 0;          // 한 사이클 동안 걸린 시간 변수

double averAcX, averAcY, averAcZ;
double averGyX, averGyY, averGyZ;

const char *ssid = "KT_GiGA_2G_1F1E";        // 희정 : KT_GiGA_2G_1F1E 연빈 : SK_WiFiGIGA4AB4
const char *password = "dcgb2ed245"; // 희정 : dcgb2ed245  연빈: 2009024098
const char *mqttServer = "54.81.240.10";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

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
    // delay(2000);
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
    // delay(2000);
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

void setup()
{
    pinMode(speaker, OUTPUT);

    initSensor();
    Serial.begin(115200);
    caliSensor();    //  초기 센서 캘리브레이션 함수 호출
    past = millis(); // past에 현재 시간 저장

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("Connected to the WiFi network");

    client.setServer(mqttServer, mqttPort);
    while (!client.connected())
    {
        Serial.println("Connecting to MQTT...");
        if (client.connect("team3"))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed with state ");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

void loop()
{
    client.loop();
    getData();
    getDT();

    int lux = analogRead(0);

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

    if (angleFiX >= 90)
        angleFiX = 90;
    if (angleFiX <= -90)
        angleFiX = -90;

    if (angleFiY >= 90)
        angleFiY = 90;
    if (angleFiY <= -90)
        angleFiY = -90;

    Serial.print("\nAngleAcX:");
    Serial.print(angleAcX);

    Serial.print("\n AngleAcY:");
    Serial.print(angleAcY);

    Serial.print("\n AngleGyZ:");
    Serial.print(angleGyZ);

    if (lux < 80)
    {
        angle = 7;
        Serial.printf("angle = %d\n", angle);
        // angleAcX = 0;
        // angleAcY = 0;
        angleGyZ = 0;
        GyZ = 0;
        action = 1;
    }

    else
    {
        if (AcY >= -6000)
        {
            if (angleGyZ >= 60 && angleGyZ <= 90)
            {
                angle = 6;
                Serial.printf("angle = %d\n", angle);
                action = 1;
                // delay(50);
            }

            else if (angleGyZ >= 30 && angleGyZ < 60)
            {
                angle = 5;
                Serial.printf("angle = %d\n", angle);
                action = 1;
                // delay(50);
            }

            else if (angleGyZ >= 0 && angleGyZ < 30)
            {
                angle = 4;
                Serial.printf("angle = %d\n", angle);
                action = 1;
                // delay(50);
            }

            else if (angleGyZ >= -30 && angleGyZ < 0)
            {
                angle = 3;
                Serial.printf("angle = %d\n", angle);
                action = 1;
                // delay(50);
            }

            else if (angleGyZ >= -60 && angleGyZ < -30)
            {
                angle = 2;
                Serial.printf("angle = %d\n", angle);
                action = 1;
                // delay(50);
            }

            else if (angleGyZ >= -90 && angleGyZ < -60)
            {
                angle = 1;
                Serial.printf("angle = %d\n", angle);
                action = 1;
                // delay(50);
            }
        }
        else action = 0;
    }

    Serial.print("현재 값: ");
    Serial.println(angle);
    Serial.printf("action: "); 
    Serial.println(action); 

    loopcount++;
    if (loopcount >= 250 && action == 1)
    {
        char buf[10];
        sprintf(buf, "%d", angle);
        client.publish("deviceid/team3/evt/angle", buf);
        loopcount = 0;
        action = 0;
    }
}