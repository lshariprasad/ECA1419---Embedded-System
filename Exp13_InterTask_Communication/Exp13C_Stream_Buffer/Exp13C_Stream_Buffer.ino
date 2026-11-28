#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"

// ECA14 Experiment 13C – Byte stream / pipe using a Stream Buffer.
StreamBufferHandle_t streamBuffer = nullptr;

void ProducerTask(void *pvParameters) {
  uint8_t sequenceNumber = 0;
  for (;;) {
    char message[6];
    message[0] = 'A' + (sequenceNumber % 26);
    message[1] = 'A' + ((sequenceNumber + 1) % 26);
    message[2] = 'A' + ((sequenceNumber + 2) % 26);
    message[3] = 'A' + ((sequenceNumber + 3) % 26);
    message[4] = '\n';
    message[5] = '\0';

    size_t bytesSent = xStreamBufferSend(streamBuffer, message, 5, pdMS_TO_TICKS(100));
    if (bytesSent == 5) {
      Serial.printf("[PIPE] Producer sent: %s", message);
    } else {
      Serial.printf("[PIPE] Partial send: %u bytes\n", (unsigned)bytesSent);
    }
    ++sequenceNumber;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void ConsumerTask(void *pvParameters) {
  char receivedData[17];
  for (;;) {
    size_t bytesReceived = xStreamBufferReceive(
      streamBuffer, receivedData, sizeof(receivedData) - 1, portMAX_DELAY);
    if (bytesReceived > 0) {
      receivedData[bytesReceived] = '\0';
      Serial.printf("[PIPE] Consumer received %u bytes: %s",
                    (unsigned)bytesReceived, receivedData);
      Serial.println("---------------------------");
      vTaskDelay(pdMS_TO_TICKS(1500));
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("PART C: FREERTOS STREAM BUFFER");

  streamBuffer = xStreamBufferCreate(32, 1);
  if (streamBuffer == nullptr) {
    Serial.println("ERROR: Stream buffer creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }
  Serial.println("Stream buffer created successfully");

  if (xTaskCreate(ProducerTask, "Producer", 4096, nullptr, 2, nullptr) != pdPASS ||
      xTaskCreate(ConsumerTask, "Consumer", 4096, nullptr, 1, nullptr) != pdPASS) {
    Serial.println("ERROR: Task creation failed");
    while (true) vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
