#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ECA14 Experiment 12B – Counting semaphore (maximum 3, initial 0).
SemaphoreHandle_t countingSemaphore = nullptr;

void CountingProducerTask(void *pvParameters) {
  int eventNumber = 0;
  for (;;) {
    ++eventNumber;
    if (xSemaphoreGive(countingSemaphore) == pdTRUE) {
      Serial.printf("[COUNTING] Event %d generated; count = %lu\n",
                    eventNumber,
                    (unsigned long)uxSemaphoreGetCount(countingSemaphore));
    } else {
      Serial.println("[COUNTING] Semaphore full; signal not added");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void CountingConsumerTask(void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(countingSemaphore, portMAX_DELAY) == pdTRUE) {
      Serial.printf("[COUNTING] Signal consumed; remaining = %lu\n",
                    (unsigned long)uxSemaphoreGetCount(countingSemaphore));
      vTaskDelay(pdMS_TO_TICKS(2500));
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("COUNTING SEMAPHORE");

  countingSemaphore = xSemaphoreCreateCounting(3, 0);
  if (countingSemaphore == nullptr) {
    Serial.println("ERROR: Counting semaphore creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  if (xTaskCreate(CountingProducerTask, "CntProd", 4096, nullptr, 2, nullptr) != pdPASS ||
      xTaskCreate(CountingConsumerTask, "CntCons", 4096, nullptr, 1, nullptr) != pdPASS) {
    Serial.println("ERROR: Task creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
