#include <Arduino.h>

// LED de teste: acende quando qualquer entrada estiver pressionada
constexpr uint8_t LED_TESTE = 23;

constexpr unsigned long DEBOUNCE_MS = 30;
constexpr unsigned long STATUS_INTERVAL_MS = 2000;

struct Input {
  uint8_t pin;
  const char *label;
  bool stablePressed;
  bool lastReading;
  unsigned long lastChangeMs;
};

Input inputs[] = {
    {13, "Alavanca UP", false, false, 0},
    {12, "Alavanca DOWN", false, false, 0},
    {14, "Alavanca LEFT", false, false, 0},
    {27, "Alavanca RIGHT", false, false, 0},
    {25, "Botao Square", false, false, 0},
    {26, "Botao Circle", false, false, 0},
    {32, "Botao Triangle", false, false, 0},
    {33, "Botao X", false, false, 0},
    {18, "Botao R1", false, false, 0},
    {19, "Botao L2", false, false, 0},
    {21, "Botao Start", false, false, 0},
    {22, "Botao Select", false, false, 0},
};

constexpr size_t INPUT_COUNT = sizeof(inputs) / sizeof(inputs[0]);

unsigned long lastStatusMs = 0;

bool isPressed(uint8_t pin) { return digitalRead(pin) == LOW; }

void printBanner() {
  Serial.println();
  Serial.println("========================================");
  Serial.println("  Controle Arcade - Teste de Entradas");
  Serial.println("========================================");
  Serial.println("Pull-up interno: repouso = solto, LOW = pressionado");
  Serial.printf("LED de teste no GPIO %d\n", LED_TESTE);
  Serial.println();
  Serial.println("Entradas monitoradas:");
  for (size_t i = 0; i < INPUT_COUNT; i++) {
    Serial.printf("  GPIO %-2d -> %s\n", inputs[i].pin, inputs[i].label);
  }
  Serial.println();
  Serial.println("Pressione botões ou mova a alavanca...");
  Serial.println("----------------------------------------");
}

void printActiveInputs() {
  bool anyActive = false;

  Serial.print("[STATUS] Ativos: ");

  for (size_t i = 0; i < INPUT_COUNT; i++) {
    if (inputs[i].stablePressed) {
      if (anyActive) {
        Serial.print(", ");
      }
      Serial.print(inputs[i].label);
      anyActive = true;
    }
  }

  if (!anyActive) {
    Serial.print("nenhum");
  }

  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(LED_TESTE, OUTPUT);
  digitalWrite(LED_TESTE, LOW);

  for (size_t i = 0; i < INPUT_COUNT; i++) {
    pinMode(inputs[i].pin, INPUT_PULLUP);
    inputs[i].lastReading = isPressed(inputs[i].pin);
    inputs[i].stablePressed = inputs[i].lastReading;
  }

  printBanner();
  printActiveInputs();
  lastStatusMs = millis();
}

void loop() {
  bool anyPressed = false;
  const unsigned long now = millis();

  for (size_t i = 0; i < INPUT_COUNT; i++) {
    Input &input = inputs[i];
    const bool reading = isPressed(input.pin);

    if (reading != input.lastReading) {
      input.lastChangeMs = now;
      input.lastReading = reading;
    }

    if ((now - input.lastChangeMs) >= DEBOUNCE_MS && reading != input.stablePressed) {
      input.stablePressed = reading;

      Serial.printf("[%s] %s (GPIO %d)\n",
                    input.stablePressed ? "PRESSIONADO" : "SOLTO",
                    input.label,
                    input.pin);
    }

    if (input.stablePressed) {
      anyPressed = true;
    }
  }

  digitalWrite(LED_TESTE, anyPressed ? HIGH : LOW);

  if ((now - lastStatusMs) >= STATUS_INTERVAL_MS) {
    printActiveInputs();
    lastStatusMs = now;
  }
}
