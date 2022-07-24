
#define FIRST_GROUP_RELAY_MODE 2      // 1 - Полное включение, 2 - пульсация
#define FIRST_GROUP_RELAY_MODE_PIN 2  // 1 - прямое реле, 2 - обратное реле

#define BTN_PIN_START 2  // Кнопка СТАРТ
#define BTN_PIN_RESET 3  // Кнопка СБРОС

#define FIRST_GROUP_RELAY_PIN_1 11
#define FIRST_GROUP_RELAY_PIN_2 12

#define BEEP_PIN 9  // Пин пьезо (при выборе 9 пина, 10 - недоступен из-за шим)

#define BUS_ID 2 // Адрес (Таймер) 
#define PIN_TIMER_ON 13 // Вывод сигнала включения

#include <Wire.h>
#include <GyverButton.h>
#include <DTM1650.h>
#include <TIMECLASS.h>
#include <ModbusRtu.h>

GButton button_start(BTN_PIN_START);
GButton button_reset(BTN_PIN_RESET);

DTM1650 display;

// Работа с шиной
Modbus bus(BUS_ID, 0, 0);
int8_t state = 0;
uint16_t temp[2] = { 0, 0 };

bool is_start_timer = false;
bool is_timer_pause = false;

bool last_dot = false;

unsigned long timer;
XLibs::TIMECLASS time;

unsigned long timer_relay_pulse;
bool is_relay_pulse = false;

void relay_on() {
#if FIRST_GROUP_RELAY_MODE == 1
#if FIRST_GROUP_RELAY_MODE_PIN == 1
  digitalWrite(FIRST_GROUP_RELAY_PIN_1, HIGH);
  digitalWrite(FIRST_GROUP_RELAY_PIN_2, HIGH);
#else
  digitalWrite(FIRST_GROUP_RELAY_PIN_1, LOW);
  digitalWrite(FIRST_GROUP_RELAY_PIN_2, LOW);
#endif
#endif

#if SECOND_GROUP_RELAY_MODE == 1
#if SECOND_GROUP_RELAY_MODE_PIN == 1
  digitalWrite(SECOND_GROUP_RELAY_PIN_1, HIGH);
  digitalWrite(SECOND_GROUP_RELAY_PIN_2, HIGH);
#else
  digitalWrite(SECOND_GROUP_RELAY_PIN_1, LOW);
  digitalWrite(SECOND_GROUP_RELAY_PIN_2, LOW);
#endif
#endif
}

void relay_off() {
#if FIRST_GROUP_RELAY_MODE_PIN == 1
  digitalWrite(FIRST_GROUP_RELAY_PIN_1, LOW);
  digitalWrite(FIRST_GROUP_RELAY_PIN_2, LOW);
#else
  digitalWrite(FIRST_GROUP_RELAY_PIN_1, HIGH);
  digitalWrite(FIRST_GROUP_RELAY_PIN_2, HIGH);
#endif
  
}

void relay_pulse(const bool pulse) {
  if (is_relay_pulse) {
    if (millis() - timer_relay_pulse > 1000) {
#if FIRST_GROUP_RELAY_MODE == 2
#if FIRST_GROUP_RELAY_MODE_PIN == 1
      digitalWrite(FIRST_GROUP_RELAY_PIN_1, LOW);
      digitalWrite(FIRST_GROUP_RELAY_PIN_2, LOW);
#else
      digitalWrite(FIRST_GROUP_RELAY_PIN_1, HIGH);
      digitalWrite(FIRST_GROUP_RELAY_PIN_2, HIGH);
#endif
#endif
      is_relay_pulse = false;
    }
  }

  if (pulse) {
#if FIRST_GROUP_RELAY_MODE == 2
#if FIRST_GROUP_RELAY_MODE_PIN == 1
    digitalWrite(FIRST_GROUP_RELAY_PIN_1, HIGH);
    digitalWrite(FIRST_GROUP_RELAY_PIN_2, HIGH);
#else
    digitalWrite(FIRST_GROUP_RELAY_PIN_1, LOW);
    digitalWrite(FIRST_GROUP_RELAY_PIN_2, LOW);
#endif
#endif
    is_relay_pulse = true;
    timer_relay_pulse = millis();
  }
}

void draw() {
  display.write_longtime(time.GetMin(), time.GetSec());
}

void setup() {
  button_start.setClickTimeout(50);
  button_reset.setClickTimeout(50);

  pinMode(FIRST_GROUP_RELAY_PIN_1, OUTPUT);
  pinMode(FIRST_GROUP_RELAY_PIN_2, OUTPUT);

  pinMode(PIN_TIMER_ON, OUTPUT);
  relay_off();

  Wire.begin();
  bus.begin(19200);

  display.init();
  display.set_brightness(DTM1650_BRIGHTNESS_MAX);
  draw();
}

void reset_timer() {
  time.Clear();
  temp[1] = time.ToSec();
  draw();
  relay_off();
  relay_pulse(!is_timer_pause);
  is_start_timer = false;
  is_timer_pause = false;
  last_dot = false;
  digitalWrite(PIN_TIMER_ON, LOW);
  tone(BEEP_PIN, 5000, 1000);
}

void button_tick() {
  button_start.tick();
  button_reset.tick();

  if (is_start_timer) {
    if (button_start.isSingle() && is_timer_pause) {
      is_timer_pause = false;
      relay_on();
      relay_pulse(true);
      tone(BEEP_PIN, 3000, 100);
    }

    if (button_reset.isSingle() && !is_timer_pause) {
      is_timer_pause = true;
      relay_off();
      relay_pulse(true);
      tone(BEEP_PIN, 2000, 100);
    }

    if (button_reset.isHolded()) {
      reset_timer();
    }

    return;
  }

  if (button_start.isSingle() || button_start.isHold()) {
    is_start_timer = true;
    is_timer_pause = false;
    time.Clear();
    temp[1] = time.ToSec();
    relay_on();
    relay_pulse(true);
    timer = millis();
    draw();
    digitalWrite(PIN_TIMER_ON, HIGH);
    tone(BEEP_PIN, 5000, 100);
  }
}

void timer_tick() {
  if (time.GetSec() < 999) {
    if (millis() - timer > 1000) {
      timer = millis();
      time.TickSec();
      last_dot = !last_dot;
      draw();
      temp[1] = time.ToSec();
    }
  } else {
    reset_timer();
  }
}

void loop() {

	state = bus.poll(temp, 2);
	button_tick();

	if (is_start_timer && !is_timer_pause) {
		timer_tick();
	}

	relay_pulse(false);
	display.set_dot(3, last_dot);
}
