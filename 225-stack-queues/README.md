# StackQueues

<p style="text-align: left">
  <a href="#русский">Русский</a> ・ <a href="#english">English</a>
</p>

---

## Русский

### Идея

Задача — реализовать стек (LIFO), используя только очереди (FIFO). Разрешены только стандартные операции очереди: push в конец, front/pop с начала, empty.

Ключевое требование — не эмулировать стек через ручные срезы массива. Только `push`, `front`, `pop`, `empty` у `std::queue`.

---

### Подход

Используются две очереди одинакового типа:

* `enQueue` — временный буфер для текущей вставки
* `deQueue` — хранит элементы в порядке, готовом для чтения верхушки стека

Инвариант:

```
deQueue.front() == верхушка стека
```

`enQueue` пустая всегда, кроме момента внутри вызова `push`.

---

### Структура класса

```cpp
class StackQueues {
    std::queue<int> enQueue;
    std::queue<int> deQueue;
};
```

* `enQueue` — буфер вставки
* `deQueue` — рабочая очередь, из которой читается верхушка

---

### Инициализация

```cpp
StackQueues() = default;
```

Обе очереди стартуют пустыми, отдельная инициализация не нужна.

---

### push

```cpp
void push(int x) {
    enQueue.push(x);

    while (!deQueue.empty()) {
        enQueue.push(deQueue.front());
        deQueue.pop();
    }

    std::swap(deQueue, enQueue);
}
```

* новый элемент кладётся в `enQueue` первым
* следом переливается всё содержимое `deQueue`, сохраняя порядок
* очереди меняются местами

После `swap` новый элемент оказывается в начале `deQueue` — то есть на верхушке стека.

---

### pop

```cpp
int pop() {
    int head = deQueue.front();
    deQueue.pop();

    return head;
}
```

Верхушка всегда лежит в начале `deQueue`, удаление — O(1).

---

### top / empty

```cpp
int top() const {
    return deQueue.front();
}

bool empty() const {
    return deQueue.empty();
}
```

Прямое чтение начала `deQueue`.

---

### Почему это работает

После каждого `push` порядок в `deQueue` перестраивается так, что последний добавленный элемент всегда впереди:

* новый элемент кладётся в `enQueue` до переливания
* при переливании старые элементы идут следом, сохраняя взаимный порядок
* `swap` делает эту последовательность рабочей очередью

`pop`, `top`, `empty` ничего не пересчитывают — читают готовый порядок.

---

### Операции

| Операция   | Сложность | Описание                                 |
| ---------- | --------- | ----------------------------------------- |
| `push(x)`  | O(n)      | вставка с перестройкой порядка            |
| `pop()`    | O(1)      | удаление верхнего элемента                |
| `top()`    | O(1)      | получить верхний элемент                  |
| `empty()`  | O(1)      | проверить, пуст ли стек                   |

n — число элементов в стеке на момент вызова push.

---

### Детали реализации

* `std::swap` для `std::queue` — O(1), меняет внутренние буферы, а не копирует элементы
* обе очереди private, доступ только через методы класса
* `pop`/`top` не бросают исключений при пустом стеке — проверка на стороне вызывающего кода

---

### Почему не через один вектор с ручным сдвигом

```cpp
std::vector<int> queue;
queue.erase(queue.begin()); // O(n) на каждый вызов
```

Такой вариант тоже даёт O(n) на push, но:

* `erase(begin())` каждый раз сдвигает весь массив в памяти
* операция не входит в разрешённый список стандартных операций очереди
* `std::queue` не даёт случайного доступа к середине — ошибку среза невозможно допустить

---

### Ограничения

* нет проверки на пустой стек в `pop()`/`top()`
* `push` — O(n), а не амортизированная O(1); вариант с O(1) push и O(n) pop/top возможен через смену роли очередей

---

## English

### Idea

The task is to implement a stack (LIFO) using only queues (FIFO). Only standard queue operations are allowed: push to back, front/pop from front, empty.

The key requirement is to avoid emulating a stack through manual array slicing. Only `push`, `front`, `pop`, `empty` on `std::queue`.

---

### Approach

Two queues of the same type are used:

* `enQueue` — temporary buffer for the current insertion
* `deQueue` — holds elements in an order ready for reading the top of the stack

Invariant:

```
deQueue.front() == top of the stack
```

`enQueue` is empty at all times except during a `push` call.

---

### Class structure

```cpp
class StackQueues {
    std::queue<int> enQueue;
    std::queue<int> deQueue;
};
```

* `enQueue` — insertion buffer
* `deQueue` — working queue, top is read from here

---

### Initialization

```cpp
StackQueues() = default;
```

Both queues start empty, no separate initialization needed.

---

### push

```cpp
void push(int x) {
    enQueue.push(x);

    while (!deQueue.empty()) {
        enQueue.push(deQueue.front());
        deQueue.pop();
    }

    std::swap(deQueue, enQueue);
}
```

* the new element is pushed into `enQueue` first
* the entire content of `deQueue` is then moved over, preserving order
* the queues are swapped

After `swap`, the new element ends up at the front of `deQueue` — the top of the stack.

---

### pop

```cpp
int pop() {
    int head = deQueue.front();
    deQueue.pop();

    return head;
}
```

The top is always at the front of `deQueue`, removal is O(1).

---

### top / empty

```cpp
int top() const {
    return deQueue.front();
}

bool empty() const {
    return deQueue.empty();
}
```

Direct read of the front of `deQueue`.

---

### Why it works

After every `push`, the order in `deQueue` is rebuilt so the most recently added element is always at the front:

* the new element goes into `enQueue` before the transfer
* during the transfer, older elements follow, keeping their relative order
* `swap` turns this sequence into the working queue

`pop`, `top`, `empty` do no recomputation — they read an already-built order.

---

### Operations

| Operation  | Complexity | Description                          |
| ---------- | ---------- | ------------------------------------- |
| `push(x)`  | O(n)       | insertion with order rebuild          |
| `pop()`    | O(1)       | remove top element                    |
| `top()`    | O(1)       | get top element                       |
| `empty()`  | O(1)       | check if the stack is empty           |

n — number of elements in the stack at the time push is called.

---

### Implementation details

* `std::swap` on `std::queue` is O(1), it swaps internal buffers rather than copying elements
* both queues are private, accessible only through class methods
* `pop`/`top` do not throw on an empty stack — the check is the caller's responsibility

---

### Why not a single vector with manual shifting

```cpp
std::vector<int> queue;
queue.erase(queue.begin()); // O(n) per call
```

This also gives O(n) push, but:

* `erase(begin())` shifts the entire array in memory every time
* the operation is outside the allowed set of standard queue operations
* `std::queue` gives no random access to the middle — a slicing mistake is impossible by construction

---

### Limitations

* no empty stack check in `pop()`/`top()`
* `push` is O(n), not amortized O(1); an O(1) push with O(n) pop/top variant is possible by swapping which queue holds which role

---

<br>

> Верхушка не ищется — она всегда впереди.
>
> The top is not searched — it is always at the front.