# ИДЗ-4. Вариант 7
Исполнитель: Миронов Павел Андреевич. Группа: БПИ-212
## Условие задачи
Задача о курильщиках. Есть три процесса–курильщика и один
процесс–посредник. Курильщик непрерывно скручивает сигареты
и курит их. Чтобы скрутить сигарету, нужны табак, бумага и спички. У одного процесса–курильщика есть табак, у второго — бумага, а у третьего — спички. Посредник кладет на стол по два разных случайных компонента. Тот процесс–курильщик, у которого
есть третий компонент, забирает компоненты со стола, скручивает
сигарету и курит. Посредник дожидается, пока курильщик закончит, затем процесс повторяется. Создать приложение, моделирующее поведение курильщиков и посредника. Каждый
курильщик — клиент. Посредник — сервер.
## 4-5 баллов
### Сценарий

#### Сервер:
1. Создание сокета и привязка к адресу сервера.
2. Ожидание подключения трех клиентов.
3. При подключении каждого клиента:
   - Получение сообщения от клиента с его идентификацией.
   - Отправка приветственного сообщения клиенту.
4. Бесконечный цикл:
   - Генерация двух случайных компонентов и положение их на стол.
   - Отправка сообщения с информацией о компонентах каждому клиенту.
   - Получение ответа от последнего клиента, который собрал сигарету.
   - Пауза в работе сервера на 5 секунд.
   - Возврат к началу цикла для генерации новых компонентов.

#### Клиент:
1. Создание сокета.
2. Привязка сокета к клиентскому адресу (любой доступный порт на клиентской машине).
3. Отправка сообщения на сервер с информацией о клиенте и его имеющемся компоненте.
4. Получение приветственного сообщения от сервера.
5. Бесконечный цикл:
   - Получение сообщения от сервера с информацией о двух компонентах на столе.
   - Если имеющийся компонент клиента и два компонента на столе позволяют собрать сигарету, клиент курит.
   - Отправка сообщения на сервер о завершении курения или ожидании новых компонентов.
   - Возврат к началу цикла для получения новых компонентов от сервера и проверки возможности курения.

В процессе обмена сервер и клиенты выводят сообщения о получении и отправке данных, чтобы отразить текущее состояние и взаимодействие.

Приложение может работать как на одном компьютере, так и на нескольких компьютерах при условии правильного указания ip-адресов, их доступности в сети и открытых портах.

Для запуска нужно скомпилировать src/4-5/mediator.c и src/4-5/smoker.c:
```
cd src/4-5
gcc -o smoker.o smoker.c
gcc -o mediator.o mediator.c
```

Запуск программы (локально на одном компьютере, в разных сессиях терминала)
```
./mediator.o 127.0.0.1 8080
./smoker.o 127.0.0.1 8080 1
./smoker.o 127.0.0.1 8080 2
./smoker.o 127.0.0.1 8080 3
```

### Пример результата работы
Сервер
```
Сервер запущен на адресе 127.0.0.1:8080
Ожидание клиента 1...
Получено: Запущен клиент с табаком!
Отправлено: Добро пожаловать, курильщик 1!
Ожидание клиента 2...
Получено: Запущен клиент с бумагой!
Отправлено: Добро пожаловать, курильщик 2!
Ожидание клиента 3...
Получено: Запущен клиент с спичками!
Отправлено: Добро пожаловать, курильщик 3!
Все клиенты подключены.
Посредник положил на стол: бумагу и табак
Один из курильщиков собрал сигарету.
Сигарета докурена. Посредник снова генерирует компоненты.
Посредник положил на стол: бумагу и спички
Один из курильщиков собрал сигарету.
Сигарета докурена. Посредник снова генерирует компоненты.
...
```
Клиент 1
```
Клиент запущен
У клиента есть табак.
Клиент берет бумагу и спички со стола.
Клиент курит...
Клиент закончил курить.
...
```
Клиент 2
```
Клиент запущен
...
```
Клиент 3
```
Клиент запущен
У клиента есть спички.
Клиент берет бумагу и табак со стола.
Клиент курит...
Клиент закончил курить.
...
```

#### P.S.
Для удобства тестирования написан docker-compose файл, который запускает контейнер с рабочим gcc
```
docker-compose up -d
```

Для подключения к контейнеру используйте команду
```
docker compose exec -it server bash
```