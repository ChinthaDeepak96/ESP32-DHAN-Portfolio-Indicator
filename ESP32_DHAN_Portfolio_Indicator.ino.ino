// === ESP32 DHAN Portfolio Indicator (Auto Categorized v2) ===
// Hardware: ESP32 WROOM-32, 3-LED traffic light, 0.96" SSD1306 OLED
// Libraries: ArduinoJson, Adafruit_GFX, Adafruit_SSD1306, WiFiClientSecure, HTTPClient

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <vector>
#include <math.h>

// ---------------------------------------------------------
const char* ssid     = "rajulakutumbam";
const char* password = "ledhupora";
const char* DHAN_ACCESS_TOKEN = "eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzUxMiJ9.eyJpc3MiOiJkaGFuIiwicGFydG5lcklkIjoiIiwiZXhwIjoxNzYyODYzODQxLCJpYXQiOjE3NjI3Nzc0NDEsInRva2VuQ29uc3VtZXJUeXBlIjoiU0VMRiIsIndlYmhvb2tVcmwiOiIiLCJkaGFuQ2xpZW50SWQiOiIxMTA4OTA3NDc4In0.TiLyoGTOWecM1OEI54FtLNwxwMpVhUL_8gnPBtj0tn3Of6c_Wcrgc52cbJOwGM50LtOXgtz1jJAhrQXnDPxSlQ";
// ---------------------------------------------------------

const char* DHAN_HOST = "https://api.dhan.co";
const long fetchIntervalMs = 30UL * 1000UL;
const unsigned long categoryDisplayMs = 15UL * 1000UL; // 15s per category

#define PIN_RED    16
#define PIN_YELLOW 17
#define PIN_GREEN   5
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------------------------------------------------
enum Category { CAT_STOCKS, CAT_MUTUAL_FUNDS, CAT_ETFS, CAT_SMALLCASES };

struct Investment {
  Category category;
  String name;
  String securityId;
  double avgPrice;
  double quantity;
  double ltp;
  double pl_amount;
  double pl_percent;
};

std::vector<Investment> investments;
unsigned long lastFetch = 0;

// ---------------------------------------------------------
void setupPins();
void fetchAndProcess();
bool fetchHoldings();
void computePLs();
void setTrafficLEDs(double pl);
void showDisplayCycle();
String getCategoryName(Category c);
Category detectCategory(const String &symbol, const String &exchange);

// ---------------------------------------------------------
void setup() {
  Serial.begin(115200);
  setupPins();

  Serial.println("\nConnecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\n✅ Connected! IP: " + WiFi.localIP().toString());

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("❌ OLED init failed!");
    while (true) delay(1000);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("DHAN Portfolio");
  display.display();

  fetchAndProcess();
  lastFetch = millis();
}

void loop() {
  if (millis() - lastFetch > fetchIntervalMs) {
    fetchAndProcess();
    lastFetch = millis();
  }
  showDisplayCycle();
  delay(200);
}

// ---------------------------------------------------------
void setupPins() {
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  digitalWrite(PIN_RED, LOW);
  digitalWrite(PIN_YELLOW, LOW);
  digitalWrite(PIN_GREEN, LOW);
}

// ---------------------------------------------------------
void fetchAndProcess() {
  Serial.println("Fetching portfolio...");
  investments.clear();
  fetchHoldings();
  computePLs();
  Serial.printf("✅ Total items parsed: %d\n", (int)investments.size());
}

// ---------------------------------------------------------
// Fetch + auto categorize holdings (stocks, ETFs, etc.)
bool fetchHoldings() {
  WiFiClientSecure client; client.setInsecure();
  HTTPClient https;
  https.begin(client, String(DHAN_HOST) + "/v2/holdings");
  https.addHeader("access-token", DHAN_ACCESS_TOKEN);

  int code = https.GET();
  if (code != 200) {
    Serial.printf("❌ HTTP error: %d\n", code);
    https.end();
    return false;
  }

  String payload = https.getString();
  https.end();

  Serial.println("---- Raw Holdings JSON ----");
  Serial.println(payload);
  Serial.println("---------------------------");

  DynamicJsonDocument doc(32768);
  if (deserializeJson(doc, payload)) {
    Serial.println("JSON parse error");
    return false;
  }

  JsonArray arr = doc.as<JsonArray>();
  if (arr.isNull()) return false;

  for (JsonObject o : arr) {
    Investment inv;
    inv.name        = o["tradingSymbol"] | "";
    inv.securityId  = o["securityId"] | "";
    inv.avgPrice    = o["avgCostPrice"] | 0.0;
    inv.quantity    = o["totalQty"] | o["dpQty"] | 0.0;
    inv.ltp         = o["lastTradedPrice"] | 0.0;
    String exchange = o["exchange"] | "";

    inv.category = detectCategory(inv.name, exchange);
    inv.pl_amount   = (inv.ltp - inv.avgPrice) * inv.quantity;
    inv.pl_percent  = (inv.avgPrice > 0) ? ((inv.ltp - inv.avgPrice) / inv.avgPrice) * 100.0 : 0.0;

    if (inv.quantity > 0 && inv.avgPrice > 0) investments.push_back(inv);

    Serial.printf("%s | %s | Qty=%.0f | Avg=%.2f | LTP=%.2f | P/L=%.0f (%.2f%%)\n",
                  getCategoryName(inv.category).c_str(),
                  inv.name.c_str(),
                  inv.quantity, inv.avgPrice, inv.ltp,
                  inv.pl_amount, inv.pl_percent);
  }
  return true;
}

// ---------------------------------------------------------
// Auto-detect category by name/exchange
Category detectCategory(const String &symbol, const String &exchange) {
  String sym = symbol; sym.toUpperCase();
  String ex  = exchange; ex.toUpperCase();

  if (sym.indexOf("BEES") >= 0 || sym.indexOf("ETF") >= 0 || sym.indexOf("GOLD") >= 0)
    return CAT_ETFS;
  else if (ex.indexOf("MF") >= 0 || sym.indexOf("MF") >= 0)
    return CAT_MUTUAL_FUNDS;
  else
    return CAT_STOCKS;
}

// ---------------------------------------------------------
void computePLs() {
  double totalPL = 0, totalInvested = 0;
  for (auto &inv : investments) {
    if (inv.quantity <= 0 || inv.avgPrice <= 0) continue;
    if (inv.ltp == 0) inv.ltp = inv.avgPrice;

    double investedValue = inv.avgPrice * inv.quantity;
    inv.pl_amount = (inv.ltp - inv.avgPrice) * inv.quantity;
    inv.pl_percent = investedValue > 0 ? (inv.pl_amount / investedValue) * 100.0 : 0;
    totalPL += inv.pl_amount;
    totalInvested += investedValue;
  }
  Serial.printf("TOTAL: Invested=%.0f  P/L=%.0f (%.2f%%)\n",
                round(totalInvested), round(totalPL),
                (totalInvested>0 ? (totalPL/totalInvested*100.0):0));
}

// ---------------------------------------------------------
// LED Logic: Green=Profit, Red=Loss, Yellow=Neutral
// ---------------------------------------------------------
void setTrafficLEDs(double pl) {
  digitalWrite(PIN_RED, LOW);
  digitalWrite(PIN_YELLOW, LOW);
  digitalWrite(PIN_GREEN, LOW);

  if (pl > 0) digitalWrite(PIN_GREEN, HIGH);
  else if (pl < 0) digitalWrite(PIN_RED, HIGH);
  else digitalWrite(PIN_YELLOW, HIGH);
}

// ---------------------------------------------------------
void showDisplayCycle() {
  static unsigned long last = 0;
  static int categoryIndex = -1;
  if (millis() - last < categoryDisplayMs) return;
  last = millis();
  categoryIndex++;
  if (categoryIndex > 3) categoryIndex = 0;

  double invested = 0, pl = 0;
  for (auto &inv : investments)
    if (inv.category == categoryIndex) {
      invested += inv.avgPrice * inv.quantity;
      pl += inv.pl_amount;
    }

  double current = invested + pl;

  // Update LED for this category
  setTrafficLEDs(pl);

  // Show on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.printf("%s Summary\n", getCategoryName((Category)categoryIndex).c_str());
  display.printf("Invested : ₹%.0f\n", round(invested));
  display.printf("Current  : ₹%.0f\n", round(current));
  display.printf("P/L ₹    : %.0f\n", round(pl));
  display.printf("P/L %%    : %.2f\n", invested>0 ? (pl/invested*100.0) : 0.0);
  display.display();

  Serial.printf("🖥 %s | Invested=₹%.0f | Current=₹%.0f | P/L=₹%.0f\n",
                getCategoryName((Category)categoryIndex).c_str(),
                round(invested), round(current), round(pl));
}

// ---------------------------------------------------------
String getCategoryName(Category c) {
  switch (c) {
    case CAT_STOCKS: return "Stocks";
    case CAT_MUTUAL_FUNDS: return "Mutual Funds";
    case CAT_ETFS: return "ETFs";
    case CAT_SMALLCASES: return "Smallcases";
    default: return "Unknown";
  }
}
