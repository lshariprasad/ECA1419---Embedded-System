#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"

// ECA14 Experiment 14 – Two periodic timers and Event Group synchronization.
EventGroupHandle_t eventGroup = nullptr;
TimerHandle_t timer1 = nullptr;
TimerHandle_t timer2 = nullptr;

#define TIMER1_EVENT (1U << 0)  // BIT 0
#define TIMER2_EVENT (1U << 1)  // BIT 1

void Timer1Callback(TimerHandle_t xTimer) {
  (void)xTimer;
  xEventGroupSetBits(eventGroup, TIMER1_EVENT);
}

void Timer2Callback(TimerHandle_t xTimer) {
  (void)xTimer;
  xEventGroupSetBits(eventGroup, TIMER2_EVENT);
}

void EventTask(void *parameter) {
  (void)parameter;
  for (;;) {
    Serial.println();
    Serial.println("Waiting for BIT 0 and BIT 1...");

    EventBits_t bits = xEventGroupWaitBits(
      eventGroup,
      TIMER1_EVENT | TIMER2_EVENT,
      pdTRUE,        // Clear bits after receiving
      pdTRUE,        // Wait for BOTH bits
      portMAX_DELAY
    );

    Serial.println("Both Timer Events detected!");
    if ((bits & TIMER1_EVENT) != 0) Serial.println("Timer 1 Event -> BIT 0");
    if ((bits & TIMER2_EVENT) != 0) Serial.println("Timer 2 Event -> BIT 1");
    Serial.println("Synchronization complete.");
    Serial.println("------------------------------");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=================================");
  Serial.println(" RTOS TIMER AND EVENT GROUP TEST");
  Serial.println("=================================");

  eventGroup = xEventGroupCreate();
  if (eventGroup == nullptr) {
    Serial.println("ERROR: Event Group creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  timer1 = xTimerCreate("Timer1", pdMS_TO_TICKS(1000), pdTRUE, nullptr, Timer1Callback);
  timer2 = xTimerCreate("Timer2", pdMS_TO_TICKS(2000), pdTRUE, nullptr, Timer2Callback);
  if (timer1 == nullptr || timer2 == nullptr) {
    Serial.println("ERROR: Software timer creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  if (xTaskCreate(EventTask, "EventTask", 2048, nullptr, 1, nullptr) != pdPASS) {
    Serial.println("ERROR: Event task creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  if (xTimerStart(timer1, 0) != pdPASS || xTimerStart(timer2, 0) != pdPASS) {
    Serial.println("ERROR: Could not start one or both timers");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }

  Serial.println("Event Group created.");
  Serial.println("Timer 1 = 1000 ms -> BIT 0");
  Serial.println("Timer 2 = 2000 ms -> BIT 1");
  Serial.println("Both events required for synchronization.");
  Serial.println("=================================");
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
