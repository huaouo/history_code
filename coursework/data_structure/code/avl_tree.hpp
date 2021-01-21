#ifndef AVLTREE_H
#define AVLTREE_H

#include "node.hpp"
#include "operation_state.h"

class TreeViewer;  //前置声明
class MainWidget;

namespace huaouo {

    template <typename K, typename V>
    class AVLTree {
        friend class ::TreeViewer;  //这两个类在全局命名空间
        friend class ::MainWidget;

    public:
        OperationState insert(const K& key, const V& val);
        OperationState remove(const K& key);
        OperationState update(const K& key, const V& val);
        const V* find(const K& key);
        ~AVLTree();

    private:
        void leftRotate(Node<K, V>*& root);
        void rightRotate(Node<K, V>*& root);
        void checkAndRotate(Node<K, V>*& root);  //检查某个节点平衡因子，并调用旋转函数
        Node<K, V>* getSuccessor(Node<K, V>*& root);  //返回某个节点的后继，同时删除后继节点
        const V* find(Node<K, V>* const& root, const K& key);  //真正执行的递归版本，下同
        OperationState remove(Node<K, V>*& root, const K& key);
        OperationState insert(Node<K, V>*& root, const K& key, const V& val);
        void postorderRecursiveDelete(Node<K, V>* root);  //析构树时使用，后序递归删除所有节点

        Node<K, V>* root = nullptr;  //树根
    };

    template <typename K, typename V>
    void AVLTree<K, V>::leftRotate(Node<K, V>*& root)
    {
        Node<K, V>* previousRoot = root;
        Node<K, V>* tmp = root->right->left;
        root->right->left = root;
        root = root->right;
        root->left->right = tmp;
        previousRoot->setHeight();  //每次旋转之后重新计算节点高度等，下同
        previousRoot->setSize();
        root->setHeight();
        root->setSize();
    }

    template <typename K, typename V>
    void AVLTree<K, V>::rightRotate(Node<K, V>*& root)
    {
        Node<K, V>* previousRoot = root;
        Node<K, V>* tmp = root->left->right;
        root->left->right = root;
        root = root->left;
        root->right->left = tmp;
        previousRoot->setHeight();
        previousRoot->setSize();
        root->setHeight();
        root->setSize();
    }

    template <typename K, typename V>
    void AVLTree<K, V>::checkAndRotate(Node<K, V>*& root)
    {
        int leftHeight = root->left == nullptr ? 0 : root->left->height;
        int rightHeight = root->right == nullptr ? 0 : root->right->height;
        int balanceFactor = leftHeight - rightHeight;
        switch (balanceFactor) {
            case 2: {
                int tmpLeftHeight = root->left->left == nullptr ? 0 : root->left->left->height;
                int tmpRightHeight = root->left->right == nullptr ? 0 : root->left->right->height;
                if (tmpLeftHeight < tmpRightHeight) { // LR型
                    leftRotate(root->left);
                }
                rightRotate(root); // LL型
                break;
            }
            case -2: {
                int tmpLeftHeight = root->right->left == nullptr ? 0 : root->right->left->height;
                int tmpRightHeight = root->right->right == nullptr ? 0 : root->right->right->height;
                if (tmpRightHeight < tmpLeftHeight) { // RL型
                    rightRotate(root->right);
                }
                leftRotate(root);  // RR型
            }
        }

        root->setHeight();
    }

    template <typename K, typename V>
    OperationState AVLTree<K, V>::insert(Node<K, V>*& root, const K& key, const V& val)
    {
        if (root == nullptr) {
            root = new Node<K, V>(key, val);
        } else if ((root->key > key && insert(root->left, key, val) == FAILURE)
                   || (root->key < key && insert(root->right, key, val) == FAILURE)
                   || (root-> key == key)) {
            return FAILURE;
        }

        root->setSize();
        root->setHeight();
        checkAndRotate(root);  //调平，下同

        return SUCCESS;
    }

    template <typename K, typename V>
    OperationState AVLTree<K, V>::insert(const K& key, const V& val)
    {
        return insert(root, key, val);
    }

    template <typename K, typename V>
    Node<K, V>* AVLTree<K, V>::getSuccessor(Node<K, V>*& root)
    {
        Node<K, V>* tmp = root;
        if(root->left == nullptr) {
            root = tmp->right;
            tmp->right = nullptr;
        } else {
            tmp = getSuccessor(root->left);
        }

        if(root != nullptr) {
            root->setHeight();
            root->setSize();
        }

        return tmp;
    }

    template <typename K, typename V>
    OperationState AVLTree<K, V>::remove(Node<K, V>*& root, const K& key)
    {
        if (root == nullptr) {
            return FAILURE;
        }

        if ((root->key > key && remove(root->left, key) == FAILURE)
            || (root->key < key && remove(root->right, key) == FAILURE)) {
            return FAILURE;
        } else if(root->key == key){
            if (root->left == nullptr && root->right == nullptr) {  //叶子
                delete root;
                root = nullptr;
            } else if (root->left == nullptr) {  //只有右子树
                Node<K, V>* tmp = root->right;
                delete root;
                root = tmp;
            } else if (root->right == nullptr) {  //只有左子树
                Node<K, V>* tmp = root->left;
                delete root;
                root = tmp;
            } else {  //有左、右子树
                Node<K, V>* successor = getSuccessor(root->right);
                Node<K, V>* leftBackup = root->left;
                Node<K, V>* rightBackup = root->right;

                delete root;
                root = successor;
                root->left = leftBackup;
                root->right = rightBackup;
            }
        }

        if(root != nullptr) {
            root->setSize();
            root->setHeight();
            checkAndRotate(root);
        }

        return SUCCESS;
    }

    template <typename K, typename V>
    OperationState AVLTree<K, V>::remove(const K& key)
    {
        return remove(root, key);
    }

    template <typename K, typename V>
    const V* AVLTree<K, V>::find(Node<K, V>* const& root, const K& key)
    {
        if (root == nullptr) {
            return nullptr;
        }

        if (root->key == key) {
            return &(root->val);  //返回了临时变量，返回值不能为引用
        } else if (root->key > key) {
            return find(root->left, key);
        } else {
            return find(root->right, key);
        }
    }

    template <typename K, typename V>
    const V* AVLTree<K, V>::find(const K& key)
    {
        return find(root, key);
    }

    template <typename K, typename V>
    OperationState AVLTree<K, V>::update(const K& key, const V& val)
    {
        auto result = const_cast<V*>(find(root, key));
        if (result == nullptr) {
            return FAILURE;
        } else {
            *result = val;
            return SUCCESS;
        }
    }

    template <typename K, typename V>
    void AVLTree<K, V>::postorderRecursiveDelete(Node<K, V>* root)
    {
        if (root->left != nullptr) {
            postorderRecursiveDelete(root->left);
        }
        if (root->right != nullptr) {
            postorderRecursiveDelete(root->right);
        }
        delete root;
    }

    template <typename K, typename V>
    AVLTree<K, V>::~AVLTree()
    {
        if(root != nullptr) {
            postorderRecursiveDelete(root);
        }
    }

} // namespace huaouo

#endif // AVLTREE_H
