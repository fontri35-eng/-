# ФизКат — домашнее задание № 2

## Структура проекта

```text
FizKat/
├── main.cpp             # Исходный код C++17
├── README.md            # Описание проекта и инструкция
├── run.sh               # Сборка и запуск на macOS/Linux
├── example_input.txt    # Ввод из примера задания
├── example_output.txt   # Ожидаемый вывод для примера
└── .gitignore           # Файлы, которые не нужно отправлять в Git
```


**Запуск** :

```bash
cd ~/Desktop/FizKat
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o fizkat
./fizkat
```

**Проверка** :

```bash
cd ~/Desktop/FizKat
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o fizkat
./fizkat < example_input.txt
```

Результат должен совпасть с файлом `example_output.txt`.

После запуска команды вводятся по одной строке. `id` и `name` — по одному слову без пробелов.

## Команды

- `add scooter id name energy score` — электросамокат.
- `add bike id name score` — велосипед.
- `add ebike id name energy score` — электровелосипед.
- `add helmet id name` — шлем.
- `add dock id name energy` — док-станция.
- `add skate id name score` — скейтборд (дополнительный тип).
- `list` — все позиции по возрастанию id.
- `novice` — техника, подходящая новичкам.
- `energy` — суточный расход с одним знаком после запятой.
- `count` — количество позиций на балансе.
- `count_novice` — количество позиций, подходящих новичкам (дополнительный отчёт).
- `focus id` — выбрать позицию для витрины.
- `show_focus` — показать выбранную позицию; если её нет, вывести `expired`.
- `remove id` — удалить позицию.
- `quit` — выход.

## Схема наследования

```text
InventoryItem (абстрактный)
└── Item
    ├── Transport
    │   ├── Scooter       + EnergyConsumer
    │   ├── Bike
    │   ├── EBike         + EnergyConsumer
    │   └── Skate
    ├── Helmet
    └── Dock              + EnergyConsumer

EnergyConsumer (абстрактный)
```

`Scooter`, `EBike` и `Dock` используют множественное наследование. Ромбовидного наследования нет. Методы `inventoryId`, `name`, `describe` чисто виртуальные в `InventoryItem`. `dailyEnergy` — чисто виртуальный метод `EnergyConsumer`. Отчёты используют виртуальные `energy`, `novice`, `describe` через указатели на `InventoryItem`.

## Операторы и шаблоны

- `operator<<` печатает инвентарный номер и имя через виртуальный `describe`.
- `operator<` сравнивает позиции по инвентарному номеру и используется для сортировки в `list`.
- `operator==` сравнивает позиции по номеру и используется для проверки дубликатов при добавлении.
- `Registry<T>` — шаблонный класс с `add`, `size`, `operator[]`. Используется для общих описаний моделей (`shared_ptr<ModelSpec>`) и для временного списка позиций (`const InventoryItem*`).
- `totalEnergy<T>` — шаблонная функция для суммирования потребления через виртуальный `energy`.

## Указатели

- `unique_ptr<InventoryItem>` хранится в `Fleet::items`: парк единолично владеет объектами инвентаря.
- `shared_ptr<ModelSpec>` — общее описание типа транспорта и границы простоты для новичка. Например, два самоката получают ссылку на один объект `ModelSpec`. Порог равен 6.
- `weak_ptr<FocusData>` — выбранная позиция витрины. У каждого `Item` есть принадлежащий ему `shared_ptr<FocusData>` — небольшая карточка с id и именем. Фокус не продлевает её жизнь: при `remove` уничтожается объект инвентаря и его карточка, так что `show_focus` выводит `expired`. Используется карточка, потому что нельзя напрямую создать `weak_ptr` из `unique_ptr`.

## Исключения и проверка

Базовый тип — `FleetError`, наследует `std::runtime_error`. От него наследуются:

- `InspectionError` — техосмотр не пройден: в имени транспортного средства содержится `broken` (регистр не важен).
- `CommandError` — неверная команда, неверное количество аргументов, отрицательная энергия, `score` не в диапазоне 1..10, повторный id или неизвестный id.

Ошибки бросаются до добавления в парк. Сообщения вида `error: ...` печатаются только в `catch` внутри `main`.

## Дополнения

Добавлен новый транспорт `Skate` (скейтборд), команда `add skate`, а также отчёты `count` и `count_novice`.

## Использование ИИ

Для разработки README использовался ChatGPT. 