# Дерево отрезков 
Например, есть массив [1, 3, 5, 7, 9, 11, 2, 4, 6], создадим для него дерево отрезков 
 
<img width="1063" height="1145" alt="image" src="https://github.com/user-attachments/assets/6683ffd8-5f78-4bb2-9a6d-346027f95ed4" />
 
Корень дерева (0-8) содержит максимальное значение всего массива (11).
Каждый узел разделяет массив на левую и правую части, вычисляя максимум в этом диапазоне.
Листовые узлы (0-0, 1-1, 2-2 и т. д.) содержат конкретные элементы массива.

Структура Node содержит границы [l, r] (включительно), указатели на левого и правого потомков и значение max_val – максимум на этом интервале.
•	Построение (buildTree) рекурсивно разбивает интервал пополам, создаёт листья для одиночных элементов и внутренние узлы, вычисляя максимум из детей.
•	Запрос максимума (queryMax):
o	Если запрашиваемый отрезок не пересекается с узлом → возвращаем очень маленькое число (-1e9), которое не влияет на максимум.
o	Если отрезок полностью покрывает узел → возвращаем node->max_val.
o	Иначе рекурсивно опрашиваем детей и возвращаем максимум из полученных значений.
•	Обновление (update) находит лист, меняет его значение, затем при возврате пересчитывает max_val для всех затронутых узлов.
•	Вывод дерева (printTree) помогает визуализировать структуру: для каждого узла печатается его интервал и максимум.
•	Освобождение памяти выполняется рекурсивным удалением всех узлов.

```
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Структура узла дерева отрезков (максимум)
struct Node {
    int l, r;          // границы интервала [l, r] (включительно)
    Node* left;        // левый потомок
    Node* right;       // правый потомок
    int max_val;       // максимум на интервале [l, r]

    // Конструктор для листа
    Node(int l, int r, int val) : l(l), r(r), left(nullptr), right(nullptr), max_val(val) {}

    // Конструктор для внутреннего узла (строится из двух детей)
    Node(int l, int r, Node* left, Node* right) : l(l), r(r), left(left), right(right) {
        max_val = max(left->max_val, right->max_val);
    }
};

// Рекурсивное построение дерева по массиву arr на интервале [l, r]
Node* buildTree(const vector<int>& arr, int l, int r) {
    if (l == r) {
        // Лист: один элемент
        return new Node(l, r, arr[l]);
    }
    int mid = (l + r) / 2;
    Node* leftChild = buildTree(arr, l, mid);
    Node* rightChild = buildTree(arr, mid + 1, r);
    return new Node(l, r, leftChild, rightChild);
}

// Запрос максимума на отрезке [ql, qr] (включительно)
int queryMax(Node* node, int ql, int qr) {
    if (node == nullptr) return -1e9;         // для пустого узла возвращаем очень маленькое значение
    // Полное непересечение
    if (qr < node->l || node->r < ql) return -1e9;
    // Полное покрытие
    if (ql <= node->l && node->r <= qr) return node->max_val;
    // Частичное пересечение – спускаемся к детям
    return max(queryMax(node->left, ql, qr), queryMax(node->right, ql, qr));
}

// Обновление: присвоить arr[index] = newVal
void update(Node* node, int index, int newVal) {
    if (node->l == node->r) {
        // Лист
        node->max_val = newVal;
        return;
    }
    int mid = (node->l + node->r) / 2;
    if (index <= mid)
        update(node->left, index, newVal);
    else
        update(node->right, index, newVal);
    node->max_val = max(node->left->max_val, node->right->max_val);
}

// Освобождение памяти (рекурсивное удаление)
void deleteTree(Node* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

// Вспомогательная функция для вывода дерева (обход в глубину)
void printTree(Node* node, int depth = 0) {
    if (!node) return;
    for (int i = 0; i < depth; i++) cout << "  ";
    cout << "[" << node->l << "," << node->r << "] max=" << node->max_val << endl;
    printTree(node->left, depth + 1);
    printTree(node->right, depth + 1);
}

int main() {
    // Исходный массив
    vector<int> arr = {1, 3, 5, 7, 9, 11, 2, 4, 6};
    int n = arr.size();

    // Построение дерева на интервале [0, n-1]
    Node* root = buildTree(arr, 0, n - 1);

    cout << "Структура дерева:\n";
    printTree(root);

    // Запросы максимума
    cout << "\nМаксимум на [0,8]: " << queryMax(root, 0, 8) << endl;          // 11
    cout << "Максимум на [2,5]: " << queryMax(root, 2, 5) << endl;          // 11
    cout << "Максимум на [3,7]: " << queryMax(root, 3, 7) << endl;          // 11
    cout << "Максимум на [4,4]: " << queryMax(root, 4, 4) << endl;          // 9

    // Обновление: заменяем элемент с индексом 5 (было 11) на 0
    update(root, 5, 0);
    cout << "\nПосле update(5,0):\n";
    cout << "Максимум на [0,8]: " << queryMax(root, 0, 8) << endl;          // 9 (бывший максимум 11 исчез)
    cout << "Максимум на [2,5]: " << queryMax(root, 2, 5) << endl;          // 9

    // Освобождение памяти
    deleteTree(root);

    return 0;
}
```
## В случае агрегирования сумм. 
Учтите, что в каждом узле может происходить несколько агрегаций!
```
// Структура узла дерева отрезков
struct Node {
    int B;          // начало интервала (включительно)
    int E;          // конец интервала (не включительно, или включительно – по соглашению)
    Node* left;     // левый потомок
    Node* right;    // правый потомок
    int sum;        // например, сумма элементов на интервале [B, E]

    // Конструктор для листа (интервал из одного элемента)
    Node(int b, int e, int val) : B(b), E(e), left(nullptr), right(nullptr), sum(val) {}

    // Конструктор для внутреннего узла
    Node(int b, int e, Node* l, Node* r) : B(b), E(e), left(l), right(r), sum(l->sum + r->sum) {}
};
// Рекурсивное построение дерева по массиву arr на интервале [l, r)
Node* buildTree(const vector<int>& arr, int l, int r) {
    if (r - l == 1) {
        // лист – один элемент
        return new Node(l, r, arr[l]);
    }
    int mid = (l + r) / 2;
    Node* leftChild = buildTree(arr, l, mid);
    Node* rightChild = buildTree(arr, mid, r);
    return new Node(l, r, leftChild, rightChild);
}

// Запрос суммы на отрезке [ql, qr) (полуинтервал)
int query(Node* node, int ql, int qr) {
    if (node == nullptr) return 0;
    // Полное непересечение
    if (qr <= node->B || node->E <= ql) return 0;
    // Полное покрытие
    if (ql <= node->B && node->E <= qr) return node->sum;
    // Частичное пересечение – спускаемся к детям
    return query(node->left, ql, qr) + query(node->right, ql, qr);
}

// Обновление: присвоить arr[idx] = newVal
void update(Node* node, int idx, int newVal) {
    if (node->B == node->E - 1) { // лист
        node->sum = newVal;
        return;
    }
    int mid = (node->B + node->E) / 2;
    if (idx < mid)
        update(node->left, idx, newVal);
    else
        update(node->right, idx, newVal);
    node->sum = node->left->sum + node->right->sum;
}

// Освобождение памяти (обход в глубину)
void deleteTree(Node* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

int main() {
    vector<int> arr = {1, 3, 5, 7, 9, 11, 2, 4, 6};
    int n = arr.size();

    // Построение дерева на полуинтервале [0, n)
    Node* root = buildTree(arr, 0, n);

    cout << "Сумма на [0,9): " << query(root, 0, n) << endl; // 48
    cout << "Сумма на [2,6): " << query(root, 2, 6) << endl; // 5+7+9+11 = 32

    update(root, 4, 100); // меняем 9 на 100
    cout << "После обновления arr[4]=100:" << endl;
    cout << "Сумма на [0,9): " << query(root, 0, n) << endl; // 139
    deleteTree(root);
    return 0;
}
```
