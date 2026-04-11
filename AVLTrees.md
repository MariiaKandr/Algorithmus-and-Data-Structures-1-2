## АВЛ-деревья 

### 1. Отличие узла AVL от узла обычного BST

**Узел бинарного дерева поиска (BST)** обычно содержит только ключ и указатели на левого и правого потомка (иногда на родителя).  
**Узел АВЛ-дерева** дополнительно хранит **высоту поддерева** (или **баланс-фактор**). В классической реализации хранят высоту, а баланс вычисляется как разность высот правого и левого поддеревьев.

```cpp
// Узел BST (минимальный)
struct BSTNode {
    int key;
    BSTNode* left;
    BSTNode* right;
};

// Узел AVL
struct AVLNode {
    int key;
    AVLNode* left;
    AVLNode* right;
    int height;   // высота узла (максимальная длина пути до листа)
    // можно добавить parent, но для AVL обычно достаточно рекурсивного подъёма
};
```

> **Зачем нужна высота?**  
> Чтобы быстро вычислять баланс-фактор:  
> `balance = height(right) - height(left)`.  
> Если |balance| > 1 — дерево несбалансировано, нужен поворот.

---

### 2. Вспомогательные функции

Перед реализацией определим базовые операции:

```cpp
// Получение высоты узла (NULL-узел имеет высоту 0)
int height(AVLNode* node) {
    return node ? node->height : 0;
}

// Вычисление баланс-фактора
int balanceFactor(AVLNode* node) {
    return height(node->right) - height(node->left);
}

// Обновление высоты узла на основе высот детей
void updateHeight(AVLNode* node) {
    node->height = 1 + std::max(height(node->left), height(node->right));
}
```

---

### 3. Повороты — основа балансировки


**Малый правый поворот** (Right rotate)  
Применяется, когда левое поддерево слишком высокое (balance ≈ -2) и левый потомок сбалансирован или перекошен влево.

```cpp
AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // Выполняем поворот
    x->right = y;
    y->left = T2;

    // Обновляем высоты
    updateHeight(y);
    updateHeight(x);

    return x; // новый корень
}
```

**Малый левый поворот** (Left rotate)  
Когда правое поддерево слишком высокое (balance ≈ +2) и правый потомок сбалансирован или перекошен вправо.

```cpp
AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    updateHeight(x);
    updateHeight(y);

    return y;
}
```

**Большой правый поворот** (Left-Right rotate)  
Случай: balance = -2, но у левого потомка баланс = +1 (левое поддерево перекошено вправо).  
Сначала делаем малый левый поворот левого потомка, затем малый правый поворот.

```cpp
AVLNode* rotateLeftRight(AVLNode* node) {
    node->left = rotateLeft(node->left);
    return rotateRight(node);
}
```

**Большой левый поворот** (Right-Left rotate)  
Случай: balance = +2, у правого потомка баланс = -1.  
Сначала малый правый поворот правого потомка, затем малый левый поворот.

```cpp
AVLNode* rotateRightLeft(AVLNode* node) {
    node->right = rotateRight(node->right);
    return rotateLeft(node);
}
```

---

### 4. Вставка узла в АВЛ-дерево

Алгоритм:
1. Обычная вставка, как в BST.
2. Обновление высоты текущего узла.
3. Вычисление баланс-фактора.
4. Если |balance| > 1 — выполняем подходящий поворот.
5. Возвращаем (возможно, новый) корень поддерева.

```cpp
AVLNode* insert(AVLNode* root, int key) {
    // 1. Обычная BST-вставка
    if (!root) {
        return new AVLNode{key, nullptr, nullptr, 1};
    }

    if (key < root->key) {
        root->left = insert(root->left, key);
    } else if (key > root->key) {
        root->right = insert(root->right, key);
    } else {
        // Ключ уже существует — ничего не делаем (или обновляем данные)
        return root;
    }

    // 2. Обновляем высоту текущего узла
    updateHeight(root);

    // 3. Баланс-фактор
    int balance = balanceFactor(root);

    // 4. Балансировка (4 случая)

    // Левый перекос (balance = -2)
    if (balance < -1) {
        // Левый-левый случай: ключ вставлен в левое поддерево левого потомка
        if (key < root->left->key) {
            return rotateRight(root);
        }
        // Левый-правый случай: ключ вставлен в правое поддерево левого потомка
        else {
            return rotateLeftRight(root);
        }
    }

    // Правый перекос (balance = +2)
    if (balance > 1) {
        // Правый-правый случай
        if (key > root->right->key) {
            return rotateLeft(root);
        }
        // Правый-левый случай
        else {
            return rotateRightLeft(root);
        }
    }

    return root;
}
```

> **Важно:** после поворота корень поддерева меняется, поэтому функция возвращает новый корень.

---

### 5. Удаление узла из АВЛ-дерева

Удаление сложнее вставки, но идея та же:  
1. Стандартное удаление из BST (3 случая: лист, один потомок, два потомка).  
2. Обновление высоты.  
3. Проверка баланса и повороты на каждом уровне рекурсии.

```cpp
// Поиск минимального узла в поддереве
AVLNode* minNode(AVLNode* node) {
    while (node->left) node = node->left;
    return node;
}

AVLNode* erase(AVLNode* root, int key) {
    if (!root) return nullptr;

    // 1. Поиск и удаление как в BST
    if (key < root->key) {
        root->left = erase(root->left, key);
    }
    else if (key > root->key) {
        root->right = erase(root->right, key);
    }
    else {
        // Узел найден
        if (!root->left || !root->right) {
            // Случай 0 или 1 потомок
            AVLNode* temp = root->left ? root->left : root->right;
            delete root;
            return temp;
        } else {
            // Два потомка: заменяем ключ на минимальный из правого поддерева
            AVLNode* temp = minNode(root->right);
            root->key = temp->key;
            root->right = erase(root->right, temp->key);
        }
    }

    // Если после удаления дерево пустое
    if (!root) return nullptr;

    // 2. Обновляем высоту
    updateHeight(root);

    // 3. Балансировка (аналогично вставке, но без ключа)
    int balance = balanceFactor(root);

    // Левый перекос
    if (balance < -1) {
        // Проверяем баланс левого потомка, чтобы выбрать тип поворота
        int leftBalance = balanceFactor(root->left);
        if (leftBalance <= 0) {
            // left-left случай
            return rotateRight(root);
        } else {
            // left-right случай
            return rotateLeftRight(root);
        }
    }

    // Правый перекос
    if (balance > 1) {
        int rightBalance = balanceFactor(root->right);
        if (rightBalance >= 0) {
            // right-right случай
            return rotateLeft(root);
        } else {
            // right-left случай
            return rotateRightLeft(root);
        }
    }

    return root;
}
```

---

### 6. Пример использования и проверка

```cpp
#include <iostream>
#include <algorithm>

using namespace std;

// ... (все объявления структур и функций выше)

void inorder(AVLNode* root) {
    if (!root) return;
    inorder(root->left);
    cout << root->key << " (h=" << root->height << ", bf=" << balanceFactor(root) << ") ";
    inorder(root->right);
}

int main() {
    AVLNode* root = nullptr;

    // Вставка
    for (int val : {10, 20, 30, 40, 50, 25}) {
        root = insert(root, val);
        cout << "После вставки " << val << ": ";
        inorder(root);
        cout << endl;
    }

    // Удаление
    root = erase(root, 40);
    cout << "После удаления 40: ";
    inorder(root);
    cout << endl;

    return 0;
}
```

---

- **Сравнение с BST:** АВЛ-дерево гарантирует высоту O(log n), но требует дополнительной памяти на высоту и повороты при каждой модификации.
- **Сложность операций:** поиск O(log n), вставка и удаление O(log n) за счёт балансировки.
- **Альтернативы:** красно-чёрные деревья (меньше поворотов, но сложнее реализация), splay-деревья (амортизированная сложность).
