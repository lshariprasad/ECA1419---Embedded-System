#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ECA14 Experiment 12C – Mutex for shared data protection.
SemaphoreHandle_t counterMutex = nullptr;
int sharedCounter = 0;

void CounterTask1(void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(counterMutex, portMAX_DELAY) == pdTRUE) {
      ++sharedCounter;
      Serial.printf("[MUTEX] Task 1 counter = %d\n", sharedCounter);
      xSemaphoreGive(counterMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void CounterTask2(void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(counterMutex, portMAX_DELAY) == pdTRUE) {
      ++sharedCounter;
      Serial.printf("[MUTEX] Task 2 counter = %d\n", sharedCounter);
      xSemaphoreGive(counterMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(700));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("MUTEX - SHARED DATA PROTECTION");

  counterMutex = xSemaphoreCreateMutex();
  if (counterMutex == nullptr) {
    Serial.println("ERROR: Mutex creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  if (xTaskCreate(CounterTask1, "Counter1", 4096, nullptr, 1, nullptr) != pdPASS ||
      xTaskCreate(CounterTask2, "Counter2", 4096, nullptr, 1, nullptr) != pdPASS) {
    Serial.println("ERROR: Task creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
