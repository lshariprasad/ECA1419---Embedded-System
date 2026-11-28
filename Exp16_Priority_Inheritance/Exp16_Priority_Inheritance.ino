#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ECA14 Experiment 16 – Priority inversion / inheritance using a FreeRTOS mutex.
// Tasks are pinned to Core 1, as described in the source manual.
SemaphoreHandle_t resourceMutex = nullptr;
TaskHandle_t LowTaskHandle = nullptr;
TaskHandle_t MediumTaskHandle = nullptr;
TaskHandle_t HighTaskHandle = nullptr;

void LowTask(void *parameter) {
  (void)parameter;
  vTaskDelay(pdMS_TO_TICKS(100));

  Serial.println("LOW -> Trying to lock shared resource");
  if (xSemaphoreTake(resourceMutex, portMAX_DELAY) == pdTRUE) {
    Serial.println("LOW -> Shared resource LOCKED");
    Serial.println("LOW -> Current Priority: 1");

    for (int i = 1; i <= 10; ++i) {
      Serial.print("LOW -> Working... Current Priority: ");
      Serial.println((unsigned)uxTaskPriorityGet(nullptr));
      vTaskDelay(pdMS_TO_TICKS(100));
    }

    Serial.println("LOW -> Releasing shared resource");
    xSemaphoreGive(resourceMutex);
    Serial.print("LOW -> Priority after release: ");
    Serial.println((unsigned)uxTaskPriorityGet(nullptr));
  }

  vTaskDelay(pdMS_TO_TICKS(1000));
  vTaskDelete(nullptr);
}

void MediumTask(void *parameter) {
  (void)parameter;
  vTaskDelay(pdMS_TO_TICKS(400));
  for (;;) {
    Serial.println("MEDIUM -> Running background task");
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void HighTask(void *parameter) {
  (void)parameter;
  vTaskDelay(pdMS_TO_TICKS(300));
  Serial.println("HIGH -> Trying to lock shared resource");

  if (xSemaphoreTake(resourceMutex, portMAX_DELAY) == pdTRUE) {
    Serial.println("HIGH -> Shared resource LOCKED");
    Serial.println("HIGH -> Resource processing complete");
    xSemaphoreGive(resourceMutex);
    Serial.println("HIGH -> Shared resource RELEASED");
  }
  vTaskDelete(nullptr);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("========================================");
  Serial.println(" PRIORITY INHERITANCE TEST");
  Serial.println("========================================");

  resourceMutex = xSemaphoreCreateMutex();
  if (resourceMutex == nullptr) {
    Serial.println("Mutex creation failed!");
    return;
  }
  Serial.println("Mutex created successfully.");

  BaseType_t lowOk = xTaskCreatePinnedToCore(LowTask, "LowTask", 3072, nullptr, 1,
                                             &LowTaskHandle, 1);
  BaseType_t mediumOk = xTaskCreatePinnedToCore(MediumTask, "MediumTask", 3072, nullptr, 2,
                                                &MediumTaskHandle, 1);
  BaseType_t highOk = xTaskCreatePinnedToCore(HighTask, "HighTask", 3072, nullptr, 3,
                                              &HighTaskHandle, 1);

  if (lowOk != pdPASS || mediumOk != pdPASS || highOk != pdPASS) {
    Serial.println("ERROR: One or more task creations failed");
    while (true) delay(1000);
  }

  Serial.println("LOW     Priority = 1");
  Serial.println("MEDIUM Priority = 2");
  Serial.println("HIGH   Priority = 3");
  Serial.println("All tasks are running on Core 1.");
  Serial.println("========================================");
}

void loop() {
  delay(1000);
}
