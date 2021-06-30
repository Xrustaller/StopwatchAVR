

#define TIMER_MODE 2 // 1 - таймер, 2 - обратный таймер

#define FIRST_GROUP_RELAY_MODE 2 // 1 - Полное включение, 2 - пульсация
#define FIRST_GROUP_RELAY_MODE_PIN 2 // 1 - прямое реле, 2 - обратное реле

#define SECOND_GROUP_RELAY_MODE 2 // 1 - Полное включение, 2 - пульсация
#define SECOND_GROUP_RELAY_MODE_PIN 2 // 1 - прямое реле, 2 - обратное реле

#define BTN_PIN_START 2 // Кнопка СТАРТ
#define BTN_PIN_RESET 3 // Кнопка СБРОС
#define BTN_PIN_MINUS 4 // Кнопка -
#define BTN_PIN_PLUS 5 // Кнопка +


#define FIRST_GROUP_RELAY_PIN_1 11
#define FIRST_GROUP_RELAY_PIN_2 12

#define SECOND_GROUP_RELAY_PIN_1 6
#define SECOND_GROUP_RELAY_PIN_2 7

#define BEEP_PIN 9 // Пин пьезо (при выборе 9 пина, 10 - недоступен из-за шим)

#include <Wire.h>
#include <GyverButton.h>
#include <DTM1650.h>

GButton button_start(BTN_PIN_START);
GButton button_reset(BTN_PIN_RESET);
GButton button_minus(BTN_PIN_MINUS);
GButton button_plus(BTN_PIN_PLUS);

DTM1650 display;

bool is_start_timer = false;
bool is_timer_pause = false;
uint8_t timer_set_sec = 0;   // переменные для отсчета минут и секунд
uint8_t timer_set_min = 1;

unsigned long timer;
uint8_t timer_sec;   // переменные для отсчета минут и секунд
uint8_t timer_min;

unsigned long timer_relay_pulse;
bool is_relay_pulse = false;

void relay_on()
{
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

void relay_off()
{
#if FIRST_GROUP_RELAY_MODE_PIN == 1
    digitalWrite(FIRST_GROUP_RELAY_PIN_1, LOW);
    digitalWrite(FIRST_GROUP_RELAY_PIN_2, LOW);
#else
    digitalWrite(FIRST_GROUP_RELAY_PIN_1, HIGH);
    digitalWrite(FIRST_GROUP_RELAY_PIN_2, HIGH);
#endif

#if SECOND_GROUP_RELAY_MODE_PIN == 1
    digitalWrite(SECOND_GROUP_RELAY_PIN_1, LOW);
    digitalWrite(SECOND_GROUP_RELAY_PIN_2, LOW);
#else
    digitalWrite(SECOND_GROUP_RELAY_PIN_1, HIGH);
    digitalWrite(SECOND_GROUP_RELAY_PIN_2, HIGH);
#endif
}

void relay_pulse(const bool pulse)
{
    if (is_relay_pulse)
    {
        if (millis() - timer_relay_pulse > 1000)
        {
#if FIRST_GROUP_RELAY_MODE == 2
#if FIRST_GROUP_RELAY_MODE_PIN == 1
            digitalWrite(FIRST_GROUP_RELAY_PIN_1, LOW);
            digitalWrite(FIRST_GROUP_RELAY_PIN_2, LOW);
#else
            digitalWrite(FIRST_GROUP_RELAY_PIN_1, HIGH);
            digitalWrite(FIRST_GROUP_RELAY_PIN_2, HIGH);
#endif
#endif

#if SECOND_GROUP_RELAY_MODE == 2
#if SECOND_GROUP_RELAY_MODE_PIN == 1
            digitalWrite(SECOND_GROUP_RELAY_PIN_1, LOW);
            digitalWrite(SECOND_GROUP_RELAY_PIN_2, LOW);
#else
            digitalWrite(SECOND_GROUP_RELAY_PIN_1, HIGH);
            digitalWrite(SECOND_GROUP_RELAY_PIN_2, HIGH);
#endif
#endif
            is_relay_pulse = false;
        }
    }
	
	if (pulse)
	{
#if FIRST_GROUP_RELAY_MODE == 2
#if FIRST_GROUP_RELAY_MODE_PIN == 1
        digitalWrite(FIRST_GROUP_RELAY_PIN_1, HIGH);
        digitalWrite(FIRST_GROUP_RELAY_PIN_2, HIGH);
#else
        digitalWrite(FIRST_GROUP_RELAY_PIN_1, LOW);
        digitalWrite(FIRST_GROUP_RELAY_PIN_2, LOW);
#endif
#endif

#if SECOND_GROUP_RELAY_MODE == 2
#if SECOND_GROUP_RELAY_MODE_PIN == 1
        digitalWrite(SECOND_GROUP_RELAY_PIN_1, HIGH);
        digitalWrite(SECOND_GROUP_RELAY_PIN_2, HIGH);
#else
        digitalWrite(SECOND_GROUP_RELAY_PIN_1, LOW);
        digitalWrite(SECOND_GROUP_RELAY_PIN_2, LOW);
#endif
#endif
		
        is_relay_pulse = true;
        timer_relay_pulse = millis();
	}
	
}

void setup()
{
    button_start.setClickTimeout(50);
    button_reset.setClickTimeout(50);
  
    button_minus.setClickTimeout(200);
    button_plus.setClickTimeout(200);

    button_minus.setStepTimeout(10);
    button_plus.setStepTimeout(10);
	
    pinMode(FIRST_GROUP_RELAY_PIN_1, OUTPUT);
    pinMode(FIRST_GROUP_RELAY_PIN_2, OUTPUT);
    pinMode(SECOND_GROUP_RELAY_PIN_1, OUTPUT);
    pinMode(SECOND_GROUP_RELAY_PIN_2, OUTPUT);
    relay_off();

    Wire.begin();
    
    display.init();
    display.set_brightness(DTM1650_BRIGHTNESS_MAX);
    display.write_time(timer_set_min, timer_set_sec);
}

void timer_set_plus()
{
    if (timer_set_sec >= 59 && timer_set_min >= 99)
    {
        tone(BEEP_PIN, 2000, 100);
    }
    else
    {
        if (timer_set_sec > 58)
        {
            timer_set_min++;
            timer_set_sec = 0;
        }
        else
        {
            timer_set_sec++;
        }
    }
    display.write_time(timer_set_min, timer_set_sec);
}

void timer_set_plus_5_min()
{
    if (timer_set_min >= 95)
    {
        tone(BEEP_PIN, 2000, 100);
    }
    else
    {
        timer_set_min += 5;
    }
    display.write_time(timer_set_min, timer_set_sec);
}

void timer_set_plus_10_min()
{
    if (timer_set_min >= 90)
    {
        tone(BEEP_PIN, 2000, 100);
    }
    else
    {
        timer_set_min += 10;
    }
    display.write_time(timer_set_min, timer_set_sec);
}

void timer_set_minus()
{
    if (timer_set_sec < 2 && timer_set_min < 1)
    {
        tone(BEEP_PIN, 200, 100);
    }
    else
    {
        if (timer_set_sec == 0)
        {
            timer_set_min--;
            timer_set_sec = 59;
        }
        else
        {
            timer_set_sec--;
        }
    }
    display.write_time(timer_set_min, timer_set_sec);
}

void timer_set_minus_5_min()
{
    if (timer_set_min < 5)
    {
        tone(BEEP_PIN, 200, 100);
    }
    else
    {
      timer_set_min -= 5;
    }
    display.write_time(timer_set_min, timer_set_sec);
}

void timer_set_minus_10_min()
{
    if (timer_set_min < 10)
    {
        tone(BEEP_PIN, 200, 100);
    }
    else
    {
        timer_set_min -= 10;
    }
    display.write_time(timer_set_min, timer_set_sec);
}

void reset_timer()
{
    display.write_time(timer_set_min, timer_set_sec);
    is_start_timer = false;
    is_timer_pause = false;
    relay_off();
    relay_pulse(true);
    tone(BEEP_PIN, 5000, 1000);
}

void button_tick()
{
    button_start.tick();
    button_reset.tick();
    button_minus.tick();
    button_plus.tick();

    if (is_start_timer)
    {
        if (button_start.isSingle() && is_timer_pause)
        {
            is_timer_pause = false;
            relay_on();
            relay_pulse(true);
            tone(BEEP_PIN, 3000, 100);
        }

        if (button_reset.isSingle() && !is_timer_pause)
        {
            is_timer_pause = true;
            relay_off();
            relay_pulse(true);
            tone(BEEP_PIN, 2000, 100);
        }
    	
        if (button_reset.isHolded())
        {
            reset_timer();
        }

        return;
    }
	
    if (button_start.isSingle() || button_start.isHold())
    {
        is_start_timer = true;
        is_timer_pause = false;
#if TIMER_MODE == 1
        timer_sec = 0;
        timer_min = 0;
#else
        timer_sec = timer_set_sec;
        timer_min = timer_set_min;
#endif
        relay_on();
        relay_pulse(true);
        timer = millis();
        display.write_time(timer_min, timer_sec);
        tone(BEEP_PIN, 5000, 100);
    }
    
    if (button_reset.isHolded())
    {
        timer_set_min = 1;
        timer_set_sec = 0;
        display.write_time(timer_set_min, timer_set_sec);
        tone(BEEP_PIN, 200, 100);
    }

    if (button_minus.isSingle())
    {
        timer_set_minus();
    }
    if (button_minus.isDouble())
    {
        timer_set_minus_5_min();
    }
    if (button_minus.isTriple())
    {
        timer_set_minus_10_min();
    }
    if (button_minus.isHolded())
    {
        timer_set_minus();
    }

    if (button_plus.isSingle())
    {
        timer_set_plus();
    }
    if (button_plus.isHolded())
    {
        timer_set_plus();
    }
    if (button_plus.isDouble())
    {
        timer_set_plus_5_min();
    }
    if (button_plus.isTriple())
    {
        timer_set_plus_10_min();
    }

    if (button_minus.isStep())
    {
        timer_set_minus();
    }
    if (button_plus.isStep())
    {
        timer_set_plus();
    }
}

void timer_tick()
{
#if TIMER_MODE == 1 // секундомер
    if (timer_sec >= timer_set_sec && timer_min >= timer_set_min)
    {
        reset_timer();
    }
    else
    {
        if (millis() - timer > 1000)
        {
            timer = millis();
            if (timer_sec > 58)
            {
                timer_min++;
                timer_sec = 0;
            }
            else
            {
                timer_sec++;
            }
            display.write_time(timer_min, timer_sec);
        }
    }
#else
    if (timer_sec < 1 && timer_min < 1)
    {
        reset_timer();
    }
    else
    {
        if (millis() - timer > 1000)
        {
            timer = millis();
            if (timer_sec == 0)
            {
                timer_min--;
                timer_sec = 59;
            }
            else
            {
                timer_sec--;
            }
            display.write_time(timer_min, timer_sec);
        }
    }
#endif
    
}

void loop()
{
    button_tick();
	
	if (is_start_timer && !is_timer_pause)
    {
		timer_tick();
    }

    relay_pulse(false);
}
