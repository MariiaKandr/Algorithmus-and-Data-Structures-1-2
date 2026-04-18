## Красно-чёрные деревья (Red-Black trees)

RB-деревья используются в стандартных библиотеках (C++ `std::map`, `std::set`, Java `TreeMap`). Так как RB-дерево хранит только цвет (1 байт на узел) вместо высоты (int), оно экономит память по сравнению с AVL. Вставка же требует не более 2 поворотов, удаление — до 3. Это быстрее, чем у AVL (много поворотов при удалении), но AVL обеспечивает более строгую балансировку и чуть быстрее поиск.

### 1. Узел красно-чёрного дерева (RB-узел)

В отличие от BST и AVL, RB-дерево не хранит высоту, а хранит **цвет** узла (красный или чёрный). Также для удобства алгоритмов обычно хранится указатель на родителя.

```cpp
enum Color { RED, BLACK };

struct RBNode {
    int key;
    RBNode* left;
    RBNode* right;
    RBNode* parent;
    Color color;
    
    RBNode(int k) : key(k), left(nullptr), right(nullptr), parent(nullptr), color(RED) {}
};
```

> **Почему новый узел красный?**  
> Чтобы не нарушать свойство 4 (одинаковое количество чёрных узлов на всех путях). Красный узел можно добавить, не меняя чёрную высоту. *Чёрная высота — количество чёрных узлов на пути от корня до конкретного узла или листа. В красно-чёрном дереве чёрная высота одинакова для всех путей от корня к листьям.*

---

### 2. Свойства красно-чёрного дерева (напоминание)

1. Каждый узел — либо красный, либо чёрный.
2. Корень — чёрный.
3. Все листья (NIL-узлы) — чёрные. В реализации мы будем использовать один глобальный `nullptr` или специальный `nil`-узел, но проще считать `nullptr` чёрным.
4. Если узел красный, то оба его сына — чёрные.
5. Для любого узла все пути от него до листьев содержат одинаковое количество чёрных узлов (чёрная высота).
   
*При добавлении или удалении узлов могут возникать ситуации, когда свойства дерева нарушаются. В таких случаях требуется перебалансировка — операции перекраски узлов и повороты дерева для восстановления свойств.*

---

### 3. Вспомогательные функции

Для навигации по дереву нам понадобятся:

```cpp
// Получение цвета узла (nullptr считается чёрным)
bool isRed(RBNode* node) {
    return node != nullptr && node->color == RED;
}

bool isBlack(RBNode* node) {
    return node == nullptr || node->color == BLACK;
}

// "Дедушка" и "дядя"
RBNode* grandparent(RBNode* node) {
    if (node && node->parent)
        return node->parent->parent;
    return nullptr;
}

RBNode* uncle(RBNode* node) {
    RBNode* g = grandparent(node);
    if (!g) return nullptr;
    if (node->parent == g->left)
        return g->right;
    else
        return g->left;
}
```

**Повороты** (аналогичны AVL, но нужно обновлять parent):

```cpp
// Левый поворот вокруг узла x
void rotateLeft(RBNode*& root, RBNode* x) {
    RBNode* y = x->right;
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

// Правый поворот
void rotateRight(RBNode*& root, RBNode* x) {
    RBNode* y = x->left;
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
```

---

### 4. Вставка узла и балансировка

**Алгоритм вставки:**

1. Выполнить обычную BST-вставку (новый узел красный).
2. Восстановить свойства RB-дерева с помощью функции `fixInsert`.

```cpp
void insert(RBNode*& root, int key) {
    // 1. Обычная BST-вставка
    RBNode* newNode = new RBNode(key);
    if (!root) {
        root = newNode;
        root->color = BLACK;   // корень всегда чёрный
        return;
    }
    
    RBNode* current = root;
    RBNode* parent = nullptr;
    while (current) {
        parent = current;
        if (key < current->key)
            current = current->left;
        else if (key > current->key)
            current = current->right;
        else {
            // ключ уже есть – ничего не делаем (или обновляем)
            delete newNode;
            return;
        }
    }
    
    newNode->parent = parent;
    if (key < parent->key)
        parent->left = newNode;
    else
        parent->right = newNode;
    
    // 2. Восстановление свойств
    fixInsert(root, newNode);
}
```

**Функция fixInsert** (балансировка после вставки).  
Случаи (пока текущий узел не корень и его родитель красный):

- **Случай 1:** Дядя красный → перекрашиваем родителя, дядю и дедушку, поднимаемся к дедушке.
- **Случай 2:** Дядя чёрный, и текущий узел — «внук» с разворотом (требуется двойной поворот).
- **Случай 3:** Дядя чёрный, и текущий узел — «прямой» потомок (один поворот и перекраска).

```cpp
void fixInsert(RBNode*& root, RBNode* node) {
    while (node != root && isRed(node->parent)) {
        RBNode* parent = node->parent;
        RBNode* grand = grandparent(node);
        RBNode* uncleNode = uncle(node);
        
        // Случай 1: дядя красный -> перекраска
        if (isRed(uncleNode)) {
            parent->color = BLACK;
            uncleNode->color = BLACK;
            grand->color = RED;
            node = grand;          // поднимаемся к дедушке
        }
        else {
            // Дядя чёрный -> повороты
            if (parent == grand->left) {
                // Случай 2: левый-правый (LR)
                if (node == parent->right) {
                    rotateLeft(root, parent);
                    node = parent;          // обновляем ссылку
                    parent = node->parent;
                }
                // Случай 3: левый-левый (LL) -> правый поворот
                rotateRight(root, grand);
                swap(parent->color, grand->color);
                node = parent;
            }
            else { // parent == grand->right
                // Случай 2: правый-левый (RL)
                if (node == parent->left) {
                    rotateRight(root, parent);
                    node = parent;
                    parent = node->parent;
                }
                // Случай 3: правый-правый (RR) -> левый поворот
                rotateLeft(root, grand);
                swap(parent->color, grand->color);
                node = parent;
            }
        }
    }
    root->color = BLACK; // гарантия свойства 2
}
```

---

### 5. Удаление узла из красно-чёрного дерева

Удаление сложнее вставки. Основная идея:

1. Найти удаляемый узел (как в BST).
2. Если у удаляемого узла меньше двух детей, заменяем его на потомка (или NIL) и удаляем.
3. Если два ребёнка — находим преемника (минимальный в правом поддереве), копируем ключ, затем удаляем преемника (у него не более одного ребёнка).
4. Если удалённый узел был **чёрным**, нарушается свойство 5 (чёрная высота). Нужно восстановить баланс с помощью `fixErase`, передавая заменяющий узел (ребёнка) и его родителя.

Приведём базовый каркас (полная реализация громоздка, но основные моменты):

```cpp
// Поиск минимального узла
RBNode* minimum(RBNode* node) {
    while (node && node->left) node = node->left;
    return node;
}

// Вспомогательная функция для замены одного поддерева другим
void transplant(RBNode*& root, RBNode* u, RBNode* v) {
    if (!u->parent)
        root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    if (v) v->parent = u->parent;
}

// Основное удаление
void erase(RBNode*& root, int key) {
    RBNode* z = root;
    while (z) {
        if (key < z->key) z = z->left;
        else if (key > z->key) z = z->right;
        else break;
    }
    if (!z) return; // нет такого ключа
    
    RBNode* y = z;
    RBNode* x = nullptr;
    Color yOriginalColor = y->color;
    
    if (!z->left) {
        x = z->right;
        transplant(root, z, z->right);
    }
    else if (!z->right) {
        x = z->left;
        transplant(root, z, z->left);
    }
    else {
        y = minimum(z->right);
        yOriginalColor = y->color;
        x = y->right;
        if (y->parent == z) {
            if (x) x->parent = y;
        } else {
            transplant(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    delete z;
    
    // Если удалённый узел был чёрным — балансировка
    if (yOriginalColor == BLACK)
        fixErase(root, x);
}
```

Функция `fixErase` обрабатывает случаи, когда нарушена чёрная высота. Она принимает узел `x`, который может быть `nullptr` (NIL). Балансировка вращает и перекрашивает узлы, пока не восстановится свойство.

Из-за объёма здесь приведу упрощённую логику:

- Пока `x` не корень и `x` чёрный:
  - Если `x` — левый ребёнок своего родителя:
    - Брат `w` правый.
    - Случай 1: брат красный → перекраска + левый поворот.
    - Случай 2: оба ребёнка брата чёрные → перекраска брата, `x` поднимается к родителю.
    - Случай 3: левый ребёнок брата красный, правый чёрный → перекраска + правый поворот.
    - Случай 4: правый ребёнок брата красный → перекраска + левый поворот, завершение.
  - Симметрично для `x` — правый ребёнок.
- В конце `x` красим в чёрный.

Полный код `fixErase` можно найти в стандартной литературе; для выполнения практических заданий достаточно реализовать упрощённый вариант, работающий на тестовых данных.

---

### 6. Пример использования и проверка свойств

```cpp
#include <iostream>
using namespace std;

// ... (все структуры и функции выше)

void inorder(RBNode* root) {
    if (!root) return;
    inorder(root->left);
    cout << root->key << "(" << (root->color == RED ? "R" : "B") << ") ";
    inorder(root->right);
}

int main() {
    RBNode* root = nullptr;
    
    int arr[] = {7, 3, 18, 10, 22, 8, 11, 26};
    for (int v : arr) insert(root, v);
    
    cout << "Inorder: "; inorder(root); cout << endl;
    
    erase(root, 18);
    cout << "После удаления 18: "; inorder(root); cout << endl;
    
    return 0;
}
```

---

### 7. Практика

**Задание 1** — построить обычное BST и проверить свойства красно-чёрного дерева.  
*Решение:* Создать BST (без балансировки), покрасить узлы (например, все в чёрный) и проверить свойства. Но, конечно, BST не будет удовлетворять свойствам RB (может быть красный родитель и красный ребёнок и т.д.). Задача, вероятно, предлагает убедиться, что простое BST не является RB-деревом. Можно вычислить чёрную высоту путей и увидеть различие.

**Задание 2*** — реализовать красно-чёрное дерево с вставкой и удалением.  
*Решение:* Использовать приведённый выше код, дополнить полной реализацией `fixErase`. Убедиться, что все свойства выполняются после каждой операции.
