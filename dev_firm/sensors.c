#include <Wire.h>

const int LPS22HB = 0x5c;
const int HDC2010 = 0x40;
const int BMX055_ACC = 0x18;
const int BMX055_GYR = 0x68;
const int BMX055_MAG = 0x10;
void suspendPeripheralDevices(void)
{
  Wire.begin();
  regWrite(BMX055_GYR, 0x11, 0x20); // DEEP_SUSPEND mode
  regWrite(BMX055_ACC, 0x11, 0x20); // DEEP_SUSPEND mode
  regWrite(BMX055_MAG, 0x4B, 0x00); // Suspend mode
  regWrite(HDC2010, 0x0e, 0x80);    // Soft-Reset
  regWrite(LPS22HB, 0x10, 0x00);    // Power-down mode
}

void measurePressure(void)
{
  regWrite(LPS22HB, 0x11, 0x11); // One shot enable
  while (true) {
    uint8_t Status;
    regRead(LPS22HB, 0x27, &Status, 1);
    // wait for T_DA==1 and P_DA==1
    if ((Status & 0x3) == 0x3)
    {
      break;
    }
    delay(10);
  }
  uint8_t buf[3];
  regRead(LPS22HB, 0x2B, buf, 2);
  Temperature1 = (buf[0] | buf[1] << 8) / 100.0;
  regRead(LPS22HB, 0x28, buf, 3);
  Pressure = (buf[0] | buf[1] << 8 | buf[2] << 16) / 4096.0;
}

void measureHumidity(void)
{
  regWrite(HDC2010, 0x0E, 0x00); // AMM off
  regWrite(HDC2010, 0x0F, 0x01); // Humidity + Temperature/both 14bit/Start measurement
  while (true) {
    uint8_t Status;
    regRead(HDC2010, 0x0F, &Status, 1);
    if ((Status & 0x01) == 0x00)
    {
      break;
    }
    delay(10);
  }
  uint8_t buf[3];
  regRead(HDC2010, 0x00, buf, 2);
  Temperature2 = ((buf[0] | buf[1] << 8) * 165.0) / 65536.0 - 40;
  regRead(HDC2010, 0x02, buf, 2);
  Humidity = ((buf[0] | buf[1] << 8) * 100.0) / 65536.0;
}

void regWrite(int device, uint8_t addr, uint8_t dat)
{
  Wire.beginTransmission(device);
  Wire.write(addr);
  Wire.write(dat);
  Wire.endTransmission();
}

void regRead(int device, uint8_t addr, uint8_t *buf, uint8_t buflen)
{
  Wire.beginTransmission(device);
  Wire.write(addr);
  Wire.endTransmission(false);
  Wire.requestFrom(device, buflen);
  while (Wire.available() < buflen)
    delay(1);
  for (int i = 0; i < buflen; i++) {
    *buf = Wire.read(); buf++;
  }
}