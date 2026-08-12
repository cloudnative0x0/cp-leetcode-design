# MyHashSet

<p style="text-align: left">
  <a href="#русский">Русский</a> ・ <a href="#english">English</a>
</p>

---

## Русский

### Идея

Задача — реализовать хеш-множество с фиксированным числом бакетов, используя метод цепочек (separate chaining) для разрешения коллизий. Каждый бакет — односвязный список узлов `Node`, владение узлами оформлено через `std::unique_ptr`.

Структура не делает рехеширование: `bucket_count` задаётся один раз в конструкторе и не меняется на протяжении жизни объекта.

---

### Хеш-функция

Индекс бакета вычисляется через остаток от деления на `bucket_count`:

$$
h(k) = k \bmod m
$$

где $k$ — ключ, $m$ — `bucket_count`.

Для `int` остаток от деления в C++ может быть отрицательным при отрицательном $k$, поэтому индекс нормализуется отдельной проверкой:

$$
h(k) =
\begin{cases}
(k \bmod m) + m, & \text{если } k \bmod m < 0 \\
k \bmod m, & \text{иначе}
\end{cases}
$$

Приведение к `long long` перед взятием остатка нужно, чтобы избежать переполнения на границах диапазона `int`, когда `bucket_count` приводится к знаковому типу.

---

### Разрешение коллизий

Два разных ключа $k_1 \neq k_2$ считаются коллизией, если $h(k_1) = h(k_2)$. При равномерном хешировании и случайных ключах вероятность коллизии одной конкретной пары ключей:

$$
P(h(k_1) = h(k_2)) = \frac{1}{m}
$$

Коллизия не приводит к перезаписи — оба ключа хранятся в одном бакете как узлы связного списка, `add` проходит список до конца перед вставкой нового узла.

---

### Коэффициент заполнения

Если в множестве хранится $n$ элементов при $m$ бакетах, коэффициент заполнения:

$$
\alpha = \frac{n}{m}
$$

При равномерном распределении ключей по бакетам математическое ожидание длины цепочки в одном бакете:

$$
E[\text{длина цепочки}] = \alpha
$$

Поскольку `bucket_count` фиксирован и рехеширования нет, $\alpha$ растёт линейно вместе с $n$ и ничем не ограничена сверху — в отличие от `std::unordered_set`, где рехеш держит $\alpha$ ниже порога.

---

### Сложность операций

Ожидаемое время операции `add` / `remove` / `contains` при равномерном хешировании и методе цепочек:

$$
O(1 + \alpha)
$$

При фиксированном $m$ и растущем $n$ выражение превращается в:

$$
O\!\left(1 + \frac{n}{m}\right)
$$

При $n \ll m$ (например, `n` в пределах нескольких тысяч, `m = 10007`) это близко к $O(1)$. При $n \gg m$ поведение вырождается в линейный поиск по списку, то есть $O(n)$ на операцию в худшем случае.

Худший случай для отдельной операции — когда все хранимые ключи попали в один бакет:

$$
O(n)
$$

такое возможно как патологический случай при не случайных ключах или намеренно подобранной коллизии.

---

### Структура класса

```cpp
struct Node {
    int key;
    std::unique_ptr<Node> next;
};

std::vector<std::unique_ptr<Node>> buckets;
size_t bucket_count;
```

* `buckets` — вектор голов связных списков, размер равен `bucket_count`
* `bucket_count` — число бакетов, задаётся один раз в конструкторе
* `Node::next` — владеющий указатель на следующий узел цепочки

---

### Инициализация

```cpp
explicit MyHashSet(size_t capacity = 10007)
    : buckets(capacity), bucket_count(capacity) {}
```

`capacity` по умолчанию — простое число 10007, выбор простого числа снижает вероятность систематических коллизий, когда ключи кратны общим делителям `bucket_count`.

Копирование запрещено (`unique_ptr` не копируется неявно), перемещение разрешено через `= default`.

---

### add

```cpp
void add(int key) {
    size_t index = hash(key);
    ...
}
```

Если бакет пуст — создаётся первый узел. Если не пуст — список обходится до конца или до совпадения ключа; при совпадении вставка не происходит (множество, не мультимножество), при отсутствии совпадения новый узел добавляется в хвост списка.

---

### remove

```cpp
void remove(int key) {
    size_t index = hash(key);
    ...
}
```

Особый случай — удаление головы бакета: `buckets[index]` переприсваивается на `next` головы, старый узел освобождается автоматически через `unique_ptr`. Для узла не в голове — `current->next` переприсваивается на `next->next`, минуя удаляемый узел.

---

### contains

```cpp
bool contains(int key) const noexcept {
    size_t index = hash(key);
    ...
}
```

Линейный обход цепочки в вычисленном бакете, без побочных эффектов на структуру.

---

### Операции

| Операция      | Сложность (ожидание) | Сложность (худший случай) |
| ------------- | --------------------- | -------------------------- |
| `add(x)`      | $O(1+\alpha)$          | $O(n)$                      |
| `remove(x)`   | $O(1+\alpha)$          | $O(n)$                      |
| `contains(x)` | $O(1+\alpha)$          | $O(n)$                      |
| `hash(x)`     | $O(1)$                 | $O(1)$                      |

---

### Почему не открытая адресация

```cpp
std::vector<std::optional<int>> table; // линейное пробирование
```

Метод цепочек выбран вместо открытой адресации по нескольким причинам:

* при коллизии не нужно искать следующий свободный слот по всей таблице — переполнение бакета не задевает соседние бакеты
* удаление узла не требует специальной метки "удалено" (tombstone), которая усложняет открытую адресацию
* при отсутствии рехеширования открытая адресация деградирует хуже: таблица может полностью заполниться и вставка станет невозможной, тогда как цепочка продолжает расти

---

### Детали реализации

* `bucket_count` не меняется после конструктора — рехеширования нет
* владение узлами цепочки полностью в `unique_ptr`, ручного `delete` нет
* `hash` помечен `noexcept`, не бросает исключений
* приведение `bucket_count` к `long long` перед делением — защита от переполнения при приведении знаков

---

### Ограничения

* без рехеширования при $n \gg m$ операции деградируют до $O(n)$
* нет метода изменения `bucket_count` во время жизни объекта
* `contains` не различает "ключа нет" от ошибки хеширования — оба случая дают `false`

---

## English

### Idea

The task is to implement a hash set with a fixed number of buckets, using separate chaining to resolve collisions. Each bucket is a singly linked list of `Node` objects, ownership of nodes goes through `std::unique_ptr`.

The structure never rehashes: `bucket_count` is set once in the constructor and stays fixed for the lifetime of the object.

---

### Hash function

The bucket index is computed as the remainder of division by `bucket_count`:

$$
h(k) = k \bmod m
$$

where $k$ is the key and $m$ is `bucket_count`.

For `int`, the C++ remainder can be negative when $k$ is negative, so the index is normalized with a separate check:

$$
h(k) =
\begin{cases}
(k \bmod m) + m, & \text{if } k \bmod m < 0 \\
k \bmod m, & \text{otherwise}
\end{cases}
$$

Casting to `long long` before taking the remainder avoids overflow at the edges of the `int` range when `bucket_count` is cast to a signed type.

---

### Collision resolution

Two distinct keys $k_1 \neq k_2$ collide when $h(k_1) = h(k_2)$. Under uniform hashing with random keys, the probability that a specific pair collides is:

$$
P(h(k_1) = h(k_2)) = \frac{1}{m}
$$

A collision does not overwrite anything — both keys are stored in the same bucket as linked-list nodes, `add` walks the list to its end before inserting a new node.

---

### Load factor

With $n$ elements stored across $m$ buckets, the load factor is:

$$
\alpha = \frac{n}{m}
$$

Under a uniform key distribution, the expected chain length in a single bucket is:

$$
E[\text{chain length}] = \alpha
$$

Since `bucket_count` is fixed and there is no rehashing, $\alpha$ grows linearly with $n$ and is unbounded above — unlike `std::unordered_set`, where rehashing keeps $\alpha$ under a threshold.

---

### Operation complexity

Expected time for `add` / `remove` / `contains` under uniform hashing with chaining:

$$
O(1 + \alpha)
$$

With $m$ fixed and $n$ growing, this becomes:

$$
O\!\left(1 + \frac{n}{m}\right)
$$

For $n \ll m$ (for example a few thousand entries with `m = 10007`) this is close to $O(1)$. For $n \gg m$ the behavior degenerates to a linear scan of a list, i.e. $O(n)$ per operation in the worst case.

The worst case for a single operation is every key landing in the same bucket:

$$
O(n)
$$

which is possible as a pathological case with non-random keys or a deliberately crafted collision.

---

### Class structure

```cpp
struct Node {
    int key;
    std::unique_ptr<Node> next;
};

std::vector<std::unique_ptr<Node>> buckets;
size_t bucket_count;
```

* `buckets` — a vector of linked-list heads, sized to `bucket_count`
* `bucket_count` — number of buckets, set once in the constructor
* `Node::next` — an owning pointer to the next node in the chain

---

### Initialization

```cpp
explicit MyHashSet(size_t capacity = 10007)
    : buckets(capacity), bucket_count(capacity) {}
```

`capacity` defaults to the prime number 10007; choosing a prime lowers the chance of systematic collisions when keys share common factors with `bucket_count`.

Copying is disabled (`unique_ptr` is not implicitly copyable), moving is enabled via `= default`.

---

### add

```cpp
void add(int key) {
    size_t index = hash(key);
    ...
}
```

If the bucket is empty, the first node is created. If not, the list is walked to its end or until a matching key is found; on a match nothing is inserted (a set, not a multiset), otherwise a new node is appended at the tail.

---

### remove

```cpp
void remove(int key) {
    size_t index = hash(key);
    ...
}
```

Removing the bucket head is a special case: `buckets[index]` is reassigned to the head's `next`, the old node is released automatically through `unique_ptr`. For a node past the head, `current->next` is reassigned to `next->next`, bypassing the removed node.

---

### contains

```cpp
bool contains(int key) const noexcept {
    size_t index = hash(key);
    ...
}
```

A linear walk of the chain in the computed bucket, with no side effects on the structure.

---

### Operations

| Operation     | Complexity (expected) | Complexity (worst case) |
| ------------- | ----------------------- | ------------------------- |
| `add(x)`      | $O(1+\alpha)$            | $O(n)$                     |
| `remove(x)`   | $O(1+\alpha)$            | $O(n)$                     |
| `contains(x)` | $O(1+\alpha)$            | $O(n)$                     |
| `hash(x)`     | $O(1)$                   | $O(1)$                     |

---

### Why not open addressing

```cpp
std::vector<std::optional<int>> table; // linear probing
```

Chaining is chosen over open addressing for a few reasons:

* on a collision there is no need to scan the whole table for the next free slot — an overflowing bucket does not touch its neighbors
* removing a node needs no tombstone marker, which complicates open addressing
* without rehashing, open addressing degrades worse: the table can fill up entirely and insertion becomes impossible, while a chain simply keeps growing

---

### Implementation details

* `bucket_count` never changes after the constructor — there is no rehashing
* chain node ownership is entirely through `unique_ptr`, no manual `delete`
* `hash` is marked `noexcept`, it does not throw
* `bucket_count` is cast to `long long` before division, guarding against overflow on the sign cast

---

### Limitations

* without rehashing, operations degrade to $O(n)$ once $n \gg m$
* no method to change `bucket_count` during the object's lifetime
* `contains` does not distinguish "key absent" from a hashing error — both return `false`

---

<br>

> Бакет не пуст — узел ищется в цепочке. Бакет пуст — узел создаётся.
>
> Bucket not empty — the node is searched in the chain. Bucket empty — the node is created.