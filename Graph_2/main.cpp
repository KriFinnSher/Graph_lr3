// Задача 1. Дано N-дерево. Удалить самые далекие от корня листья в дереве.

#include<iostream>
#include<chrono>
#include<vector>
#include<ctime>
#include<fstream>
#include<string>
#include<sstream>


struct Node {
    int depth, val;
    Node* parent;
    std::vector<Node*> childs;
    Node(int value, int arity) : depth(0), val(value), childs(arity, nullptr), parent(nullptr) {}
};

void add_node(int val, Node*& parent, int arity) {
    int rand_iter = rand() % arity;
    if (parent->childs[rand_iter] != nullptr)
        add_node(val, parent->childs[rand_iter], arity);
    else {
        Node* node = new Node(val, arity);
        node->parent = parent;
        parent->childs[rand_iter] = node;
    }
}

int tree_height(Node*& root) {
    if (root == nullptr)
        return -1;
    int height = -1;

    for (Node* child : root->childs)
        height = std::max(height, tree_height(child));
    return height + 1;
}

int depth_of_node(Node*& node) {
    if (node->parent == nullptr)
        return 0;
    return 1 + depth_of_node(node->parent);
}

void set_all_depths(Node*& root) {
    if (root == nullptr)
        return;
    root->depth = depth_of_node(root);

    for (int i = 0; i < root->childs.size(); i++)
        set_all_depths(root->childs[i]);
}

void delete_distant_leafs(Node*& root, int max_height) {
    if (root == nullptr)
        return;
    for (int i = 0; i < root->childs.size(); i++) {
        Node* child = root->childs[i];
        if (!child)
            continue;
        if (child->depth == max_height) {
            delete child;
            for (int j = i; j < root->childs.size() - 1; j++)
                root->childs[j] = root->childs[j + 1];
            root->childs.pop_back();
            i--;
        }
    }
    for (int i = 0; i < root->childs.size(); i++)
        delete_distant_leafs(root->childs[i], max_height);
}

void print_tree(Node* root, int current_depth = 0) {
    if (root == nullptr)
        return;
    std::string indent;
    for (int i = 0; i < current_depth; ++i)
        indent += "  \\";
    std::cout << indent + "__" << root->val << std::endl;
    for (Node* child : root->childs)
        print_tree(child, current_depth + 1);
}

void clear_tree(Node*& root) {
    if (root == nullptr)
        return;
    for (Node* child : root->childs)
        clear_tree(child);
    delete root;
}

Node* create_custom_tree(std::ifstream& tree_file, int arity) {
    std::vector<int> childs;
    std::string node_line;
    std::getline(tree_file, node_line);
    std::istringstream temp_node(node_line);

    int val = 0, flag = 0;
    temp_node >> val;
    while (temp_node >> flag)
        childs.push_back(flag);
    Node* root = new Node(val, arity);
    for (int i = 0; i < childs.size(); i++) {
        if (childs[i] == 1)
            root->childs[i] = create_custom_tree(tree_file, arity);
    }
    return root;
}

void assign_parent_pointers(Node* root) {
    if (!root)
        return;
    for (Node* child : root->childs) {
        if (child) {
            child->parent = root; // Устанавливаем указатель на родителя для каждого потомка
            assign_parent_pointers(child); // Рекурсивно вызываем функцию для каждого потомка
        }
    }
}

void generate_file(const std::string& filename, int count) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Невозможно открыть файл " << filename << std::endl;
        return;
    }
    int rand_num = 0;
    for (int i = 0; i < count; ++i) {
        rand_num = rand() % 10;
        file << rand_num << std::endl;
    }
    file.close();
}

int main() {
    srand(time(0));
    setlocale(LC_ALL, "ru");

    std::cout << "\t\t\t\t----------- Информация по программе -----------\n\n"
        << "Для работы с деревом выберете способ его создания:\n\n"
        << "1 - произвольное построение по заданному кол-ву узлов\n"
        << "2 - построение по заданному шаблону с конкретными вершинами\n";

    int choice = 0;
    std::cin >> choice;
    std::string file_name;
    int counts_of_nodes = 0, arity = 0;

    if (choice == 1) {
        std::cout << "Укажите кол-во создаваемых узлов: ";
        std::cin >> counts_of_nodes;
        std::cout << "Укажите арность дерева: ";
        std::cin >> arity;

        Node* root = new Node(rand() % 10, arity);
        file_name = "num" + std::to_string(counts_of_nodes) + ".txt";
        generate_file(file_name, counts_of_nodes);

        std::ifstream in_file(file_name);
        int num = 0;
        while (in_file >> num)
            add_node(num, root, arity);
        int root_height = tree_height(root);
        set_all_depths(root);
        std::cout << "Исходное дерево:" << std::endl;
        print_tree(root);
        std::cout << "\n\nПосле удаления самых далеких листов:" << std::endl;
        auto start1 = std::chrono::high_resolution_clock::now();
        delete_distant_leafs(root, root_height);
        auto end1 = std::chrono::high_resolution_clock::now();
        auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end1 - start1).count();
        print_tree(root);
        clear_tree(root);
        std::cout << "\nВремя работы алгоритма: " << duration1 << " мкс.\n";
    }
    else if (choice == 2) {
        std::cout << "Укажите арность дерева: ";
        std::cin >> arity;
        std::cout << "Введите каждый узел на новой строке в формате val f1 f2 ... fn, где\n"
            << "val - значение узла, fi = 1, если потомок\nесть и 0 в противном случае\n\n"
            << "*создание дерева осуществляется в прямом порядке*"
            << std::endl;


        std::string node_line;
        std::vector<std::string> nodes;
        std::ofstream custom("custom_1.txt");
        std::cin.get();

        while (std::getline(std::cin, node_line)) {
            nodes.push_back(node_line);
            if (node_line == "-1")
                break;
        }
        nodes.pop_back();
        for (const std::string& node : nodes) {
            custom << node << std::endl;
        }
        std::ifstream custom1("custom_1.txt");
        Node* root = create_custom_tree(custom1, arity);
        assign_parent_pointers(root);
        int root_height = tree_height(root);
        set_all_depths(root);
        std::cout << "Исходное дерево:" << std::endl;
        print_tree(root);
        std::cout << "\n\nПосле удаления самых далеких листов:" << std::endl;
        auto start2 = std::chrono::high_resolution_clock::now();
        delete_distant_leafs(root, root_height);
        auto end2 = std::chrono::high_resolution_clock::now();
        auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2).count();
        print_tree(root);
        clear_tree(root);
        std::cout << "\nВремя работы алгоритма: " << duration2 << " мкс.\n";
    }
    else {
        std::cout << "Некорректный ввод, программа завершена";
        return -1;
    }
    return 0;
}
