/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include <bluefruit.h>

// Beacon uses the Manufacturer Specific Data field in the advertising
// packet, which means you must provide a valid Manufacturer ID. Update
// the field below to an appropriate value. For a list of valid IDs see:
// https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers
// 0x004C is Apple (for example)
#define MANUFACTURER_ID   0x004C 

// AirLocate UUID: E2C56DB5-DFFB-48D2-B060-D0F5A71096E0
uint8_t beaconUuid[16] = 
{ 
  0xE2, 0xC5, 0x6D, 0xB5, 0xDF, 0xFB, 0x48, 0xD2, 
  0xB0, 0x60, 0xD0, 0xF5, 0xA7, 0x10, 0x96, 0xE0, 
};

// A valid Beacon packet consists of the following information:
// UUID, Major, Minor, RSSI @ 1M
BLEBeacon beacon(beaconUuid, 0x0000, 0x0000, -40);


// This function sets up the power amplifier and the LNA on the RFCAT N32 radio
static void pa_lna_assist()
{

uint32_t err_code;

    nrf_gpio_cfg_output(23);
    nrf_gpio_pin_clear(23); //
    nrf_gpio_cfg_output(22);
    nrf_gpio_pin_clear(22); // 
   
    // Configure SoftDevice PA/LNA assist
    ble_opt_t opt;
    memset(&opt, 0, sizeof(ble_opt_t));
    // Common PA/LNA config
    opt.common_opt.pa_lna.gpiote_ch_id  = 0;                      // GPIOTE channel
    opt.common_opt.pa_lna.ppi_ch_id_clr = 1;                      // PPI channel for pin clearing
    opt.common_opt.pa_lna.ppi_ch_id_set = 0;                      // PPI channel for pin setting
    
    // PA config
    opt.common_opt.pa_lna.pa_cfg.active_high = 1;                 // Set the pin to be active high
    opt.common_opt.pa_lna.pa_cfg.enable      = 1;                 // Enable toggling
    opt.common_opt.pa_lna.pa_cfg.gpio_pin    = 23;                // The GPIO pin to toggle
  
    // LNA config
    opt.common_opt.pa_lna.lna_cfg.active_high  = 1;               // Set the pin to be active high
    opt.common_opt.pa_lna.lna_cfg.enable       = 1;               // Enable toggling
    opt.common_opt.pa_lna.lna_cfg.gpio_pin     = 22;              // The GPIO pin to toggle

    err_code = sd_ble_opt_set(BLE_COMMON_OPT_PA_LNA, &opt);
    Serial.println(err_code);
}



void setup() 
{


  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("RFCAT N32 Beacon Example");
  Serial.println("--------------------------\n");

  Bluefruit.begin();

  // off Blue LED for lowest power consumption
  Bluefruit.autoConnLed(false);
  Bluefruit.setTxPower(0);    // Check bluefruit.h for supported values - The RFCAT N32 power amplifer adds 18.9dBm to this value
  Bluefruit.setName("RFCAT N32");

  // Manufacturer ID is required for Manufacturer Specific Data
  beacon.setManufacturer(MANUFACTURER_ID);
  
  pa_lna_assist(); // must be run after Bluefruit.begin

  // Setup the advertising packet
  startAdv();


  Serial.println("Broadcasting beacon, open your beacon app to test");
  // Suspend Loop() to save power, since we didn't have any code there
  suspendLoop();
}

void startAdv(void)
{  
  // Advertising packet
  // Set the beacon payload using the BLEBeacon class populated
  // earlier in this example
  Bluefruit.Advertising.setBeacon(beacon);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * Apple Beacon specs
   * - Type: Non connectable, undirected
   * - Fixed interval: 100 ms -> fast = slow = 100 ms
   */
//  Bluefruit.Advertising.setType(BLE_GAP_ADV_TYPE_ADV_NONCONN_IND);
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(160, 160);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void loop() 
{
  // loop is already suspended, CPU will not run loop() at all
}
