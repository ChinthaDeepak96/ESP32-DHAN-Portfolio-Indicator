# IoT-Based Real-Time Stock Portfolio Indicator using ESP32 and DHAN API

## 📘 Overview
This project connects an ESP32 microcontroller with the DHAN Developer API to monitor real-time portfolio profit/loss. It visually indicates portfolio status using traffic LEDs (Green/Yellow/Red) and displays live values on an OLED screen.

---

## ⚙️ Features
- Fetches **real-time stock, ETF, and mutual fund data** from DHAN API  
- Displays **Invested, Current, and P/L values** on OLED  
- LED status indicators for profit/loss/neutral  
- Automatic category cycling every 15s  
- Secure HTTPS requests with `WiFiClientSecure`  
- Written fully in **Arduino (C++)** for ESP32  

---

## 🧠 Hardware Components
| Component | Description |
|------------|-------------|
| ESP32-WROOM-32 | Wi-Fi microcontroller |
| SSD1306 OLED (0.96”) | Display module |
| Traffic Light LED (3-color) | Profit/Loss indication |
| Breadboard, Jumpers, Resistors | Connections |
| Wi-Fi Router | Internet for API |

---

## 🧮 Calculation Logic
| Metric | Formula |
|--------|----------|
| Invested Value | avgPrice × quantity |
| Profit/Loss | (ltp - avgPrice) × quantity |
| Current Value | Invested + P/L |
| P/L % | (P/L ÷ Invested) × 100 |

---

## 🖥 Output Example

ETFs Summary
Invested : ₹2280787
Current : ₹2237660
P/L ₹ : -43127
P/L % : -1.89


- 🟢 **Green LED** → Profit  
- 🔴 **Red LED** → Loss  
- 🟡 **Yellow LED** → Neutral  

---

## 🧰 Libraries Used
- `WiFi.h`
- `WiFiClientSecure.h`
- `HTTPClient.h`
- `ArduinoJson.h`
- `Adafruit_GFX.h`
- `Adafruit_SSD1306.h`

---

## 📷 Demo & Results
Include screenshots and video:
- OLED display showing category summary  
- LEDs changing color based on P/L  
- Serial output showing live API data  

---

## 🧾 Report
Full project documentation:  
📄 [`ESP32_DHAN_Portfolio_Project_Report.pdf`](report/ESP32_DHAN_Portfolio_Project_Report.pdf)

---

## 🧩 Future Improvements
- Auto token refresh via cloud function  
- Telegram/Email alerts  
- Voice-based status report  
- Integration with Google Home or Alexa  

---

## 👨‍💻 Author
**Chintha Deepak**  
B.Tech CSE (AI & Robotics)  
💼 https://www.linkedin.com/in/chinthadeepak2305/
📧 deepakchintha2305@gmail.com
