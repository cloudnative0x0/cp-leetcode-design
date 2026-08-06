# QueueStacks

<p style="text-align: left">
  <a href="#русский">Русский</a> ・ <a href="#english">English</a>
</p>

---

## Русский

### Идея

Задача — реализовать очередь (FIFO), используя только стеки (LIFO). Разрешены только стандартные операции стека: push, top/pop, empty.

Ключевое требование — не эмулировать очередь через ручные сдвиги массива. Только `push`, `top`, `pop`, `empty` у `std::stack`.

---

### Подход

Используются два стека одинакового типа:

* `enStack` — буфер, в который попадают все вставляемые элементы
* `deStack` — рабочий стек, из которого читается голова очереди

Инвариант:

```
deStack.top() == голова очереди
```

Перекладка из `enStack` в `deStack` происходит только тогда, когда `deStack` пуст.

---

### Структура класса

```cpp
class QueueStacks {
    std::stack<int> enStack;
    std::stack<int> deStack;
};
```

* `enStack` — стек вставки
* `deStack` — стек чтения, из него берётся голова очереди

---

### Инициализация

```cpp
QueueStacks() = default;
```

Оба стека стартуют пустыми, отдельная инициализация не нужна.

---

### push

```cpp
void push(int x) {
    enStack.push(x);
}
```

Вставка всегда идёт в `enStack`, без затрагивания `deStack`. Порядок в `deStack` не пересобирается на каждый вызов — это отличает подход от push-тяжёлой реализации через очереди.

---

### moveElement

```cpp
void moveElement() {
    if (deStack.empty()) {
        while (!enStack.empty()) {
            int topIdx = enStack.top();

            deStack.push(topIdx);
            enStack.pop();
        }
    }
}
```

Перекладка запускается только при пустом `deStack`. При перекладке порядок элементов переворачивается: самый старый элемент `enStack` оказывается внизу `enStack`, но наверху `deStack` после переноса.

---

### pop / peek

```cpp
int pop() {
    moveElement();

    int head = deStack.top();
    deStack.pop();

    return head;
}

int peek() {
    moveElement();

    return deStack.top();
}
```

Оба метода сначала вызывают `moveElement`, чтобы гарантировать актуальность головы очереди в `deStack`, затем читают `top()`.

---

### empty

```cpp
bool empty() {
    return enStack.empty() && deStack.empty();
}
```

Очередь пуста только тогда, когда пусты оба стека — элементы могут находиться в любом из них в зависимости от истории вызовов.

---

### Почему это работает

`enStack` копит новые вставки в порядке поступления. Как только `deStack` опустошается, весь `enStack` переливается в него целиком — при этом порядок элементов инвертируется, и самый ранний из непрочитанных элементов оказывается на вершине `deStack`.

Пока `deStack` не пуст, `enStack` продолжает копить новые элементы без переливания — они попадут в очередь только после того, как текущий `deStack` будет вычерпан до дна.

`push` не трогает `deStack`, `pop`/`peek` не трогают `enStack` напрямую — они вызывают `moveElement`, которая решает, нужна ли перекладка.

---

### Операции

| Операция   | Сложность            | Описание                                     |
| ---------- | --------------------- | --------------------------------------------- |
| `push(x)`  | O(1)                   | вставка в `enStack`                           |
| `pop()`    | амортизированно O(1)   | снятие головы очереди, при необходимости — перекладка |
| `peek()`   | амортизированно O(1)   | чтение головы очереди, при необходимости — перекладка |
| `empty()`  | O(1)                   | проверка обоих стеков                         |

Единичный вызов `pop`/`peek` может стоить O(n), если он инициирует перекладку, но каждый элемент переносится из `enStack` в `deStack` не более одного раза за весь жизненный цикл — отсюда амортизированная O(1).

---

### Детали реализации

* перекладка условная — по `if (deStack.empty())`, а не безусловная на каждый вызов
* оба стека private, доступ только через методы класса
* `pop`/`peek` не бросают исключений при пустой очереди — проверка на стороне вызывающего кода

---

### Почему не через один вектор с ручным сдвигом

```cpp
std::vector<int> stack;
stack.erase(stack.begin()); // O(n) на каждый вызов pop
```

Такой вариант тоже может сработать, но:

* `erase(begin())` сдвигает весь массив в памяти на каждом `pop`, без амортизации
* операция не входит в разрешённый список стандартных операций стека
* `std::stack` не даёт случайного доступа к середине — сдвиг вручную сделать невозможно по конструкции

---

### Ограничения

* нет проверки на пустую очередь в `pop()`/`peek()`
* амортизированная сложность, а не строгая O(1) на каждый вызов — при накопленной перекладке отдельный `pop` может стоить O(n)

---

## English

### Idea

The task is to implement a queue (FIFO) using only stacks (LIFO). Only standard stack operations are allowed: push, top/pop, empty.

The key requirement is to avoid emulating a queue through manual array shifting. Only `push`, `top`, `pop`, `empty` on `std::stack`.

---

### Approach

Two stacks of the same type are used:

* `enStack` — buffer that receives every inserted element
* `deStack` — working stack, the head of the queue is read from here

Invariant:

```
deStack.top() == head of the queue
```

Elements are moved from `enStack` to `deStack` only when `deStack` is empty.

---

### Class structure

```cpp
class QueueStacks {
    std::stack<int> enStack;
    std::stack<int> deStack;
};
```

* `enStack` — insertion stack
* `deStack` — reading stack, the head of the queue is taken from here

---

### Initialization

```cpp
QueueStacks() = default;
```

Both stacks start empty, no separate initialization needed.

---

### push

```cpp
void push(int x) {
    enStack.push(x);
}
```

Insertion always goes into `enStack`, without touching `deStack`. The order in `deStack` is not rebuilt on every call — this is what sets the approach apart from a push-heavy implementation using queues.

---

### moveElement

```cpp
void moveElement() {
    if (deStack.empty()) {
        while (!enStack.empty()) {
            int topIdx = enStack.top();

            deStack.push(topIdx);
            enStack.pop();
        }
    }
}
```

The transfer only runs when `deStack` is empty. During the transfer the order of elements is reversed: the oldest element of `enStack`, which sits at its bottom, ends up on top of `deStack`.

---

### pop / peek

```cpp
int pop() {
    moveElement();

    int head = deStack.top();
    deStack.pop();

    return head;
}

int peek() {
    moveElement();

    return deStack.top();
}
```

Both methods call `moveElement` first, to guarantee the head of the queue is up to date in `deStack`, then read `top()`.

---

### empty

```cpp
bool empty() {
    return enStack.empty() && deStack.empty();
}
```

The queue is empty only when both stacks are empty — elements can sit in either one depending on the call history.

---

### Why it works

`enStack` accumulates new insertions in arrival order. Once `deStack` runs dry, the entire `enStack` is poured into it in one go — the order of elements is inverted in the process, and the earliest unread element ends up on top of `deStack`.

While `deStack` is not empty, `enStack` keeps accumulating new elements without a transfer — they only enter the queue once the current `deStack` has been drained to the bottom.

`push` never touches `deStack`, `pop`/`peek` never touch `enStack` directly — they call `moveElement`, which decides whether a transfer is needed.

---

### Operations

| Operation  | Complexity           | Description                                    |
| ---------- | --------------------- | ------------------------------------------------ |
| `push(x)`  | O(1)                   | insertion into `enStack`                          |
| `pop()`    | amortized O(1)         | remove the head of the queue, transfer if needed  |
| `peek()`   | amortized O(1)         | read the head of the queue, transfer if needed    |
| `empty()`  | O(1)                   | checks both stacks                                |

A single `pop`/`peek` call can cost O(n) if it triggers a transfer, but each element moves from `enStack` to `deStack` at most once over its entire lifetime — hence the amortized O(1).

---

### Implementation details

* the transfer is conditional, guarded by `if (deStack.empty())`, not run unconditionally on every call
* both stacks are private, accessible only through class methods
* `pop`/`peek` do not throw on an empty queue — the check is the caller's responsibility

---

### Why not a single vector with manual shifting

```cpp
std::vector<int> stack;
stack.erase(stack.begin()); // O(n) per pop call
```

This could also work, but:

* `erase(begin())` shifts the entire array in memory on every `pop`, with no amortization
* the operation is outside the allowed set of standard stack operations
* `std::stack` gives no random access to the middle — a manual shift is impossible by construction

---

### Limitations

* no empty queue check in `pop()`/`peek()`
* amortized complexity, not strict O(1) per call — after enough accumulation, a single `pop` can cost O(n)

---

<br>

> Голова не переносится каждый раз — только когда рабочий стек пуст.
>
> The head is not moved every time — only when the working stack runs dry.