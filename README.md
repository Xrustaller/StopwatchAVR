# BackTimer

Проект таймера

При прошиве можно выбрать какой таймер зашить:

TIMER_MODE (1 - таймер, 2 - обратный таймер)

Режимы работы реле:

FIRST_GROUP_RELAY_MODE или SECOND_GROUP_RELAY_MODE (1 - Полное включение, 2 - пульсация)
FIRST_GROUP_RELAY_MODE_PIN или SECOND_GROUP_RELAY_MODE_PIN (1 - прямое реле, 2 - обратное реле)

Управление 4мя кнопками
- Старт
- Сброс
- Плюс
- Минус

Кнопки управления:
- При однократном нажатии на плюс/минус время изменяется на 1 секунду
- При двойном нажатии на плюс/минус время изменяется на 5 мин
- При тройном нажатии на плюс/минус время изменяется на 10 мин
- При удержании плюса/минуса время изменяется по секунде каждые 10мс

Сброс до стандартной одной минуты осуществляется при удержании сброса

Что бы запустить таймер, нужно однократно нажать кнопку старт.

После запуска что бы сбросить таймер нужно зажать кнопку сброс и тогда таймер остановиться и выведет предыдущее заданное время.