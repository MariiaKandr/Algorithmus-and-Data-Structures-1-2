## Splay-деревьев (самоперестраивающиеся деревья)

- **Амортизированная сложность:** O(log n) в среднем, но отдельные операции могут быть O(n) (например, при последовательном доступе к элементам в порядке возрастания). Однако за счёт splay последующие операции ускоряются.
- **Преимущества:** Простота реализации (нет дополнительных полей), хорошая производительность на реальных данных с локальностью обращений.
- **Недостатки:** В худшем случае дерево может быть сильно несбалансированным (вырожденным), но амортизация гарантирует эффективность.
- **Применение:** Кэши, сборщики мусора, реализации `std::map` в некоторых библиотеках (например, в старых версиях GCC использовались splay-деревья для ассоциативных контейнеров).

### 1. Узел Splay-дерева

Splay-дерево не требует дополнительных полей (ни высоты, ни цвета). Достаточно обычного узла BST с указателями на родителей (для упрощения реализации splay-операций).

```cpp
struct SplayNode {
    int key;
    SplayNode* left;
    SplayNode* right;
    SplayNode* parent;
    
    SplayNode(int k) : key(k), left(nullptr), right(nullptr), parent(nullptr) {}
};
```

> **Отличие от AVL и RB:**  
> Никакой балансировочной информации в узле нет. «Балансировка» происходит за счёт операции **splay** — перемещения accessed узла в корень с помощью серии поворотов.

---

### 2. Базовые повороты (как в BST, но с обновлением parent)

Повороты в splay-деревьях аналогичны AVL, но обязательно поддерживают указатель на родителя.

```cpp
// Правый поворот вокруг узла x
void rotateRight(SplayNode*& root, SplayNode* x) {
    SplayNode* y = x->left;
    if (!y) return;
    
    x->left = y->right;
    if (y->right) y->right->parent = x;
    
    y->parent = x->parent;
    if (!x->parent)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    
    y->right = x;
    x->parent = y;
}

// Левый поворот вокруг узла x
void rotateLeft(SplayNode*& root, SplayNode* x) {
    SplayNode* y = x->right;
    if (!y) return;
    
    x->right = y->left;
    if (y->left) y->left->parent = x;
    
    y->parent = x->parent;
    if (!x->parent)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    
    y->left = x;
    x->parent = y;
}
```

---

### 3. Операция Splay (главная «фишка» дерева)

Splay поднимает узел `x` в корень дерева, выполняя последовательные повороты. Существует три случая:

- **Zig** (x — ребёнок корня) → один поворот.
- **Zig-Zig** (x, parent, grandparent выровнены в одну линию) → два одинаковых поворота (сначала вокруг grandparent, потом вокруг parent).
- **Zig-Zag** (x, parent, grandparent образуют «ломаную» линию) → два разных поворота (сначала вокруг parent, затем вокруг grandparent).

```cpp
void splay(SplayNode*& root, SplayNode* x) {
    if (!x) return;
    
    while (x->parent) {
        SplayNode* parent = x->parent;
        SplayNode* grand = parent->parent;
        
        if (!grand) {
            // Zig: один поворот
            if (x == parent->left)
                rotateRight(root, parent);
            else
                rotateLeft(root, parent);
        } 
        else {
            // Zig-Zig: оба поворота в одном направлении
            if (x == parent->left && parent == grand->left) {
                rotateRight(root, grand);
                rotateRight(root, parent);
            }
            // Zig-Zag: разные направления
            else if (x == parent->right && parent == grand->right) {
                rotateLeft(root, grand);
                rotateLeft(root, parent);
            }
            else if (x == parent->right && parent == grand->left) {
                rotateLeft(root, parent);
                rotateRight(root, grand);
            }
            else { // x == parent->left && parent == grand->right
                rotateRight(root, parent);
                rotateLeft(root, grand);
            }
        }
    }
    root = x; // x теперь корень
}
```

---

### 4. Операции поиска, вставки и удаления на основе Splay

#### Поиск
Выполняем обычный BST-поиск, затем делаем splay над найденным узлом (или над последним посещённым, если ключ не найден).

```cpp
SplayNode* find(SplayNode*& root, int key) {
    if (!root) return nullptr;
    
    SplayNode* current = root;
    SplayNode* last = nullptr;
    while (current) {
        last = current;
        if (key < current->key)
            current = current->left;
        else if (key > current->key)
            current = current->right;
        else {
            splay(root, current);
            return current;
        }
    }
    // Ключ не найден — поднимаем последний узел
    if (last) splay(root, last);
    return nullptr;
}
```

#### Вставка
1. Выполнить обычную BST-вставку.
2. Сделать splay над новым узлом (он становится корнем).

```cpp
void insert(SplayNode*& root, int key) {
    if (!root) {
        root = new SplayNode(key);
        return;
    }
    
    // Сначала ищем, чтобы получить последний узел
    SplayNode* current = root;
    SplayNode* parent = nullptr;
    while (current) {
        parent = current;
        if (key < current->key)
            current = current->left;
        else if (key > current->key)
            current = current->right;
        else {
            // Ключ уже существует — просто поднимаем его
            splay(root, current);
            return;
        }
    }
    
    // Создаём новый узел
    SplayNode* newNode = new SplayNode(key);
    newNode->parent = parent;
    if (key < parent->key)
        parent->left = newNode;
    else
        parent->right = newNode;
    
    // Поднимаем новый узел в корень
    splay(root, newNode);
}
```

#### Удаление
Алгоритм (через splay):
1. Найти узел с ключом `key` (и поднять его в корень с помощью `find`).
2. Если не найден — завершить.
3. Теперь удаляемый узел — корень.
4. Если у корня нет левого ребёнка — просто заменяем корень на правого.
5. Иначе: находим максимальный узел в левом поддереве (через `findMax`), поднимаем его в корень левого поддерева, затем прикрепляем правое поддерево.

```cpp
SplayNode* findMax(SplayNode*& root) {
    if (!root) return nullptr;
    while (root->right) root = root->right;
    splay(root, root); // поднимаем максимум в корень
    return root;
}

void erase(SplayNode*& root, int key) {
    if (!root) return;
    
    // Находим узел и поднимаем его в корень
    if (!find(root, key)) return; // ключ не найден (find уже сделал splay последнего)
    
    // Теперь удаляемый узел — root
    SplayNode* leftSub = root->left;
    SplayNode* rightSub = root->right;
    
    delete root;
    
    if (!leftSub) {
        root = rightSub;
        if (root) root->parent = nullptr;
        return;
    }
    
    if (!rightSub) {
        root = leftSub;
        if (root) root->parent = nullptr;
        return;
    }
    
    // Отключаем левое поддерево от правого
    leftSub->parent = nullptr;
    rightSub->parent = nullptr;
    
    // Находим максимум в левом поддереве (он станет его корнем)
    root = leftSub;
    SplayNode* maxLeft = findMax(root); // splay максимума внутри leftSub
    
    // Присоединяем правое поддерево
    maxLeft->right = rightSub;
    rightSub->parent = maxLeft;
}
```

> **Важно:** В функции `findMax` мы делаем splay для максимального узла, поэтому после этого он становится корнем левого поддерева. У него нет правого ребёнка (он максимум), поэтому можно безопасно добавить `rightSub`.

---

### 5. Пример использования

```cpp
#include <iostream>
using namespace std;

// ... (все структуры и функции выше)

void inorder(SplayNode* root) {
    if (!root) return;
    inorder(root->left);
    cout << root->key << " ";
    inorder(root->right);
}

int main() {
    SplayNode* root = nullptr;
    
    // Вставка
    for (int v : {5, 2, 8, 1, 3, 7, 9}) {
        insert(root, v);
        cout << "После вставки " << v << ", корень: " << root->key << endl;
    }
    
    cout << "Inorder: "; inorder(root); cout << endl;
    
    // Поиск
    find(root, 3);
    cout << "После поиска 3, корень стал: " << root->key << endl;
    
    // Удаление
    erase(root, 8);
    cout << "После удаления 8, корень: " << root->key << endl;
    cout << "Inorder: "; inorder(root); cout << endl;
    
    return 0;
}
```

**Пример вывода:**
```
После вставки 5, корень: 5
После вставки 2, корень: 2
После вставки 8, корень: 8
После вставки 1, корень: 1
После вставки 3, корень: 3
После вставки 7, корень: 7
После вставки 9, корень: 9
Inorder: 1 2 3 5 7 8 9 
После поиска 3, корень стал: 3
После удаления 8, корень: 7
Inorder: 1 2 3 5 7 9 
```

---

### 6. Практика

**Задание 1*** — создать самоперестраивающееся дерево, где добавление и удаление основаны на операции splay.  
 
Дополнительно можно добавить:

- **Разделение (split)** дерева по ключу с помощью splay.
- **Слияние (merge)** двух деревьев, где все ключи одного меньше другого.

Эти операции полезны, но в задании они не указаны явно, поэтому достаточно `insert` и `erase` через splay.
