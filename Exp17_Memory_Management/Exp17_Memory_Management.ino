#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ECA14 Experiment 17 – FreeRTOS heap, stack high-water mark and fixed memory pool.
#define BLOCK_SIZE 32
#define NUM_BLOCKS 4

uint8_t pool[NUM_BLOCKS][BLOCK_SIZE];
bool used[NUM_BLOCKS] = {false, false, false, false};

int allocateBlock() {
  for (int i = 0; i < NUM_BLOCKS; ++i) {
    if (!used[i]) {
      used[i] = true;
      return i;
    }
  }
  return -1;
}

void releaseBlock(int block) {
  if (block >= 0 && block < NUM_BLOCKS) {
    used[block] = false;
  }
}

void MemoryTask(void *parameter) {
  (void)parameter;

  // Heap allocation / release test.
  Serial.println("\n--- HEAP MEMORY ---");
  size_t before = ESP.getFreeHeap();
  int *data = (int *)pvPortMalloc(10 * sizeof(int));

  Serial.print("Heap before : ");
  Serial.println((unsigned long)before);
  if (data != nullptr) {
    for (int i = 0; i < 10; ++i) data[i] = i;
    size_t after = ESP.getFreeHeap();
    Serial.print("Heap after : ");
    Serial.println((unsigned long)after);
    Serial.print("Heap reduction : ");
    Serial.println((unsigned long)(before - after));
    vPortFree(data);
    Serial.print("Heap after release : ");
    Serial.println((unsigned long)ESP.getFreeHeap());
  } else {
    Serial.println("ERROR: pvPortMalloc allocation failed");
  }

  // Stack high-water mark.
  Serial.println("\n--- STACK HIGH-WATER MARK ---");
  Serial.print("Minimum remaining stack (raw FreeRTOS units): ");
  Serial.println((unsigned long)uxTaskGetStackHighWaterMark(nullptr));

  // Fixed-size memory pool test.
  Serial.println("\n--- MEMORY POOL ---");
  int blocks[NUM_BLOCKS];
  Serial.print("Pool size : ");
  Serial.print(NUM_BLOCKS * BLOCK_SIZE);
  Serial.println(" bytes");

  for (int i = 0; i < NUM_BLOCKS; ++i) {
    used[i] = false;
    blocks[i] = allocateBlock();
    if (blocks[i] >= 0) {
      Serial.printf("Block %d allocated\n", i + 1);
    } else {
      Serial.printf("Block %d allocation failed\n", i + 1);
    }
  }

  if (allocateBlock() == -1) {
    Serial.println("Block 5 : FAILED - Pool FULL");
  }

  for (int i = 0; i < NUM_BLOCKS; ++i) releaseBlock(blocks[i]);
  Serial.println("All blocks released");

  int test = allocateBlock();
  if (test >= 0) {
    Serial.println("Block successfully reallocated");
    releaseBlock(test);
  } else {
    Serial.println("ERROR: Memory pool reuse failed");
  }

  Serial.println("\n--- MEMORY TEST COMPLETE ---");
  vTaskDelete(nullptr);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("================================");
  Serial.println(" RTOS MEMORY MANAGEMENT TEST");
  Serial.println("================================");

  if (xTaskCreate(MemoryTask, "MemoryTask", 4096, nullptr, 1, nullptr) != pdPASS) {
    Serial.println("ERROR: Memory task creation failed");
  }
}

void loop() {
  delay(1000);
}
