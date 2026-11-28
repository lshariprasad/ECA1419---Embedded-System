#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ECA14 Experiment 11 – Task creation, states and priorities.
// Classic ESP32 Dev Module; Serial Monitor: 115200 baud.

SemaphoreHandle_t serialMutex = nullptr;

void printTaskLine(const char *taskName, UBaseType_t priority,
                   uint32_t count, uint32_t delayMs) {
  if (serialMutex != nullptr) {
    xSemaphoreTake(serialMutex, portMAX_DELAY);
  }
  Serial.printf("%s | Priority: %u | Count: %lu | RUNNING -> BLOCKED (%lu ms)\n",
                taskName, (unsigned)priority,
                (unsigned long)count, (unsigned long)delayMs);
  if (serialMutex != nullptr) {
    xSemaphoreGive(serialMutex);
  }
}

void HighTask(void *parameter) {
  uint32_t count = 0;
  const uint32_t delayMs = 500;
  for (;;) {
    ++count;
    printTaskLine("HIGH", uxTaskPriorityGet(nullptr), count, delayMs);
    vTaskDelay(pdMS_TO_TICKS(delayMs));
  }
}

void MediumTask(void *parameter) {
  uint32_t count = 0;
  const uint32_t delayMs = 1000;
  for (;;) {
    ++count;
    printTaskLine("MEDIUM", uxTaskPriorityGet(nullptr), count, delayMs);
    vTaskDelay(pdMS_TO_TICKS(delayMs));
  }
}

void LowTask(void *parameter) {
  uint32_t count = 0;
  const uint32_t delayMs = 1500;
  for (;;) {
    ++count;
    printTaskLine("LOW", uxTaskPriorityGet(nullptr), count, delayMs);
    vTaskDelay(pdMS_TO_TICKS(delayMs));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("========================================");
  Serial.println("FreeRTOS TASK PRIORITY EXPERIMENT");
  Serial.println("========================================");

  serialMutex = xSemaphoreCreateMutex();
  if (serialMutex == nullptr) {
    Serial.println("ERROR: Serial mutex creation failed");
    while (true) delay(1000);
  }

  BaseType_t highOk = xTaskCreate(HighTask, "HighTask", 2048, nullptr, 3, nullptr);
  BaseType_t mediumOk = xTaskCreate(MediumTask, "MediumTask", 2048, nullptr, 2, nullptr);
  BaseType_t lowOk = xTaskCreate(LowTask, "LowTask", 2048, nullptr, 1, nullptr);

  if (highOk != pdPASS || mediumOk != pdPASS || lowOk != pdPASS) {
    Serial.println("ERROR: One or more task creations failed");
    while (true) delay(1000);
  }

  Serial.println("Three tasks created successfully.");
  Serial.println("Priority: HIGH=3, MEDIUM=2, LOW=1");
  Serial.println("Delay: HIGH=500 ms, MEDIUM=1000 ms, LOW=1500 ms");
  Serial.println("State: RUNNING -> BLOCKED -> READY -> RUNNING");
  Serial.println("========================================");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
