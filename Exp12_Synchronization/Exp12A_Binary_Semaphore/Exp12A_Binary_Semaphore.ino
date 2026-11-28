#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ECA14 Experiment 12A – Binary semaphore event signaling.
SemaphoreHandle_t binarySemaphore = nullptr;

void BinaryProducerTask(void *pvParameters) {
  int eventNumber = 0;
  for (;;) {
    ++eventNumber;
    Serial.printf("[BINARY] Producer generated event %d\n", eventNumber);
    if (xSemaphoreGive(binarySemaphore) == pdTRUE) {
      Serial.println("[BINARY] Event signal sent");
    } else {
      Serial.println("[BINARY] Signal not added; semaphore already full");
    }
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

void BinaryConsumerTask(void *pvParameters) {
  for (;;) {
    if (xSemaphoreTake(binarySemaphore, portMAX_DELAY) == pdTRUE) {
      Serial.println("[BINARY] Consumer received signal");
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("BINARY SEMAPHORE EVENT SIGNALING");

  binarySemaphore = xSemaphoreCreateBinary();
  if (binarySemaphore == nullptr) {
    Serial.println("ERROR: Binary semaphore creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  if (xTaskCreate(BinaryProducerTask, "BinProd", 4096, nullptr, 2, nullptr) != pdPASS ||
      xTaskCreate(BinaryConsumerTask, "BinCons", 4096, nullptr, 1, nullptr) != pdPASS) {
    Serial.println("ERROR: Task creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
