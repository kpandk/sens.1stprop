typedef struct {
  uint8_t uuid[16];
  int16_t field0;
  int16_t field1;
  int16_t field2;
  //  int16_t field3;
  //  int16_t field4;
  //  int16_t field5;
} AD_DATA_t;

const int ledPin1 =  4;

AD_DATA_t Ad_Data = {
  // f96d0000-1139-4e07-8ccf-d28be904fc0f
  .uuid = {0x0f, 0xfc, 0x04, 0xe9, 0x8b, 0xd2, 0xcf, 0x8c, 0x07, 0x4e, 0x39, 0x11, 0x00, 0x00, 0x6d, 0xf9},
};

void setup() {
  pinMode(ledPin1, OUTPUT);
  
  suspendPeripheralDevices();
  //  Serial.begin(115200);
  //  while (!Serial)
  //    delay(100);
  startAdvertising();
}

float Pressure;
float Temperature1;
float Humidity;
float Temperature2;

void loop() {
  measurePressure();
  measureHumidity();

  //  Serial.printf("T1 %f P %f T2 %f H %f\r\n", Temperature1, Pressure, Temperature2, Humidity);

  Ad_Data.field0 = (int16_t)((Pressure - 1013.25) * 100.0);
  Ad_Data.field1 = (int16_t)(Temperature2 * 100.0);
  Ad_Data.field2 = (int16_t)(Humidity * 100.0);

  updateAdvertisingData();
  digitalWrite(ledPin1, LOW);
  delay(50);
  digitalWrite(ledPin1, HIGH);
  delay(5000);
}