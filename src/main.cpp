// // Basic demo for accelerometer readings from Adafruit MPU6050

// #include <Adafruit_MPU6050.h>
// #include <Adafruit_Sensor.h>
// #include <Wire.h>
// #include <math.h>

// Adafruit_MPU6050 mpu;
// int DELAY = 200;
// double x_rad, y_rad, z_rad;
// double x_v, y_v, z_v;
// double x_pos, y_pos, z_pos;
// double getTime = DELAY / 1000.0;

// double xAngle, yAngle, zAngle;
// double xAngleInit, yAngleInit, zAngleInit;

// const double RADIAN_TO_DEGREE = 180 / 3.14159;

// void setup(void)
// {
//     Serial.begin(115200);
//     while (!Serial)
//     {
//         delay(10); // will pause Zero, Leonardo, etc until serial console opens
//     }

//     // Try to initialize!
//     if (!mpu.begin())
//     {
//         Serial.println("Failed to find MPU6050 chip");
//         while (1)
//         {
//             delay(10);
//         }
//     }

//     mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
//     mpu.setGyroRange(MPU6050_RANGE_250_DEG);
//     mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
//     Serial.println("");
//     delay(100);
// }

// void loop()
// {

//     /* Get new sensor events with the readings */
//     sensors_event_t a, g, temp;
//     mpu.getEvent(&a, &g, &temp);

//     /* Print out the values */
//     // Serial.print("Acceleration[m/s^2]: ");
//     // Serial.print(a.acceleration.x);
//     // Serial.print(",");
//     // Serial.print(a.acceleration.y);
//     // Serial.print(",");
//     // Serial.print(a.acceleration.z);
//     // Serial.print(", ");
//     // Serial.println("\n");

//     // x_v = x_v + double(a.acceleration.x * getTime);
//     // y_v = y_v + double(a.acceleration.x * getTime);
//     // z_v = z_v + double(a.acceleration.x * getTime);

//     // x_pos = x_pos + (x_v * (DELAY / 1000)) + (0.5 * a.acceleration.x * pow(getTime, 2));
//     // y_pos = y_pos + (x_v * (DELAY / 1000)) + (0.5 * a.acceleration.y * pow(getTime, 2));
//     // z_pos = z_pos + (x_v * (DELAY / 1000)) + (0.5 * a.acceleration.z * pow(getTime, 2));

//     // // Serial.print("Velocity[m/s]: ");
//     // Serial.print("Position[m]: ");
//     // Serial.print(x_pos);
//     // Serial.print(", ");
//     // Serial.print(y_pos);
//     // Serial.print(", ");
//     // Serial.print(z_pos);
//     // Serial.println("\n");

//     // Acceleration Angle
//     xAngle = atan(-a.acceleration.x / sqrt(pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2))) * RADIAN_TO_DEGREE;
//     yAngle = atan(-a.acceleration.y / sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.z, 2))) * RADIAN_TO_DEGREE;
//     zAngle = atan(-a.acceleration.z / sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2))) * RADIAN_TO_DEGREE;

//     xAngleInit = xAngle;
//     yAngleInit = yAngle;
//     zAngleInit = zAngle;

//     Serial.print("Angle[DEGREE]: ");
//     Serial.print(xAngle);
//     Serial.print(", ");
//     Serial.print(yAngle);
//     Serial.print(", ");
//     Serial.print(zAngle);
//     Serial.println("\n");

//     // Serial.print("angular speed[rad/s]: ");
//     // Serial.print(g.gyro.x);
//     // Serial.print(",");
//     // Serial.print(g.gyro.y);
//     // Serial.print(",");
//     // Serial.print(g.gyro.z);
//     // Serial.println("\n");

//     // x_rad = x_rad + (g.gyro.x * getTime);
//     // y_rad = y_rad + (g.gyro.y * getTime);
//     // z_rad = z_rad + (g.gyro.z * getTime);

//     // Serial.print("angle[rad]: ");
//     // Serial.print(x_rad);
//     // Serial.print(",");
//     // Serial.print(y_rad);
//     // Serial.print(",");
//     // Serial.print(z_rad);
//     // Serial.println("\n");

//     delay(DELAY); // 《 자이로 Gyro 센서로 Roll과 Pitch, Yaw의 각도 구하기 실습  》
// }

#include <Wire.h>
// #include "SD.h"
// #define SD_ChipSelectPin 4
// #include "TMRpcm.h"
#include "SPI.h"

// TMRpcm tmrpcm;

// int Track[10] = {"1.wav", "2.wav", "3.wav", "4.wav", "5.wav", "6.wav", "7.wav", "8.wav", "9.wav", "10.wav"};
int x = 5;

long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ, rotX1, rotY1, rotZ1;

void setupMPU()
{
    Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
    Wire.write(0x6B);                  //Accessing the register 6B - Power Management (Sec. 4.28)
    Wire.write(0b00000000);            //Setting SLEEP register to 0. (Required; see Note on p. 9)
    Wire.endTransmission();
    Wire.beginTransmission(0b1101000); //I2C address of the MPU
    Wire.write(0x1B);                  //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4)
    Wire.write(0x00000000);            //Setting the gyro to full scale +/- 250deg./s
    Wire.write(0b00011000);            //Setting the gyro to full scale +/- 2000deg./s
    Wire.endTransmission();
    Wire.beginTransmission(0b1101000); //I2C address of the MPU
    Wire.write(0x1C);                  //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5)
    //Wire.write(0b00000000); //Setting the accel to +/- 2g
    Wire.write(0b00011000); //Setting the accel to +/- 16g
    Wire.endTransmission();
}

void processGyroData()
{
    rotX = (gyroX / (131.0 * (2000 / 250))) - 0.019;
    rotY = (gyroY / (131.0 * (2000 / 250))) + 0.022;
    rotZ = (gyroZ / (131.0 * (2000 / 250))) - 0.015;

    if (rotZ <= 0.01 && rotZ >= -0.01)
    {
        rotZ = 0;
    }

    if (rotX >= 3 || rotX <= -3)
    {
        rotZ = 0;
    }

    rotX1 = rotX + rotX + rotX1;
    rotY1 = rotY + rotY + rotY1;
    rotZ1 = rotZ + rotZ + rotZ1;
}

void recordGyroRegisters()
{

    Wire.beginTransmission(0b1101000);
    Wire.write(0x43);
    Wire.endTransmission();
    Wire.requestFrom(0b1101000, 6);
    while (Wire.available() < 6)
        ;
    gyroX = Wire.read() << 8 | Wire.read();
    gyroY = Wire.read() << 8 | Wire.read();
    gyroZ = Wire.read() << 8 | Wire.read();
    processGyroData();
}

void printData()
{
    Serial.print("Gyro (deg)");
    Serial.print(" X=");
    Serial.print(rotX1);
    Serial.print(" Y=");
    Serial.print(rotY1);
    Serial.print(" Z=");
    Serial.println(rotZ1);
}
void setup()
{
    // tmrpcm.speakerPin = 9;
    Serial.begin(115200);
    Wire.begin();
    setupMPU();
    // if (!SD.begin(SD_ChipSelectPin)) {
    // Serial.println("SD fail");
    // return;
    // tmrpcm.setVolume(5);
    //tmrpcm.play("1.wav");
    // }
}

void loop()
{
    recordGyroRegisters();

    if (rotZ1 >= -15 && rotZ1 <= 15)
    {
        x = 5;
        delay(50);
    }

    if (rotZ1 >= -30 && rotZ1 <= -16)
    {
        x = 6;
        delay(50);
    }

    if (rotZ1 >= -45 && rotZ1 <= -31)
    {
        x = 7;
        delay(50);
    }

    if (rotZ1 >= 16 && rotZ1 <= 30)
    {
        x = 4;
        delay(50);
    }

    if (rotZ1 >= 31 && rotZ1 <= 45)
    {
        x = 3;
        delay(50);
    }
    if (rotX <= -5)
    {

        // tmrpcm.play(Track[x]);
        delay(50);
    }

    printData();
    delay(200);
}