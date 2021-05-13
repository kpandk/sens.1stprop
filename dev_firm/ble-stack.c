#include <bluefruit.h>

const char NAME[] = "EnvSensor";
const int TX_POWER = -8; // -8, -4, 0, 4, 8
const float FAST_ADV_INTERVAL = 1.0;
const float SLOW_ADV_INTERVAL = 5.0;

void startAdvertising(void)
{
  // Setup BLE
  Bluefruit.begin();
  Bluefruit.autoConnLed(false);
  Bluefruit.setTxPower(TX_POWER);
  Bluefruit.setName(NAME);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  //  Bluefruit.ScanResponse.addName();

  /* Start Advertising
     - Enable auto advertising if disconnected
     - Interval:  fast mode = 300ms, slow mode = 1s
     - Timeout for fast mode is 60 seconds
     - Start(timeout) with timeout = 0 will advertise forever (until connected)

     For recommended advertising interval
     https://developer.apple.com/library/content/qa/qa1931/_index.html
  */
  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED);
  //  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_NONCONNECTABLE_SCANNABLE_UNDIRECTED);
  Bluefruit.Advertising.addData(BLE_GAP_AD_TYPE_SERVICE_DATA_128BIT_UUID, &Ad_Data, sizeof(Ad_Data));
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(FAST_ADV_INTERVAL / 0.000625, SLOW_ADV_INTERVAL / 0.000625); // in units of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(15);
}

void updateAdvertisingData()
{
  static bool once = true;
  Bluefruit.Advertising.clearData();
  Bluefruit.Advertising.addData(BLE_GAP_AD_TYPE_SERVICE_DATA_128BIT_UUID, &Ad_Data, sizeof(Ad_Data));
  if (once) {
    Bluefruit.Advertising.start(0);
    once = false;
  }
}