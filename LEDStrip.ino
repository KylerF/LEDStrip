#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <NeoPixelBus.h>

#define LED_COUNT 245
#define LED_PIN 4

const int MIC_PIN = 23;

const int colorSaturation = 128;

const char* ssid = "MySpectrumWiFi58-2G";
const char* password = "kylejoe19";

const unsigned long time_delay = 10;

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(LED_COUNT, LED_PIN);

RgbColor red(colorSaturation, 0, 0);
RgbColor green(0, colorSaturation, 0);
RgbColor blue(0, 0, colorSaturation);
RgbColor white(colorSaturation);
RgbColor black(0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslBlack(black);

void setup() {
  Serial.begin(115200);

  // LED Setup
  strip.Begin();
  strip.Show();

  // WiFi Setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  // **************************************************** /

  // OTA Setup
  ArduinoOTA.setHostname("LightStrip");
  
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // **************************************************** /
}

/**
 * Function called every loop to update the current animation.
 */
void animate() {
  static unsigned long current_millis;
  static unsigned long prev_millis;
  static unsigned long time_passed;

  static bool first_run = true;
  
  static int counter = 0;
  static bool counting_up = true;

  // Update the time
  current_millis = millis();

  // Only run if enough time has passed since we 
  // were here last
  if (current_millis - prev_millis < time_delay && !first_run)
    return;

  // Save the time
  prev_millis = current_millis;
  
  // Flip the counter around if needed
  if (counter >= LED_COUNT)
    counting_up = false;
  else if (counter <= 0 && !first_run)
    counting_up = true;


  if (counting_up) {
    // Set LEDs to a random color one at a time
    int random_r = random(0, colorSaturation);
    int random_g = random(0, colorSaturation);
    int random_b = random(0, colorSaturation); 

    RgbColor random_rgb_color(random_r, random_g, random_b);
    
    strip.SetPixelColor(counter, random_rgb_color);
    strip.Show();
    
    counter += 1;
  } else {
    // Turn off LEDs one at a time
    strip.SetPixelColor(counter, black);
    strip.Show();

    counter -= 1;
  }

  // Remember that we have been here before
  first_run = false;
}

void loop() {
  animate();
  
  ArduinoOTA.handle();
}
