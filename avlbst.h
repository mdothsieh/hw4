#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child);
    void removeFix(AVLNode<Key, Value>* node, int diff);


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
    // root is empty, make new root
    if(this -> root_ == NULL){
        this -> root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }

    // go down tree like normal bst insert
    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = NULL;

    while(current != NULL){
        parent = current;
        if(new_item.first < current -> getKey()){
            current = current -> getLeft();
        }
        else if(new_item.first > current -> getKey()){
            current = current -> getRight();
        }
        else{
            // key already exists
            current -> setValue(new_item.second);
            return;
        }
    }

    // attach new node to parent
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);
    if(new_item.first < parent -> getKey()){
        parent -> setLeft(newNode);
    }
    else{
        parent -> setRight(newNode);
    }

    insertFix(parent, newNode); // fix balance and rotaet if needed starting from parent
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    // TODO
    // find target node
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key,Value>*>(this -> internalFind(key)); 
    if(node == NULL){
        return; // key not found
    }

    //if node has 2 children, swap with predecessor
    if(node->getLeft() != NULL && node->getRight() != NULL) {
        Node<Key, Value>* predNode =
            BinarySearchTree<Key, Value>::predecessor(node);
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(predNode);
        if(pred != NULL) {
            nodeSwap(node, pred);
        }
    }

    //node has at most one child

    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* child = NULL;

    if(node->getLeft() != NULL) {
        child = node->getLeft();
    }
    else if(node->getRight() != NULL) {
        child = node->getRight();
    }

    if(child != NULL) {
        child->setParent(parent);
    }

    int diff = 0; // tells removeFix which side lost a node

    if(parent == NULL) {
        // We are deleting the root
        this->root_ = child;
    }
    else {
        if(parent->getLeft() == node) {
            parent->setLeft(child);
            // left subtree shurnk and balance = balance -1
            diff = -1;
        }
        else {
            parent->setRight(child);
            // right subtree shrunk and balance = balance +1
            diff = 1;
        }
    }

    delete node;

    // Rebalance going up from parent
    if(parent != NULL) {
        removeFix(parent, diff);
    }
}

// helper function = rotate left
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node){
    if(node == NULL){ // nothing to rotate
        return;
    }
    AVLNode<Key, Value>* rightChild = node->getRight(); // left rotation requires right child
    if(rightChild == NULL){ // cannot rotate left with no right child
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent(); // save parent to reconnect after rotation

    //1. move rightChild's left subtree to be node's right subtree
    node->setRight(rightChild->getLeft());
    if(rightChild->getLeft() != NULL) {
        rightChild->getLeft()->setParent(node);
    }

    //2.rotate
    rightChild->setLeft(node);
    node->setParent(rightChild);

    //3. fix connectino to parent
    rightChild->setParent(parent);
    //if node was root rightchild becomes new root
    if(parent == NULL){
        this->root_ = rightChild;
    }
    else{
        if(parent->getLeft() == node){ // reconnected correct child and parent
            parent->setLeft(rightChild);
        }
        else{
            parent->setRight(rightChild);
        }
    }
}

//helper function = rotate right
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node){
    if(node == NULL){//nothing to rate
        return;
    }
    AVLNode<Key, Value>* leftChild = node->getLeft(); // right rotation requires left child
    if(leftChild == NULL){ // cannot rotate right with no left child
        return;
    }

    AVLNode<Key, Value>* parent = node->getParent(); // save parent to reconnect after rotation

    //1. move leftChild's right subtree to be node's left subtree
    node->setLeft(leftChild->getRight());
    if(leftChild->getRight() != NULL) {
        leftChild->getRight()->setParent(node);
    }

    //2. rotate
    leftChild->setRight(node);
    node->setParent(leftChild);

    //3. fix connection to parent
    leftChild->setParent(parent);
    //if node was root leftchild becomes new root
    if(parent == NULL){
        this->root_ = leftChild;
    }
    else{
        if(parent->getLeft() == node){ // reconnected correct child and parent
            parent->setLeft(leftChild);
        }
        else{
            parent->setRight(leftChild);
        }
    }

}

// helper function = insert fix
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* child){
    if(parent == NULL){
        return; // reached root no need to fix
    }

    //1. update balance of parent 
    if(child == parent->getLeft()) {
        // new node is left child, so update balance +1
        parent->updateBalance(1);
    }
    else {
        // new node is right child, so update balance -1
        parent->updateBalance(-1);
    }
    int8_t parentBalance = parent->getBalance();

    //2. check balance of parent
    if(parentBalance == 0){
        return; // tree is balanced, stop
    }

    //3. if parent balance is +-1, continue up the tree
    if(parentBalance == 1 || parentBalance == -1){
        AVLNode<Key, Value>* grandparent = parent->getParent();
        if(grandparent != NULL){
            insertFix(grandparent, parent);
        }
        return;
    }

    //4. parent balance is +-2, need to rotate

    // left heavy case
    if(parentBalance == 2){ // left heavy
        AVLNode<Key, Value>* leftChild = parent->getLeft();
        if(leftChild == NULL){
            return; 
        }

        // LL case
        if(leftChild->getBalance() >= 0) {
            rotateRight(parent);
            // after LL rotation both nodes become balanced
            parent->setBalance(0);
            leftChild->setBalance(0);
        }
        else { //LR case
            AVLNode<Key, Value>* grandChild = leftChild->getRight();

            // grandchild balance before rotation
            int8_t gcBalance = 0;
            if(grandChild != NULL) {
                gcBalance = grandChild->getBalance();
            }

            // two rotations: left on child, right on parent
            rotateLeft(leftChild);
            rotateRight(parent);

            // update balances after LR rotation
            if(gcBalance == 1) {
                parent->setBalance(-1);
                leftChild->setBalance(0);
            }
            else if(gcBalance == -1) {
                parent->setBalance(0);
                leftChild->setBalance(1);
            }
            else {
                // both nodes balanced
                parent->setBalance(0);
                leftChild->setBalance(0);
            }

            // set grandchild balance to 0
            if(grandChild != NULL) {
                grandChild->setBalance(0);
            }
        }
    }

    // right heavy case
    else if(parentBalance == -2){ // right heavy
        AVLNode<Key, Value>* rightChild = parent->getRight();
        if(rightChild == NULL){
            return;
        }

        // RR case
        if(rightChild->getBalance() <= 0){
            rotateLeft(parent);
            // after RR rotation both nodes become balanced
            parent->setBalance(0);
            rightChild->setBalance(0);
        }
        else { // RL case
            AVLNode<Key, Value>* grandChild = rightChild->getLeft();

            // grandchild balance before rotation
            int8_t gcBalance = 0;
            if(grandChild != NULL){
                gcBalance = grandChild->getBalance();
            }

            // two rotations: right on child, left on parent
            rotateRight(rightChild);
            rotateLeft(parent);

            // update balances after RL rotation
            if(gcBalance == -1){
                parent->setBalance(1);
                rightChild->setBalance(0);
            }
            else if(gcBalance == 1){
                parent->setBalance(0);
                rightChild->setBalance(-1);
            }
            else{
                // both nodes balanced
                parent->setBalance(0);
                rightChild->setBalance(0);
            }

            // set grandchild balance to 0
            if(grandChild != NULL){
                grandChild->setBalance(0);
            }
        }
    }
}



// helper function = remove fix
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff){
    //diff = -1 left subtree shorter
    //diff = 1 right subtree shorter
    if (node == NULL){
        return; // reached root no need to fix
    }   

    // walk up tree updating balances
    while (node != NULL){

        //1. update balance
        node->updateBalance(diff);
        int8_t nodeBalance = node->getBalance();

        //2. if left heavy 
        if(nodeBalance == 2){
            AVLNode<Key, Value>* leftChild = node->getLeft();
            int8_t leftBalance = 0;
            if(leftChild != NULL){
                leftBalance = leftChild->getBalance();
            }

            // LL case
            if(leftBalance >= 0){
                rotateRight(node);

                if(leftBalance == 0){
                    // height doesn't change 
                    node->setBalance(-1);
                    leftChild->setBalance(1);
                    break;
                }
                else{
                    // fully fixed, move up the tree
                    node->setBalance(0);
                    leftChild->setBalance(0);
                }
            }

            //LR case
            else{
                AVLNode<Key, Value>* grandChild = leftChild->getRight();
                int8_t gcBalance = 0;
                if(grandChild != NULL){
                    gcBalance = grandChild->getBalance();
                }

                // two rotations: left on child, right on node
                rotateLeft(leftChild);
                rotateRight(node);

                // update balances after LR rotation
                if(gcBalance == 1){
                    node->setBalance(-1);
                    leftChild->setBalance(0);
                }
                else if(gcBalance == -1){
                    node->setBalance(0);
                    leftChild->setBalance(1);
                }
                else{
                    node->setBalance(0);
                    leftChild->setBalance(0);
                }

                if(grandChild != NULL){
                    grandChild->setBalance(0);
                }
            }
        }

        //3. if right heavy
        else if(nodeBalance == -2){
            AVLNode<Key, Value>* rightChild = node->getRight();
            int8_t rightBalance = 0;
            if(rightChild != NULL){
                rightBalance = rightChild->getBalance();
            }

            // RR case
            if(rightBalance <= 0){
                rotateLeft(node);

                if(rightBalance == 0){
                    // height doesn't change
                    node->setBalance(1);
                    rightChild->setBalance(-1);
                    break;
                }
                else{
                    // fully fixed, move up the tree
                    node->setBalance(0);
                    rightChild->setBalance(0);
                }
            }

            //RL case
            else{
                AVLNode<Key, Value>* grandChild = rightChild->getLeft();
                int8_t gcBalance = 0;
                if(grandChild != NULL){
                    gcBalance = grandChild->getBalance();
                }

                // double rotation: right on child, left on node
                rotateRight(rightChild);
                rotateLeft(node);

                // adjust balances depending on grandChild's old balance
                if(gcBalance == -1){
                    node->setBalance(1);
                    rightChild->setBalance(0);
                }
                else if(gcBalance == 1){
                    node->setBalance(0);
                    rightChild->setBalance(-1);
                }
                else{
                    node->setBalance(0);
                    rightChild->setBalance(0);
                }

                if(grandChild != NULL){
                    grandChild->setBalance(0);
                }
            }
        }

        // if balance = 0, height shrank, continue up the tree
        else if(nodeBalance == 0){
            // keep going upward
        }

        // if balance = ±1 → height did not change → stop
        else{
            break; 
        }

        // move up to parent 
        AVLNode<Key, Value>* parent = node->getParent();
        if(parent != NULL){
            if(parent->getLeft() == node){
                diff = -1;  // left subtree shrank
            }
            else{
                diff = 1;   // right subtree shrank
            }
        } 

        node = parent; // move up the tree
    } 
}





template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
