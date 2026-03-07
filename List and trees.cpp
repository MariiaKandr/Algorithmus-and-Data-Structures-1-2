// ПР4: переход от связных списков к двоичным деревьям на C++
// Рассмотрим, как из отсортированного односвязного списка
// построить сбалансированное двоичное дерево поиска (BST),
// а также основные операции с деревом: обходы, вставку, поиск.

#include <iostream>
#include <vector>

using namespace std;

// --- Структура узла односвязного списка (повторение) ---
struct ListNode {
    int data;
    ListNode* next;

    ListNode(int val) : data(val), next(nullptr) {}
};

// --- Структура узла двоичного дерева (новая структура) ---
// В отличие от списка, узел дерева имеет два указателя: на левое и правое поддеревья.
struct TreeNode {
    int data;
    TreeNode* left;
    TreeNode* right;

    TreeNode(int val) : data(val), left(nullptr), right(nullptr) {}
};

// --- Функции для работы со связным списком ---

// Создание списка из массива (для удобства)
ListNode* createList(const vector<int>& values) {
    ListNode* head = nullptr;
    ListNode* tail = nullptr;
    for (int v : values) {
        ListNode* newNode = new ListNode(v);
        if (!head) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    return head;
}

// Вывод списка
void printList(ListNode* head) {
    cout << "Список: ";
    while (head) {
        cout << head->data << " ";
        head = head->next;
    }
    cout << endl;
}

// --- Преобразование отсортированного списка в сбалансированное двоичное дерево поиска ---
// Алгоритм: находим середину списка (медленный и быстрый указатели),
// создаём корень из середины, рекурсивно строим левое поддерево из левой половины,
// правое — из правой половины.
// Возвращает корень дерева.
TreeNode* sortedListToBST(ListNode*& head, int start, int end) {
    // Базовый случай: если подсписок пуст
    if (start > end) return nullptr;

    // Находим середину: (start + end) / 2
    int mid = start + (end - start) / 2;

    // Рекурсивно строим левое поддерево (элементы до середины)
    TreeNode* leftChild = sortedListToBST(head, start, mid - 1);

    // Корень — текущий узел списка (голова после обработки левой половины)
    TreeNode* root = new TreeNode(head->data);
    root->left = leftChild;

    // Сдвигаем голову списка вперёд
    head = head->next;

    // Рекурсивно строим правое поддерево (элементы после середины)
    root->right = sortedListToBST(head, mid + 1, end);

    return root;
}

// Обёртка для преобразования списка в дерево
TreeNode* convertListToBST(ListNode* head) {
    // Сначала подсчитаем длину списка
    int len = 0;
    ListNode* temp = head;
    while (temp) {
        ++len;
        temp = temp->next;
    }
    // Вызываем рекурсивную функцию, передавая голову по ссылке
    return sortedListToBST(head, 0, len - 1);
}

// --- Функции для работы с двоичным деревом ---

// Обход дерева в прямом порядке (Корень-Лево-Право)
void preorder(TreeNode* root) {
    if (!root) return;
    cout << root->data << " ";
    preorder(root->left);
    preorder(root->right);
}

// Обход дерева в симметричном порядке (Лево-Корень-Право)
void inorder(TreeNode* root) {
    if (!root) return;
    inorder(root->left);
    cout << root->data << " ";
    inorder(root->right);
}

// Обход дерева в обратном порядке (Лево-Право-Корень)
void postorder(TreeNode* root) {
    if (!root) return;
    postorder(root->left);
    postorder(root->right);
    cout << root->data << " ";
}

// Поиск значения в двоичном дереве поиска (рекурсивно)
TreeNode* searchBST(TreeNode* root, int key) {
    if (!root || root->data == key)
        return root;
    if (key < root->data)
        return searchBST(root->left, key);
    else
        return searchBST(root->right, key);
}

// Вставка нового узла в двоичное дерево поиска
TreeNode* insertBST(TreeNode* root, int val) {
    if (!root) {
        return new TreeNode(val);
    }
    if (val < root->data) {
        root->left = insertBST(root->left, val);
    } else if (val > root->data) {
        root->right = insertBST(root->right, val);
    }
    // Если значение уже есть, ничего не делаем (можно изменить поведение)
    return root;
}

// Подсчёт высоты дерева (количество рёбер на самом длинном пути от корня до листа)
int height(TreeNode* root) {
    if (!root) return -1; // высота пустого дерева = -1 (по другой версии 0)
    int leftH = height(root->left);
    int rightH = height(root->right);
    return 1 + max(leftH, rightH);
}

// Освобождение памяти дерева (пост-обход)
void deleteTree(TreeNode* root) {
    if (!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

// --- Главная функция для демонстрации ---
int main() {
    // 1. Создаём отсортированный список (например, из массива)
    vector<int> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    ListNode* listHead = createList(arr);
    cout << "Исходный отсортированный ";
    printList(listHead);

    // 2. Преобразуем список в сбалансированное дерево поиска
    TreeNode* treeRoot = convertListToBST(listHead);
    cout << "\nДерево построено!\n";

    // 3. Выполняем обходы дерева
    cout << "Прямой обход (preorder):   ";
    preorder(treeRoot);
    cout << endl;

    cout << "Симметричный обход (inorder): ";
    inorder(treeRoot);
    cout << "  (должен совпадать с исходным списком)\n";

    cout << "Обратный обход (postorder): ";
    postorder(treeRoot);
    cout << endl;

    // 4. Высота дерева
    cout << "\nВысота дерева: " << height(treeRoot) << endl;

    // 5. Поиск элемента в дереве
    int key = 5;
    TreeNode* found = searchBST(treeRoot, key);
    if (found)
        cout << "Элемент " << key << " найден в дереве.\n";
    else
        cout << "Элемент " << key << " не найден.\n";

    key = 10;
    found = searchBST(treeRoot, key);
    if (found)
        cout << "Элемент " << key << " найден в дереве.\n";
    else
        cout << "Элемент " << key << " не найден.\n";

    // 6. Вставка нового элемента
    cout << "\nВставляем элемент 10 в дерево...\n";
    treeRoot = insertBST(treeRoot, 10);
    cout << "Симметричный обход после вставки: ";
    inorder(treeRoot);
    cout << endl;

    // 7. Очистка памяти
    deleteTree(treeRoot);
    // Список уже не нужен, так как его узлы были использованы для создания дерева?
    // Внимание: в функции convertListToBST мы потребляем узлы списка,
    // т.е. после преобразования список уничтожен (память узлов перешла в дерево).
    // Поэтому отдельно удалять список не нужно, иначе double free (ошибка повторного удаления).
    // Если бы мы хотели сохранить список, нужно было бы копировать данные,
    // но здесь мы просто перемещаем узлы.

    cout << "\nПрограмма завершена.\n";
    return 0;
}
