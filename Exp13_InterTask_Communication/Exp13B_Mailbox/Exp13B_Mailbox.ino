#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// ECA14 Experiment 13B – Mailbox-style queue (one uint32_t item).
// FreeRTOS mailbox is represented by a queue of length one.
QueueHandle_t mailbox = nullptr;

void ProducerTask(void *pvParameters) {
  uint32_t message = 1000;
  for (;;) {
    message += 100;
    if (xQueueSend(mailbox, &message, pdMS_TO_TICKS(500)) == pdPASS) {
      Serial.printf("[MAILBOX] Message sent: %lu\n", (unsigned long)message);
    } else {
      Serial.println("[MAILBOX] Slot occupied; message not sent");
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void ConsumerTask(void *pvParameters) {
  uint32_t receivedMessage;
  for (;;) {
    if (xQueueReceive(mailbox, &receivedMessage, portMAX_DELAY) == pdPASS) {
      Serial.printf("[MAILBOX] Message received: %lu\n", (unsigned long)receivedMessage);
      Serial.println("---------------------------");
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("PART B: FREERTOS MAILBOX");

  mailbox = xQueueCreate(1, sizeof(uint32_t));
  if (mailbox == nullptr) {
    Serial.println("ERROR: Mailbox creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }
  Serial.println("One-item mailbox created successfully");

  if (xTaskCreate(ProducerTask, "Producer", 4096, nullptr, 2, nullptr) != pdPASS ||
      xTaskCreate(ConsumerTask, "Consumer", 4096, nullptr, 1, nullptr) != pdPASS) {
    Serial.println("ERROR: Task creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
