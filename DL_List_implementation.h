#pragma once
#include <cassert>
#include <iostream>


// declaration 
template <typename T>
class List_I;

// namespace for node class, just to hide it 
namespace node_space
{
    // Node class 
    template<typename T>
    class Node
    {
    public:
        T value;
        Node* next;
        Node* prev;
    public:
        Node() : next(nullptr), prev(nullptr) {}
        Node(const T& value) : value(value), next(nullptr), prev(nullptr) {}
        Node(const T& value, Node* prev) : value(value), next(nullptr), prev(prev) {}

        ~Node() {}
    };
    template<typename T>
    inline std::ostream& operator<<(std::ostream& out, const Node<T>& node)
    {
        out << node.value;
        return out;
    }
}

// Basic Iterator namespace 
namespace iter
{
    // basic iterator 
    template<typename List_I>
    class Iterator
    {
    public:
        using ValueType = typename List_I::ValueType;
        using PointerType = node_space::Node<ValueType>*;
        using ReferenceType = node_space::Node<ValueType>&;
    public:
        Iterator(PointerType ptr)
            : ptr(ptr) {}

        PointerType operator->()
        {
            return ptr;
        }
        ReferenceType operator*()
        {
            return *(ptr);
        }

        bool operator==(const Iterator& rhs) const
        {
            return (ptr == rhs.ptr);
        }
        bool operator!=(const Iterator& rhs) const
        {
            return !(*this == rhs);
        }

    public:
        PointerType ptr = nullptr;
    };
}

template<typename List_I>
class Forward_Iterator : public iter::Iterator<List_I>
{
public:
    using ValueType = typename List_I::ValueType;
    using PointerType = node_space::Node<ValueType>*;
    using ReferenceType = node_space::Node<ValueType>&;
public:
    Forward_Iterator(PointerType ptr)
        : iter::Iterator<List_I>(ptr), ptr(ptr) {}

    Forward_Iterator& operator++()
    {
        *this = Forward_Iterator(ptr->next);
        return *this;
    }
    Forward_Iterator& operator++(int)
    {
        Forward_Iterator it = *this;
        *this = Forward_Iterator(ptr->next);
        return it;
    }
    Forward_Iterator& operator--()
    {
        *this = Forward_Iterator(ptr->prev);
        return *this;
    }
    Forward_Iterator& operator--(int)
    {
        Forward_Iterator it = *this;
        *this = Forward_Iterator(ptr->prev);
        return it;
    }

    // WARNIN! IT'S JUST FOR DUBUGG, MAY CAUSE A BUG!
    Forward_Iterator operator+(const size_t num)
    {
        if (ptr->next == nullptr)
            assert(false);

        size_t temp_val = num;
        while (temp_val != 0 && ptr != nullptr)
        {
            ptr = ptr->next;
            --temp_val;
        }

        *this = Forward_Iterator(ptr);
        return *this;
    }

    // WARNIN! IT'S JUST FOR DUBUGG, MAY CAUSE A BUG!
    Forward_Iterator operator-(const size_t num)
    {
        if (ptr->prev == nullptr)
            assert(false);

        size_t temp_val = num;
        while (temp_val != 0 && ptr != nullptr)
        {
            ptr = ptr->prev;
            --temp_val;
        }
        if (ptr == nullptr)
            assert(false);  // for debugg

        *this = Forward_Iterator(ptr);

        return *this;
    }

    Forward_Iterator operator=(iter::Iterator<List_I>& rhs)
    {
        this->ptr = rhs.ptr;
        return Forward_Iterator(rhs.ptr);
    }
public:
    PointerType ptr;
};

template<typename List_I>
class Reversed_Iterator : public iter::Iterator<List_I>
{
public:
    using ValueType = typename List_I::ValueType;
    using PointerType = node_space::Node<ValueType>*;
    using ReferenceType = node_space::Node<ValueType>&;
public:
    Reversed_Iterator(PointerType ptr)
        : iter::Iterator<List_I>(ptr), ptr(ptr) {}

    Reversed_Iterator operator++()
    {
        *this = Reversed_Iterator(ptr->prev);
        return *this;
    }
    Reversed_Iterator& operator++(int)
    {
        Reversed_Iterator it = *this;
        *this = Reversed_Iterator(ptr->prev);
        return it;
    }
    Reversed_Iterator& operator--()
    {
        *this = Reversed_Iterator(ptr->next);
        return *this;
    }
    Reversed_Iterator& operator--(int)
    {
        Reversed_Iterator it = *this;
        *this = Reversed_Iterator(ptr->next);
        return it;
    }

    // WARNIN! IT'S JUST FOR DUBUGG, MAY CAUSE A BUG!
    Reversed_Iterator operator+(const size_t num)
    {
        if (ptr->prev == nullptr)
            assert(false);
        size_t temp_val = num;
        while (temp_val != 0 && ptr != nullptr)
        {
            ptr = ptr->prev;
            --temp_val;
        }
        *this = Reversed_Iterator(ptr);

        return *this;
    }
    // WARNIN! IT'S JUST FOR DUBUGG, MAY CAUSE A BUG!
    Reversed_Iterator operator-(const size_t num)
    {
        if (ptr->next == nullptr)
            assert(false);
        size_t temp_val = num;
        while (temp_val != 0 && ptr != nullptr)
        {
            ptr = ptr->next;
            --temp_val;
        }
        *this = Reversed_Iterator(ptr);
        return *this;
    }

    Reversed_Iterator operator=(const iter::Iterator<List_I>& rhs)
    {
        this->ptr = rhs.ptr;
        return Reversed_Iterator(rhs.ptr);
    }
public:
    PointerType ptr;
};





// Doubly-linked List class 
template<typename T>
class List_I
{
    // iterators
public:
    using ValueType = T;   // data type 
    using Iterator = Forward_Iterator<List_I<T>>;  // forward iterator 
    using Reverse_Iterator = Reversed_Iterator<List_I<T>>;   // reverse iterator 

    // get iterators
    constexpr Iterator begin() const noexcept { return Iterator(head); }
    constexpr Iterator end() const noexcept { return Iterator(nullptr); }
    constexpr Reverse_Iterator rbegin() const noexcept { return Reverse_Iterator(tail); }
    constexpr Reverse_Iterator rend() const noexcept { return Reverse_Iterator(nullptr); }
    /////////////

public:
    List_I() {}
    List_I(List_I&& rhs) noexcept
    {
        this->head = rhs.head;
        this->tail = rhs.tail;
        rhs.head = nullptr;
        rhs.tail = nullptr;
    }  // move constructor

    List_I(const List_I& rhs) noexcept
    {
        this->head = rhs.head;
        this->tail = rhs.tail;
    }  // copy constructor 
    ~List_I()
    {
        this->clear();
        delete head, tail;
    }

    // basic functions 
    void push_back(const T& val)
    {

        if (tail == nullptr)  // if list's empty
        {
            node_space::Node<T>* new_node =
                new node_space::Node(val);   // Set value, and both, prev and next to nullptr

            head = tail = new_node;   // set both head and tail to this new node

            return;
        }

        node_space::Node<T>* new_node =
            new node_space::Node(val, tail);  // set value, prev pointer to tail and next to null

        tail->next = new_node;
        tail = new_node;
    }

    bool is_empty()
    {
        if (tail == nullptr)  // if tail points to nullptr 
            return true;  // the list is empty
        else   // othervise 
            return false;  // the list is not empty
    }
    size_t size()
    {
        size_t result = 0;

        node_space::Node<T>* temp = head;
        while (temp != nullptr)
        {
            ++result;
            temp = temp->next;
        }
        return result;
    }
    void clear()
    {
        while (head != nullptr)
        {
            node_space::Node<T>* temp = head;
            head = head->next;
            //temp->~Node();
            delete temp;
        }
        head = tail = nullptr;
    }

    constexpr T& back() const noexcept
    {
        if (tail == nullptr)
            assert(false);
        return tail->value;
    }
    constexpr T& front() const noexcept
    {
        if (head == nullptr)
            assert(false);
        return head->value;
    }


    template<typename Iter>
    Iterator insert(Iter&& position, const T& val)
    {
        if (position == nullptr)
            return nullptr;
        if (position == tail)
            return this->insert_back(val);
        else if (position == head)
            return this->insert_front(val);
        node_space::Node<T>* new_node = new node_space::Node<T>(val);

        new_node->next = position.ptr->next;
        new_node->prev = position.ptr;
        position.ptr->next->prev = new_node;
        position.ptr->next = new_node;

        return Iterator(new_node);
    }
    Iterator insert_back(const T& val)
    {
        node_space::Node<T>* new_node = new node_space::Node<T>(val);
        new_node->prev = tail;
        tail->next = new_node;
        tail = new_node;

        return Iterator(tail);
    }
    Iterator insert_front(const T& val)
    {
        node_space::Node<T>* new_node = new node_space::Node<T>(val);
        new_node->next = head;
        head->prev = new_node;
        head = new_node;

        return Iterator(head);
    }


    template<typename Iter, typename... Args>
    Iterator emplace(const Iter& position, Args&&... args)
    {
        if (position == head)
            return Iterator(this->emplace_front(args...));
        else if (position == tail)
            return Iterator(this->emplace_back(args...));

        // allocate memmory and set value
        node_space::Node<T>* new_node = new node_space::Node<T>();
        new(&new_node->value) T(std::forward<T>(args)...);

        // example. insert to after 2nd node 
        // list: [val] - [val] - [val]
        // it      1       2       3
        //       [val] - [val] - [val] [new_node_val]
        //       [val] - [val] - [new_node_val] - [val]

        new_node->prev = position.ptr;
        new_node->next = position.ptr->next;
        new_node->next->prev = new_node;
        position.ptr->next = new_node;

        return Iterator(new_node);
    }
    template<typename... Args>
    Iterator emplace_front(Args&&... args)
    {
        node_space::Node<T>* new_node = new node_space::Node<T>();
        new(&new_node->value) T(std::forward<Args>(args)...);

        if (tail != nullptr)
        {
            new_node->next = head;
            head->prev = new_node;
            head = new_node;
        }
        else
            head = tail = new_node;

        return new_node;
    }
    template<typename... Args>
    Iterator emplace_back(Args&&... args)
    {
        node_space::Node<T>* new_node = new node_space::Node<T>();
        new(&new_node->value) T(std::forward<Args>(args)...);

        if (tail != nullptr)
        {
            new_node->prev = tail;
            tail->next = new_node;
            tail = new_node;
        }
        else
            head = tail = new_node;

        return new_node;
    }

    Iterator erase(Iterator& position)
    {
        if (position == nullptr)
            return nullptr;
        else if (position == tail)  // pop_back
        {
            this->pop_back();
            return tail;
        }
        else if (position == head) // pop_front
        {
            this->pop_front();
            return head;
        }


        position.ptr->prev->next = position.ptr->next;
        position.ptr->next->prev = position.ptr->prev;

        delete position.ptr;

        return head;
    }
    Iterator pop_front()
    {
        node_space::Node<T>* temp = head;
        head = head->next;
        head->prev = nullptr;

        delete temp;
        return head;
    }
    Iterator pop_back()
    {
        node_space::Node<T>* temp = tail;
        tail = tail->prev;
        tail->next = nullptr;

        delete temp;
        return tail;
    }


    ////////////
private:
    node_space::Node<T>* head = nullptr;   // first 
    node_space::Node<T>* tail = nullptr;   // last
};
