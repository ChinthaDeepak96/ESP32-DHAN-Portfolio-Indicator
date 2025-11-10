# ESP32-DHAN-Portfolio-Indicator
IoT-based real-time stock portfolio monitoring system using ESP32, DHAN API, OLED display, and LED indicators. Fetches live profit/loss and visualizes it through color-coded lights and an OLED dashboard.



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
