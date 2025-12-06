# STM32HttpClient
A lightweight HTTP client implementation for STM32 (HAL-based). Supports WiFi modules (ESP8266/ESP01) for sending GET/POST requests without external libraries. Fully documented &amp; easy to integrate into any STM32 project.

---

## 📌 Overview

STM32HttpClient is a minimal and efficient implementation that enables STM32 microcontrollers to perform HTTP GET and POST requests over WiFi.  
Communication is handled through AT commands, making it compatible with ESP8266/ESP01 modules using standard firmware.

This repository contains **two separate projects**:

---

## 📁 Project Structure

### **1️⃣ Backend (.NET Project)**
Located in the folder:  
`/Backend/`

- A lightweight .NET backend used for testing HTTP communication.
- Includes a simple API endpoint for receiving POST/GET requests from STM32.
- Great for debugging, logging, and real-world testing.

---

### **2️⃣ STM32 CubeIDE Project**
Located in:  
`/STM32HttpClient/`

- Main firmware project for STM32f103c8t6.
- Includes full implementation of:
  - WiFi initialization (ESP AT commands)
  - HTTP GET & POST requests
  - Timeout handling
  - UART communication
  - Response parsing
- Fully based on **HAL drivers** (no FreeRTOS).

---

## 📦 Hardware Library Package
A ZIP file containing STM32-related hardware support files is included inside the repo:  
`/HardwareLibrary/HardwareLib.zip`

This library includes:
- Utility functions  
- HAL-layer helper modules  
- UART communication helpers  
- Ready-to-use tools for integrating ESP modules with STM32  

---

## 🚀 Features

- HTTP **GET** & **POST** support  
- Works with **ESP8266 / ESP01 (AT Firmware)**  
- Simple integration — just drop `.c/.h` files  
- Clean, documented and minimal  
- No external libraries or RTOS required  
- Perfect for IoT and embedded cloud-based devices  

---

## 🧪 Testing
The included .NET backend is designed specifically for testing:  
- Local WiFi requests  
- Response validation  
- Payload debugging  

---

## 💡 Usage
Clone the repository and open both projects.
## Notes
For the UART library to work properly, replace HAL’s default UART interrupt handlers with Uart_isr() in stm32f1xx_it.c:
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  //HAL_UART_IRQHandler(&huart1);
  Uart_isr(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
