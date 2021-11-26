// Basic demo for accelerometer readings from Adafruit MPU6050

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <math.h>

Adafruit_MPU6050 mpu;
int DELAY = 200;
double x_rad, y_rad, z_rad;
double x_v, y_v, z_v;
double x_pos, y_pos, z_pos;
double getTime = DELAY / 1000.0;

double xAngle, yAngle, zAngle;
double xAngleInit, yAngleInit, zAngleInit;

const double RADIAN_TO_DEGREE = 180 / 3.14159;

void setup(void)
{
    Serial.begin(115200);
    while (!Serial)
    {
        delay(10); // will pause Zero, Leonardo, etc until serial console opens
    }

    // Try to initialize!
    if (!mpu.begin())
    {
        Serial.println("Failed to find MPU6050 chip");
        while (1)
        {
            delay(10);
        }
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    Serial.println("");
    delay(100);
}

void loop()
{

    /* Get new sensor events with the readings */
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    /* Print out the values */
    // Serial.print("Acceleration[m/s^2]: ");
    // Serial.print(a.acceleration.x);
    // Serial.print(",");
    // Serial.print(a.acceleration.y);
    // Serial.print(",");
    // Serial.print(a.acceleration.z);
    // Serial.print(", ");
    // Serial.println("\n");

    // x_v = x_v + double(a.acceleration.x * getTime);
    // y_v = y_v + double(a.acceleration.x * getTime);
    // z_v = z_v + double(a.acceleration.x * getTime);

    // x_pos = x_pos + (x_v * (DELAY / 1000)) + (0.5 * a.acceleration.x * pow(getTime, 2));
    // y_pos = y_pos + (x_v * (DELAY / 1000)) + (0.5 * a.acceleration.y * pow(getTime, 2));
    // z_pos = z_pos + (x_v * (DELAY / 1000)) + (0.5 * a.acceleration.z * pow(getTime, 2));

    // // Serial.print("Velocity[m/s]: ");
    // Serial.print("Position[m]: ");
    // Serial.print(x_pos);
    // Serial.print(", ");
    // Serial.print(y_pos);
    // Serial.print(", ");
    // Serial.print(z_pos);
    // Serial.println("\n");

    // Acceleration Angle
    xAngle = atan(-a.acceleration.x / sqrt(pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2))) * RADIAN_TO_DEGREE;
    yAngle = atan(-a.acceleration.y / sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.z, 2))) * RADIAN_TO_DEGREE;
    zAngle = atan(-a.acceleration.z / sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2))) * RADIAN_TO_DEGREE;

    xAngleInit = xAngle;
    yAngleInit = yAngle;
    zAngleInit = zAngle;

    Serial.print("Angle[DEGREE]: ");
    Serial.print(xAngle);
    Serial.print(", ");
    Serial.print(yAngle);
    Serial.print(", ");
    Serial.print(zAngle);
    Serial.println("\n");

    // Serial.print("angular speed[rad/s]: ");
    // Serial.print(g.gyro.x);
    // Serial.print(",");
    // Serial.print(g.gyro.y);
    // Serial.print(",");
    // Serial.print(g.gyro.z);
    // Serial.println("\n");

    // x_rad = x_rad + (g.gyro.x * getTime);
    // y_rad = y_rad + (g.gyro.y * getTime);
    // z_rad = z_rad + (g.gyro.z * getTime);

    // Serial.print("angle[rad]: ");
    // Serial.print(x_rad);
    // Serial.print(",");
    // Serial.print(y_rad);
    // Serial.print(",");
    // Serial.print(z_rad);
    // Serial.println("\n");

    delay(DELAY); // 《 자이로 Gyro 센서로 Roll과 Pitch, Yaw의 각도 구하기 실습  》
}
