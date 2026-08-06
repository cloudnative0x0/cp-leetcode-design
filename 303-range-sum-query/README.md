# RangeSumQuery

<p style="text-align: left">
  <a href="#русский">Русский</a> ・ <a href="#english">English</a>
</p>

---

## Русский

### Идея

Задача — по неизменяемому массиву отвечать на запросы суммы отрезка `sumRange(left, right)`. Нужно избежать пересчёта суммы заново на каждом запросе.

---

### Подход

Строится массив префиксных сумм `Prefix` длиной `nums.size() + 1`:

```
Prefix[i] = сумма первых i элементов nums
```

`Prefix[0] = 0` — база, задаётся автоматически конструктором `vector`.

---

### Структура класса

```cpp
class RangeSumQuery {
    std::vector<int> Prefix;
};
```

* `Prefix` — единственное поле, вся работа завязана на нём

---

### Конструктор

```cpp
RangeSumQuery(std::vector<int>& nums) {
    Prefix = std::vector<int>(nums.size() + 1);

    for (size_t i = 0; i < nums.size(); i++) {
        Prefix[i+1] = Prefix[i] + nums[i];
    }
}
```

Размер `Prefix` берётся из реального размера `nums`, а не задаётся константой заранее.

Каждый шаг цикла добавляет к уже накопленной сумме ровно один элемент.

---

### sumRange

```cpp
int sumRange(int left, int right) {
    return Prefix[right+1] - Prefix[left];
}
```

Сумма отрезка получается вычитанием двух готовых префиксов, без обхода `nums`.

---

### Почему это работает

`Prefix[right+1]` — сумма всех элементов до `right` включительно. `Prefix[left]` — сумма всех элементов до `left`, не включая его. Разница — ровно сумма `nums[left..right]`.

Построение выполняется один раз, запросы читают готовые значения.

---

### Операции

| Операция      | Сложность | Описание                              |
| ------------- | --------- | -------------------------------------- |
| конструктор   | O(n)      | построение префиксных сумм             |
| `sumRange`    | O(1)      | сумма отрезка через вычитание          |

n — размер `nums`.

---

### Детали реализации

* размер `Prefix` берётся из `nums.size()`, магических констант нет
* индексация сдвинута на 1, чтобы `Prefix[0] = 0` покрывал случай `left == 0` без отдельной ветки

---

### Почему не считать сумму на каждом запросе

```cpp
int sumRange(int left, int right) {
    int sum = 0;
    for (int i = left; i <= right; i++) sum += nums[i];
    return sum;
}
```

Работает корректно, но O(n) на каждый вызов. При большом числе запросов суммарная сложность становится O(n·q), тогда как с префиксными суммами — O(n + q).

---

### Ограничения

* массив `nums` должен быть неизменяемым после построения `Prefix`; если элементы `nums` меняются, `Prefix` не обновляется автоматически
* нет проверки границ `left`/`right`

---

## English

### Idea

The task is to answer range sum queries `sumRange(left, right)` on an immutable array. The goal is to avoid recomputing the sum on every query.

---

### Approach

A prefix sum array `Prefix` of length `nums.size() + 1` is built:

```
Prefix[i] = sum of the first i elements of nums
```

`Prefix[0] = 0` — the base case, set automatically by the `vector` constructor.

---

### Class structure

```cpp
class RangeSumQuery {
    std::vector<int> Prefix;
};
```

* `Prefix` — the only field, all logic is built around it

---

### Constructor

```cpp
RangeSumQuery(std::vector<int>& nums) {
    Prefix = std::vector<int>(nums.size() + 1);

    for (size_t i = 0; i < nums.size(); i++) {
        Prefix[i+1] = Prefix[i] + nums[i];
    }
}
```

The size of `Prefix` comes from the actual size of `nums`, not a hardcoded constant.

Each loop step adds exactly one element to the already accumulated sum.

---

### sumRange

```cpp
int sumRange(int left, int right) {
    return Prefix[right+1] - Prefix[left];
}
```

The range sum comes from subtracting two already-built prefixes, without scanning `nums`.

---

### Why it works

`Prefix[right+1]` is the sum of everything up to and including `right`. `Prefix[left]` is the sum of everything before `left`. The difference is exactly the sum of `nums[left..right]`.

The build happens once, queries read already computed values.

---

### Operations

| Operation     | Complexity | Description                       |
| ------------- | ---------- | ---------------------------------- |
| constructor   | O(n)       | build the prefix sum array         |
| `sumRange`    | O(1)       | range sum via subtraction          |

n — size of `nums`.

---

### Implementation details

* `Prefix` size comes from `nums.size()`, no magic constants
* indexing is shifted by 1 so `Prefix[0] = 0` covers `left == 0` without a separate branch

---

### Why not sum on every query

```cpp
int sumRange(int left, int right) {
    int sum = 0;
    for (int i = left; i <= right; i++) sum += nums[i];
    return sum;
}
```

Correct, but O(n) per call. With many queries the total cost becomes O(n·q), while prefix sums give O(n + q).

---

### Limitations

* `nums` must stay immutable after `Prefix` is built; if `nums` changes, `Prefix` does not update automatically
* no bounds check on `left`/`right`

---

<br>

> Сумма не собирается заново — она уже разложена по шагам.
>
> The sum is not rebuilt — it is already broken down step by step.