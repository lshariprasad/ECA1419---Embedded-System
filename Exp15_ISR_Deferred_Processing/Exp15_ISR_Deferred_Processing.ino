#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ECA14 Experiment 15 – GPIO0/BOOT interrupt with deferred task processing.
#define BUTTON_PIN 0

TaskHandle_t InterruptTaskHandle = nullptr;
volatile uint32_t interruptCount = 0;

void IRAM_ATTR ButtonISR() {
  ++interruptCount;
  BaseType_t higherPriorityTaskWoken = pdFALSE;
  if (InterruptTaskHandle != nullptr) {
    vTaskNotifyGiveFromISR(InterruptTaskHandle, &higherPriorityTaskWoken);
    if (higherPriorityTaskWoken == pdTRUE) {
      portYIELD_FROM_ISR();
    }
  }
}

void InterruptTask(void *parameter) {
  (void)parameter;
  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    uint32_t countSnapshot = interruptCount;
    Serial.print("HIGH PRIORITY TASK -> Interrupt Count: ");
    Serial.println((unsigned long)countSnapshot);
    Serial.println("Deferred processing executed.");
    Serial.println("------------------------------");
  }
}

void BackgroundTask(void *parameter) {
  (void)parameter;
  for (;;) {
    Serial.println("LOW PRIORITY TASK -> Background processing");
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.println("=================================");
  Serial.println(" ISR PRIORITY AND DEFERRED PROCESSING");
  Serial.println("=================================");

  if (xTaskCreate(InterruptTask, "InterruptTask", 2048, nullptr, 3,
                  &InterruptTaskHandle) != pdPASS) {
    Serial.println("ERROR: Interrupt task creation failed");
    while (true) delay(1000);
  }

  if (xTaskCreate(BackgroundTask, "BackgroundTask", 2048, nullptr, 1,
                  nullptr) != pdPASS) {
    Serial.println("ERROR: Background task creation failed");
    while (true) delay(1000);
  }

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ButtonISR, FALLING);
  Serial.println("Interrupt configured on GPIO 0.");
  Serial.println("Interrupt Task Priority = 3");
  Serial.println("Background Task Priority = 1");
  Serial.println("Press the BOOT button.");
  Serial.println("=================================");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
