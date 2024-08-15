#pragma once

template <typename T>
class node {
public:
    T data;
    node* next;

    node(T value) : data(value), next(nullptr) {}
};

template <typename T>
class linked_list {
private:
    node<T>* head;

public:
    linked_list() : head(nullptr) {}

    // ����� ��� ���������� �������� � ����� ������
    void append(T value) {
        node<T>* new_node = new node<T>(value);
        if (!head) {
            head = new_node;
        }
        else {
            node<T>* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = new_node;
        }
    }

    // ����� ��� �������� �������� �� ��������
    void remove(T value) {
        if (!head) return;

        // �������� ������ ������
        if (head->data == value) {
            node<T>* temp = head;
            head = head->next;
            delete temp;
            return;
        }

        node<T>* current = head;
        while (current->next && current->next->data != value) {
            current = current->next;
        }

        // ���� ������� ������, ������� ���
        if (current->next) {
            node<T>* temp = current->next;
            current->next = current->next->next;
            delete temp;
        }
    }

    // ���������� ��� ������������ ������
    ~linked_list() {
        while (head) {
            node<T>* temp = head;
            head = head->next;
            delete temp;
        }
    }
};