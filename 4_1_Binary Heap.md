## 1. Теоретический минимум

**Куча (heap)** — это специализированное дерево, в котором выполняется *основное свойство кучи*:

> Для любого узла `A` и его потомка `B` ключ `A` ≥ ключ `B` (max-куча)  
> или ключ `A` ≤ ключ `B` (min-куча).

Таким образом, в **max-куче** корень всегда содержит максимальный элемент всей структуры, а в **min-куче** — минимальный. Мы будем рассматривать max-кучу; переход к min-куче потребует только смены знаков в сравнениях.

**Двоичная куча** — это куча, в которой каждый узел имеет не более двух потомков, а само дерево является *полным*: все уровни, кроме, возможно, последнего, заполнены целиком, и последний уровень заполняется слева направо.

Именно полнота дерева позволяет хранить двоичную кучу не в виде разрозненных узлов с указателями, а очень компактно — в обычном массиве.

---

## 2. Представление двоичной кучи в массиве

Пронумеруем узлы полного двоичного дерева по уровням сверху вниз и слева направо, начиная с индекса 0. Тогда для узла с индексом `i`:

- индекс **левого потомка**: `2*i + 1`;
- индекс **правого потомка**: `2*i + 2`;
- индекс **родителя**: `(i - 1) / 2` (целочисленное деление).

**Пример.** Max-куча `[50, 30, 40, 10, 20]`:

```
Индексы:   0   1   2   3   4
Массив:  [50, 30, 40, 10, 20]
```

Дерево:

```
       50 (0)
       /   \
    30(1)  40(2)
    /  \
 10(3) 20(4)
```

Проверим формулы:
- Левый потомок корня (0) = `2·0+1 = 1` → 30.
- Правый потомок корня = `2·0+2 = 2` → 40.
- У узла 1 (`30`): левый = `2·1+1=3` → 10, правый = `4` → 20.

Такая нумерация гарантирует отсутствие «дырок» в массиве, и по ней всегда можно однозначно перемещаться от родителя к потомкам и обратно.

В коде мы будем использовать `std::vector<int>` для хранения элементов кучи.

---

## 3. Операции над кучей как набор свободных функций

Начнём с реализации без класса. Представим, что куча — это просто переменная типа `std::vector<int>`, а все операции — отдельные функции, принимающие этот вектор по ссылке.

### 3.1. Вспомогательные просеивания

Свойство кучи может нарушаться при вставке или удалении элемента. Восстанавливают его две локальные операции:

- **Просеивание вверх** (`siftUp`) – поднимает элемент, пока он больше родителя.
- **Просеивание вниз** (`siftDown`) – опускает элемент, пока он меньше хотя бы одного из потомков.

Напишем их как свободные функции, работающие с вектором `data`.

```cpp
#include <vector>
#include <algorithm>   // для std::swap

// Просеивание элемента с индексом index вверх по куче
void siftUp(std::vector<int>& data, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (data[index] <= data[parent]) {
            break;   // свойство кучи не нарушено
        }
        std::swap(data[index], data[parent]);
        index = parent;   // продолжаем подъём
    }
}

// Просеивание элемента с индексом index вниз по куче
void siftDown(std::vector<int>& data, int index) {
    int size = data.size();
    while (true) {
        int left  = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if (left < size && data[left] > data[largest]) {
            largest = left;
        }
        if (right < size && data[right] > data[largest]) {
            largest = right;
        }

        if (largest == index) {
            break;   // элемент на своём месте
        }

        std::swap(data[index], data[largest]);
        index = largest;   // опускаемся дальше
    }
}
```

> **Пояснение.** `siftUp` работает за время, пропорциональное высоте дерева — O(log n), потому что на каждом шаге индекс уменьшается примерно вдвое. `siftDown` каждый раз выбирает наибольшего из себя и двух потомков и меняется с ним, если потомок больше; в худшем случае элемент проходит высоту дерева, также O(log n).

### 3.2. Базовые операции над кучей

Используя эти две функции, реализуем основные операции.

#### Вставка элемента (`push`)

1. Кладём новый элемент в конец массива (левое свободное место последнего уровня).
2. Проталкиваем его вверх вызовом `siftUp`.

```cpp
void heapPush(std::vector<int>& data, int value) {
    data.push_back(value);
    siftUp(data, data.size() - 1);
}
```

#### Получение максимума (`top`) и удаление максимума (`pop`)

Максимальный элемент всегда в корне — `data[0]`.

**Удаление:**
1. Копируем последний элемент на место корня.
2. Удаляем последний элемент (дубликат).
3. Восстанавливаем свойство кучи просеиванием нового корня вниз.

```cpp
int heapTop(const std::vector<int>& data) {
    // для простоты полагаем, что куча не пуста
    return data[0];
}

void heapPop(std::vector<int>& data) {
    // также предполагаем, что data не пуст
    data[0] = data.back();
    data.pop_back();
    if (!data.empty()) {
        siftDown(data, 0);
    }
}
```

#### Построение кучи из неупорядоченного массива (`buildHeap`)

Если у нас уже есть заполненный вектор, можно превратить его в корректную кучу, вызвав `siftDown` для всех узлов, не являющихся листьями, в обратном порядке:

```cpp
void buildHeap(std::vector<int>& data) {
    int n = data.size();
    // Начинаем с последнего родителя: индекс n/2 - 1
    for (int i = n / 2 - 1; i >= 0; --i) {
        siftDown(data, i);
    }
}
```

Этот алгоритм работает за O(n), а не за O(n log n). Подробное доказательство оставим для самостоятельного изучения, но интуитивно: большинство элементов находятся на нижних уровнях и опускаются на малое количество шагов.

#### Увеличение и уменьшение ключа

Иногда нужно изменить приоритет уже существующего элемента (например, в алгоритме Дейкстры). Мы знаем его позицию `index`.

```cpp
void heapIncreaseKey(std::vector<int>& data, int index, int newValue) {
    if (newValue < data[index]) {
        // нарушение контракта: новое значение должно быть не меньше старого
        return;
    }
    data[index] = newValue;
    siftUp(data, index);
}

void heapDecreaseKey(std::vector<int>& data, int index, int newValue) {
    if (newValue > data[index]) {
        return; // контракт: новое значение не больше старого
    }
    data[index] = newValue;
    siftDown(data, index);
}
```

После увеличения ключ мог стать больше родителя → `siftUp`. После уменьшения — меньше потомков → `siftDown`.

#### Слияние двух куч

Классический наивный способ: вставить все элементы второй кучи в первую.

```cpp
void heapMerge(std::vector<int>& data, std::vector<int>& other) {
    for (int val : other) {
        heapPush(data, val);
    }
    other.clear();  // по соглашению забираем элементы
}
```

Сложность: O(m log(n+m)), где m — размер второй кучи, n — первой. Можно лучше: сложить массивы и вызвать `buildHeap` за O(n+m).

### 3.3. Пример использования свободных функций

```cpp
#include <iostream>
#include <vector>

// ... здесь определённые выше функции ...

int main() {
    std::vector<int> heap;

    // Вставка
    heapPush(heap, 20);
    heapPush(heap, 10);
    heapPush(heap, 40);
    heapPush(heap, 30);
    heapPush(heap, 50);

    std::cout << "Максимум: " << heapTop(heap) << "\n"; // 50

    heapPop(heap);
    std::cout << "После pop: " << heapTop(heap) << "\n"; // 40

    // Увеличим ключ на позиции 1 (пусть там 30) до 60
    heapIncreaseKey(heap, 1, 60);
    std::cout << "После increase: " << heapTop(heap) << "\n"; // 60

    // Построим кучу из массива
    std::vector<int> arr = {15, 5, 20, 1, 17, 10};
    buildHeap(arr);
    std::cout << "Корень построенной кучи: " << heapTop(arr) << "\n"; // 20

    // Слияние
    heapMerge(heap, arr);
    std::cout << "Размер после слияния: " << heap.size() << "\n";
    while (!heap.empty()) {
        std::cout << heapTop(heap) << " ";
        heapPop(heap);
    }
    // Вывод: все элементы по убыванию (кучная сортировка)
    return 0;
}
```

Такой подход наглядно показывает, что куча — это просто массив + соблюдение правил. Но у него есть недостатки: каждый вызов требует передавать вектор явно, и ничто не мешает случайно «поломать» кучу, изменив массив напрямую без вызова `siftUp`/`siftDown`. Поэтому на практике данные и операции объединяют в класс, обеспечивающий инкапсуляцию.

---

## 4. Инкапсуляция в класс `MaxHeap`

Теперь «обернём» наш массив и функции в единую структуру, чтобы:

- скрыть прямую работу с вектором от пользователя;
- гарантировать, что все изменения проходят через правильные методы;
- предоставить удобный интерфейс.

### 4.1. Структура класса

```cpp
#include <vector>
#include <stdexcept>
#include <iostream>

class MaxHeap {
private:
    std::vector<int> data;   // скрытое хранилище кучи

    // Приватные методы – их может вызывать только сам класс
    void siftUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (data[index] <= data[parent]) {
                break;
            }
            std::swap(data[index], data[parent]);
            index = parent;
        }
    }

    void siftDown(int index) {
        int size = data.size();
        while (true) {
            int left  = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size && data[left] > data[largest]) {
                largest = left;
            }
            if (right < size && data[right] > data[largest]) {
                largest = right;
            }
            if (largest == index) break;

            std::swap(data[index], data[largest]);
            index = largest;
        }
    }

public:
    // Конструкторы
    MaxHeap() = default;   // пустая куча

    explicit MaxHeap(const std::vector<int>& arr) : data(arr) {
        int n = data.size();
        for (int i = n / 2 - 1; i >= 0; --i) {
            siftDown(i);
        }
    }

    // Публичный интерфейс – те же операции, что и раньше
    void push(int value) {
        data.push_back(value);
        siftUp(data.size() - 1);
    }

    int top() const {
        if (data.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return data[0];
    }

    void pop() {
        if (data.empty()) {
            throw std::runtime_error("Heap is empty");
        }
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) {
            siftDown(0);
        }
    }

    void increaseKey(int index, int newValue) {
        if (index < 0 || index >= static_cast<int>(data.size())) {
            throw std::out_of_range("Index out of range");
        }
        if (newValue < data[index]) {
            throw std::invalid_argument("New value is smaller than current");
        }
        data[index] = newValue;
        siftUp(index);
    }

    void decreaseKey(int index, int newValue) {
        if (index < 0 || index >= static_cast<int>(data.size())) {
            throw std::out_of_range("Index out of range");
        }
        if (newValue > data[index]) {
            throw std::invalid_argument("New value is larger than current");
        }
        data[index] = newValue;
        siftDown(index);
    }

    void merge(MaxHeap& other) {
        for (int val : other.data) {
            this->push(val);
        }
        other.data.clear();
    }

    bool empty() const { return data.empty(); }
    size_t size() const { return data.size(); }
};
```

### 4.2. Что изменилось по сравнению со свободными функциями?

1. **Данные стали приватным полем `data`.** Прямой доступ извне невозможен, что защищает целостность кучи.
2. **`siftUp` и `siftDown`** объявлены в `private`, потому что пользователю не нужно просеивать элементы вручную — это внутренние вспомогательные механизмы.
3. **Публичные методы** (`push`, `pop`, `top`, `increaseKey`, `decreaseKey`, `merge`) предоставляют ровно те операции, которые мы определили. Их сигнатуры просты и не требуют явной передачи вектора.
4. **Добавлены проверки ошибок:** исключения при попытке получить максимум из пустой кучи, неверном индексе или некорректном изменении ключа.
5. **Конструкторы** позволяют создать либо пустую кучу, либо кучу из готового массива (с автоматическим построением свойства кучи за O(n)).

Такая инкапсуляция делает использование кучи безопасным и интуитивным.

### 4.3. Пример работы с классом

```cpp
int main() {
    MaxHeap heap;

    heap.push(20);
    heap.push(10);
    heap.push(40);
    heap.push(30);
    heap.push(50);

    std::cout << "Максимальный элемент: " << heap.top() << "\n"; // 50

    heap.pop();
    std::cout << "После удаления максимума: " << heap.top() << "\n"; // 40

    // Построим вторую кучу из массива
    std::vector<int> arr = {15, 5, 20, 1, 17, 10};
    MaxHeap heap2(arr);
    std::cout << "Максимум второй кучи: " << heap2.top() << "\n"; // 20

    // Слияние
    heap.merge(heap2);
    std::cout << "Размер после слияния: " << heap.size() << "\n"; // 10

    // Извлечём все элементы – получим сортировку по убыванию
    while (!heap.empty()) {
        std::cout << heap.top() << " ";
        heap.pop();
    }
    return 0;
}
```

---

## 5. Временная сложность операций

| Операция             | Сложность      |
|----------------------|----------------|
| `push`               | O(log n)       |
| `pop`                | O(log n)       |
| `top`                | O(1)           |
| `increaseKey`        | O(log n)       |
| `decreaseKey`        | O(log n)       |
| построение из массива | O(n)           |
| `merge` (простое)    | O(m log(n+m))  |

Двоичная куча занимает O(n) памяти.

---

## 6. Переход к min-куче

Чтобы превратить реализацию в min-кучу, достаточно изменить знаки сравнения:

- в `siftUp`: `data[index] < data[parent]`;
- в `siftDown`: искать наименьший `smallest` вместо `largest`;
- в `increaseKey` / `decreaseKey` соответственно меняется логика разрешённых изменений (увеличение или уменьшение).

В профессиональном коде используют шаблонный класс со стратегией сравнения, как это сделано в `std::priority_queue`.

---

## 7. Связь со стандартной библиотекой

В стандартной библиотеке C++ уже есть адаптер `std::priority_queue`, который внутри использует кучу (по умолчанию `std::vector` и max-кучу):

```cpp
#include <queue>
std::priority_queue<int> maxPQ;   // max-куча
std::priority_queue<int, std::vector<int>, std::greater<int>> minPQ; // min-куча
```

Есть и низкоуровневые алгоритмы для превращения произвольного контейнера в кучу:

```cpp
std::vector<int> v = {3, 1, 4, 1, 5};
std::make_heap(v.begin(), v.end());   // построение
v.push_back(9);
std::push_heap(v.begin(), v.end());   // вставка с сохранением кучи
std::pop_heap(v.begin(), v.end());    // перемещение максимума в конец
v.pop_back();
```
---

## Заключение
**Вопросы для самопроверки:**
1. Почему индексация потомков именно `2i+1` и `2i+2`? Как изменились бы формулы при нумерации с 1?
2. Почему построение кучи из массива функцией `buildHeap` выполняется за линейное время?
3. Какие предосторожности нужно предпринять, чтобы не нарушить свойство кучи при прямом изменении элемента?
4. Как бы вы модифицировали класс `MaxHeap`, чтобы он работал не только с `int`, а с любым типом, поддерживающим сравнение?
5. Предложите способ эффективного слияния двух куч без повторной вставки каждого элемента.
