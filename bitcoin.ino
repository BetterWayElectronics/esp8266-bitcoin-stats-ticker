#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char *ssid     = "YOUR SSID";
const char *password = "YOUR PASSWORD";

//Change 0x27 as required!
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 
HTTPClient http;
String payload;
int start_idx;
int end_idx ;
String prc;
int price;
int price_usd;
int old_price;
int old_price_usd;
int key_blink = 0;
int ok_flag = 0;
int value;
int old_value;

byte Heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

byte Skull[] = {
  B00000,
  B01110,
  B10101,
  B11011,
  B01110,
  B01110,
  B00000,
  B00000
};

byte ArrowUp[] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00100
};

byte ArrowDown[] = {
  B00100,
  B00100,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100
};

byte Blink3[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

byte Blink4[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

void setup() {
  WiFi.waitForConnectResult();
  Serial.begin(115200);
  lcd.begin(0, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bitcoin Status");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  lcd.createChar(0, Heart);
  lcd.createChar(1, Skull);
  lcd.createChar(2, ArrowUp);
  lcd.createChar(3, ArrowDown);
  lcd.createChar(4, Blink3);
  lcd.createChar(5, Blink4);
  delay(1000);
  lcd.clear();
}

void loop() {
  WiFi.waitForConnectResult();

  http.begin("http://api.coindesk.com/v1/bpi/currentprice/AUD.json");
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    payload = http.getString();
    start_idx = payload.lastIndexOf("rate_float") + 12;
    end_idx = payload.lastIndexOf("}}}");
    prc = payload.substring(start_idx, end_idx);
    price = (float)prc.toInt();
    
//If you want to also display USD
    http.begin("http://api.coindesk.com/v1/bpi/currentprice/USD.json");
    int httpCode = http.GET();
    payload = http.getString();
    start_idx = payload.lastIndexOf("rate_float") + 12;
    end_idx = payload.lastIndexOf("}}}");
    prc = payload.substring(start_idx, end_idx);
    price_usd = (float)prc.toInt();

//Blinky (Serves no real purpose)
    lcd.setCursor(15, 0);
    lcd.write(key_blink == 0 ? byte(0) : byte(4));
    lcd.setCursor(15, 1);
    lcd.write(key_blink == 0 ? byte(0) : byte(4));
    //lcd.print(key_blink == 0 ? "-" : "_");
    key_blink = 1 - key_blink;

//How much have I got? Either use an API or just put the value here.
    value = 0.1337 * price;
    
//Value Status Icon
    if (old_value < value) 
    {
      lcd.setCursor(14, 0);
      lcd.write(byte(2));
    }
    if (old_value > value) 
    {
       lcd.setCursor(14, 0);
       lcd.write(byte(3));
    }
    
//Price Status Icon
    if (old_price < price) 
    {
      lcd.setCursor(14, 1);
      lcd.write(byte(2));
    }
    if (old_price > price) 
    {
       lcd.setCursor(14, 1);
       lcd.write(byte(3));
    }

// Main Function!
    if (old_price != price) {
      lcd.setCursor(0, 0);
      lcd.print("Funds =");
      lcd.setCursor(9, 0);
      lcd.print(value);
      lcd.setCursor(0, 1);
      lcd.print("1 BTC = ");
      lcd.setCursor(8, 1);
      lcd.print(price);
      
      old_price = price;
      old_value = value;
    }
  }
  else {
    lcd.setCursor(13, 0);
    lcd.print("ERR");
  }
  delay(1500);
}
