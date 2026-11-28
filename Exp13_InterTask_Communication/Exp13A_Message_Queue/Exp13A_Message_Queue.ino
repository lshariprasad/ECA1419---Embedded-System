#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// ECA14 Experiment 13A – FIFO message queue (5 uint32_t items).
QueueHandle_t dataQueue = nullptr;

void ProducerTask(void *pvParameters) {
  uint32_t data = 100;
  for (;;) {
    data += 10;
    if (xQueueSend(dataQueue, &data, pdMS_TO_TICKS(100)) == pdPASS) {
      Serial.printf("[PRODUCER] Data sent: %lu\n", (unsigned long)data);
    } else {
      Serial.println("[PRODUCER] Queue full; data not sent");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void ConsumerTask(void *pvParameters) {
  uint32_t receivedData;
  for (;;) {
    if (xQueueReceive(dataQueue, &receivedData, portMAX_DELAY) == pdPASS) {
      Serial.printf("[CONSUMER] Data received: %lu\n", (unsigned long)receivedData);
      Serial.println("---------------------------");
      vTaskDelay(pdMS_TO_TICKS(1500));
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("PART A: FREERTOS MESSAGE QUEUE");

  dataQueue = xQueueCreate(5, sizeof(uint32_t));
  if (dataQueue == nullptr) {
    Serial.println("ERROR: Queue creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }
  Serial.println("Message queue created successfully");

  if (xTaskCreate(ProducerTask, "Producer", 4096, nullptr, 2, nullptr) != pdPASS ||
      xTaskCreate(ConsumerTask, "Consumer", 4096, nullptr, 1, nullptr) != pdPASS) {
    Serial.println("ERROR: Task creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
