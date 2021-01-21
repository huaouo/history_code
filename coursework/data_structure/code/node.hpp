#ifndef NODE_H
#define NODE_H

#include <algorithm> // std::max

namespace huaouo {

    template <typename K, typename V>
    class Node {
    public:
        Node(K key, V val);
        void setHeight();  //重置高度
        void setSize();  //重置包含本节点的子树节点数之和

        K key;
        V val;
        int height = 0;
        int size = 0;
        Node<K, V>* left = nullptr;
        Node<K, V>* right = nullptr;
    };

    template <typename K, typename V>
    Node<K, V>::Node(K key, V val)
    {
        this->key = key;
        this->val = val;
    }

    template <typename K, typename V>
    void Node<K, V>::setHeight()
    {
        height = 1 + std::max(left == nullptr ? 0 : left->height,
                              right == nullptr ? 0 : right->height);
    }

    template <typename K, typename V>
    void Node<K, V>::setSize()
    {
        size = 1 + (left == nullptr ? 0 : left->size)
                                         + (right == nullptr ? 0 : right->size);
    }

} // namespace huaouo

#endif // NODE_H
