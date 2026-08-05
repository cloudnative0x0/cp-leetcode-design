# MinStack

<p style="text-align: left">
  <a href="#русский">Русский</a> ・ <a href="#english">English</a>
</p>

---

## Русский

### Идея

Задача — реализовать стек с дополнительной операцией `getMin()`, которая возвращает минимальный элемент за O(1), не нарушая асимптотику остальных операций.

Ключевое требование — никакого пересчёта при каждом запросе минимума. Минимум должен быть доступен мгновенно, независимо от размера стека.

---

### Подход

Используются два массива одинакового размера:

* `Arr` — хранит значения стека
* `MinArr` — хранит минимум на каждом префиксе

Индекс `topIdx` указывает на первую свободную позицию (размер стека).

Инвариант:

```
MinArr[i] = min(Arr[0..i])
```

---

### Структура класса

```cpp
class MinStack {
    std::vector<int> Arr;
    std::vector<int> MinArr;
    int topIdx;
};
```

* `Arr` — основной стек
* `MinArr` — минимум на каждом шаге
* `topIdx` — текущий размер

---

### Инициализация

```cpp
MinStack() {
    Arr = std::vector<int>(3000);
    MinArr = std::vector<int>(3000);
    topIdx = 0;
}
```

Стартовый буфер фиксированного размера, чтобы уменьшить число реаллокаций.

---

### push

```cpp
void push(int value) {
    if (topIdx >= Arr.size()) {
        Arr.resize(Arr.size() * 2);
        MinArr.resize(MinArr.size() * 2);
    }

    Arr[topIdx] = value;

    if (topIdx == 0) {
        MinArr[topIdx] = value;
    } else {
        int currentMin = MinArr[topIdx - 1];

        if (value < currentMin) {
            MinArr[topIdx] = value;
        } else {
            MinArr[topIdx] = currentMin;
        }
    }

    topIdx++;
}
```

* проверка переполнения
* запись значения
* обновление минимума через предыдущий

---

### pop

```cpp
void pop() {
    topIdx--;
}
```

Удаление — уменьшение размера.

---

### top / getMin

```cpp
int top() const {
    return Arr[topIdx - 1];
}

int getMin() const {
    return MinArr[topIdx - 1];
}
```

Доступ к последнему элементу.

---

### Почему это работает

Минимум сохраняется на каждом шаге:

* либо остаётся прежним
* либо обновляется новым значением

При удалении ничего пересчитывать не нужно.

---

### Операции

| Операция   | Сложность | Описание                                  |
| ---------- | --------- | ----------------------------------------- |
| `push(x)`  | O(1)      | добавление элемента и обновление минимума |
| `pop()`    | O(1)      | удаление верхнего элемента                |
| `top()`    | O(1)      | получить верхний элемент                  |
| `getMin()` | O(1)      | получить текущий минимум                  |

---

### Детали реализации

* начальный размер — 3000
* увеличение ×2 при переполнении
* непрерывная память (`vector`)
* два массива

---

### Почему не использовать пары

```cpp
stack<pair<int,int>> st;
```

Здесь логика разнесена на два массива:

* меньше накладных расходов
* проще доступ
* инвариант выражен явно

---

### Ограничения

* нет проверки на пустой стек
* `pop()` ничего не возвращает

---

## English

### Idea

The task is to implement a stack with an additional `getMin()` operation that returns the minimum element in O(1), without affecting the complexity of other operations.

The key requirement is no recomputation on queries. The minimum must be available instantly regardless of stack size.

---

### Approach

Two arrays of equal size are used:

* `Arr` — stores stack values
* `MinArr` — stores the minimum for each prefix

The index `topIdx` points to the next free position (current stack size).

Invariant:

```
MinArr[i] = min(Arr[0..i])
```

---

### Class structure

```cpp
class MinStack {
    std::vector<int> Arr;
    std::vector<int> MinArr;
    int topIdx;
};
```

* `Arr` — main stack
* `MinArr` — minimum at each step
* `topIdx` — current size

---

### Initialization

```cpp
MinStack() {
    Arr = std::vector<int>(3000);
    MinArr = std::vector<int>(3000);
    topIdx = 0;
}
```

A fixed-size initial buffer reduces reallocations.

---

### push

```cpp
void push(int value) {
    if (topIdx >= Arr.size()) {
        Arr.resize(Arr.size() * 2);
        MinArr.resize(MinArr.size() * 2);
    }

    Arr[topIdx] = value;

    if (topIdx == 0) {
        MinArr[topIdx] = value;
    } else {
        int currentMin = MinArr[topIdx - 1];

        if (value < currentMin) {
            MinArr[topIdx] = value;
        } else {
            MinArr[topIdx] = currentMin;
        }
    }

    topIdx++;
}
```

* overflow check
* value write
* minimum update using previous value

---

### pop

```cpp
void pop() {
    topIdx--;
}
```

Removal is just size decrement.

---

### top / getMin

```cpp
int top() const {
    return Arr[topIdx - 1];
}

int getMin() const {
    return MinArr[topIdx - 1];
}
```

Direct access to the last element.

---

### Why it works

The minimum is stored at every step:

* it either stays the same
* or gets updated with the new value

No recomputation is needed on pop.

---

### Operations

| Operation  | Complexity | Description                       |
| ---------- | ---------- | --------------------------------- |
| `push(x)`  | O(1)       | insert element and update minimum |
| `pop()`    | O(1)       | remove top element                |
| `top()`    | O(1)       | get top element                   |
| `getMin()` | O(1)       | get current minimum               |

---

### Implementation details

* initial size — 3000
* resize ×2 on overflow
* contiguous memory (`vector`)
* two arrays

---

### Why not use pairs

```cpp
stack<pair<int,int>> st;
```

Here the logic is split into two arrays:

* less overhead
* simpler access
* explicit invariant

---

### Limitations

* no empty stack checks
* `pop()` does not return a value

---

<br>

> Минимум не ищется — он хранится заранее.
> 
> The minimum is not searched — it is already stored.
