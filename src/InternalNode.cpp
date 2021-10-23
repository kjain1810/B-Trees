#include "InternalNode.hpp"
#include "LeafNode.hpp"
#include "RecordPtr.hpp"
#include <map>

//creates internal node pointed to by tree_ptr or creates a new one
InternalNode::InternalNode(const TreePtr &tree_ptr) : TreeNode(INTERNAL, tree_ptr) {
    this->keys.clear();
    this->tree_pointers.clear();
    if(!is_null(tree_ptr))
        this->load();
}

//max element from tree rooted at this node
Key InternalNode::max() {
    Key max_key = DELETE_MARKER;
    TreeNode* last_tree_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    max_key = last_tree_node->max();
    delete last_tree_node;
    return max_key;
}

//if internal node contains a single child, it is returned
TreePtr InternalNode::single_child_ptr() {
    if(this->size == 1)
        return this->tree_pointers[0];
    return NULL_PTR;
}

//inserts <key, record_ptr> into subtree rooted at this node.
//returns pointer to split node if exists
//TODO: InternalNode::insert_key to be implemented
TreePtr InternalNode::insert_key(const Key &key, const RecordPtr &record_ptr) {
    TreePtr new_tree_ptr = NULL_PTR;
    TreePtr child_ptr; // the child pointer where the key has to go
    int child_idx;
    // Finding the child pointer
    child_ptr = this->tree_pointers.back();
    child_idx = this->tree_pointers.size() - 1;
    if(key <= this->keys[0])
    {
        child_ptr = this->tree_pointers[0];
        child_idx = 0;
    }
    else if(key > this->keys.back())
    {
        child_ptr = this->tree_pointers.back();
        child_idx = this->tree_pointers.size() - 1;
    }
    else 
    {
        for(int a = 0; a < this->keys.size() - 1; a++)
            if(key > this->keys[a] && key <= this->keys[a + 1])
            {
                child_ptr = this->tree_pointers[a + 1];
                child_idx = a + 1;
            }
    }
    // Inserting the key into the child pointer
    TreeNode *child_node = this->tree_node_factory(child_ptr);
    TreePtr child_ret = child_node->insert_key(key, record_ptr);
    if(child_ret != NULL_PTR)
    {
        TreeNode *new_child_node = this->tree_node_factory(child_ret);
        this->keys.push_back(child_node->max());
        this->tree_pointers.push_back(child_ret);
        for(int a = this->keys.size() - 1; a >= 1; a--)
            if(this->keys[a] < this->keys[a - 1])
            {
                swap(this->keys[a], this->keys[a - 1]);
                swap(this->tree_pointers[a], this->tree_pointers[a + 1]);
            }
        this->size++;
        if(this->size > FANOUT)
        {
            InternalNode *new_internal_node = new InternalNode();
            while(this->size > (FANOUT + 1) / 2)
            {
                new_internal_node->keys.push_back(this->keys.back());
                new_internal_node->tree_pointers.push_back(this->tree_pointers.back());
                this->keys.pop_back();
                this->tree_pointers.pop_back();
                this->size--;
                new_internal_node->size++;
            }
            for(int a = 0; a < new_internal_node->tree_pointers.size() / 2; a++)
                swap(new_internal_node->tree_pointers[a], new_internal_node->tree_pointers[new_internal_node->tree_pointers.size() - a - 1]);
            for(int a = 0; a < new_internal_node->keys.size() / 2; a++)
                swap(new_internal_node->keys[a], new_internal_node->keys[new_internal_node->keys.size() - a - 1]);
            new_internal_node->keys.pop_back();
            new_internal_node->dump();
            new_tree_ptr = new_internal_node->tree_ptr;
        }
    }
    this->dump();
    return new_tree_ptr;
}

//deletes key from subtree rooted at this if exists
//TODO: InternalNode::delete_key to be implemented
void InternalNode::delete_key(const Key &key) {
    TreePtr new_tree_ptr = NULL_PTR;
    TreePtr child_ptr; // the child pointer where the key has to go
    int child_idx;
    // Finding the child pointer
    child_ptr = this->tree_pointers.back();
    child_idx = this->tree_pointers.size() - 1;
    if(key <= this->keys[0])
    {
        child_ptr = this->tree_pointers[0];
        child_idx = 0;
    }
    else if(key > this->keys.back())
    {
        child_ptr = this->tree_pointers.back();
        child_idx = this->tree_pointers.size() - 1;
    }
    else 
    {
        for(int a = 0; a < this->keys.size() - 1; a++)
            if(key > this->keys[a] && key <= this->keys[a + 1])
            {
                child_ptr = this->tree_pointers[a + 1];
                child_idx = a + 1;
            }
    }
    // DELETING
    TreeNode *child_node = this->tree_node_factory(child_ptr);
    child_node->delete_key(key);
    // CHECK IF UNDERFLOW
    if(child_node->node_type == INTERNAL)
    {
        // Child is internal node
        InternalNode *child = new InternalNode(child_ptr);
        if(child->size < (FANOUT + 1) / 2)
        {
            // left redistribution
            bool leftRedistribution = false;
            InternalNode *prevChild, *nextChild;
            if(child_idx == 0);
            else 
            {
                prevChild = new InternalNode(this->tree_pointers[child_idx - 1]);
                if(prevChild->size + child->size >= 2 * ((FANOUT + 1) / 2))
                    leftRedistribution = true;
            }
            // left merge
            bool leftMerge = false;
            if(child_idx == 0);
            else
            {
                prevChild = new InternalNode(this->tree_pointers[child_idx - 1]);
                if(prevChild->size + child->size <= FANOUT)
                    leftMerge = true;
            }
            // right redistribute
            bool rightRedistribution = false;
            if(child_idx == this->tree_pointers.size() - 1);
            else
            {
                nextChild = new InternalNode(this->tree_pointers[child_idx + 1]);
                if(nextChild->size + child->size >= 2 * ((FANOUT + 1) / 2))
                    rightRedistribution = true;
            }
            // right merge
            bool rightMerge = false;
            if(child_idx == this->tree_pointers.size() - 1);
            else
            {
                nextChild = new InternalNode(this->tree_pointers[child_idx + 1]);
                if(nextChild->size + child->size <= FANOUT)
                    rightMerge = true;
            }
            if(leftRedistribution)
                this->internalLeftRedistribute(child_idx);
            else if(leftMerge)
                this->internalLeftMerge(child_idx);
            else if(rightRedistribution)
                this->internalRightRedistribute(child_idx);
            else if (rightMerge)
                this->internalRightMerge(child_idx);
        }
    }
    else 
    {
        // Child is leaf node
        LeafNode *child = new LeafNode(child_ptr);
        if(child->size < (FANOUT + 1) / 2)
        {
            // left redistribute
            bool leftRedistribution = false;
            LeafNode *prevChild, *nextChild;
            if(child_idx == 0);
            else
            {
                prevChild = new LeafNode(this->tree_pointers[child_idx - 1]);
                if(prevChild->size + child->size >= 2 * ((FANOUT + 1) / 2))
                    leftRedistribution = true;
            }
            // left merge
            bool leftMerge = false;
            if(child_idx == 0);
            else
            {
                prevChild = new LeafNode(this->tree_pointers[child_idx - 1]);
                if(prevChild->size + child->size <= FANOUT)
                    leftMerge = true;
            }
            // right redistribute
            bool rightRedistribution = false;
            if(child_idx == this->tree_pointers.size() - 1);
            else
            {
                nextChild = new LeafNode(this->tree_pointers[child_idx + 1]);
                if(nextChild->size + child->size >= 2 * ((FANOUT + 1) / 2))
                    rightRedistribution = true;
            }
            // right merge
            bool rightMerge = false;
            if(child_idx == this->tree_pointers.size() - 1);
            else
            {
                nextChild = new LeafNode(this->tree_pointers[child_idx + 1]);
                if(nextChild->size + child->size <= FANOUT)
                    rightMerge = true;
            }
            if(leftRedistribution)
                this->leafLeftRedistribute(child_idx);
            else if(leftMerge)
                this->leafLeftMerge(child_idx);
            else if(rightRedistribution)
                this->leafRightRedistribute(child_idx);
            else if (rightMerge)
                this->leafRightMerge(child_idx);
        }
    }
    this->dump();
    this->write(cout);
}

void InternalNode::leafLeftRedistribute(int idx)
{
    LeafNode *child = new LeafNode(this->tree_pointers[idx]);
    LeafNode *prevChild = new LeafNode(this->tree_pointers[idx - 1]);
    while(child->size < (FANOUT + 1) / 2)
    {
        RecordPtr here = prevChild->data_pointers.rbegin()->second;
        Key key = prevChild->data_pointers.rbegin()->first;
        prevChild->delete_key(key);
        child->insert_key(key, here);
        this->keys[idx - 1] = prevChild->max();
    }
    child->dump();
    prevChild->dump();
}

void InternalNode::internalLeftRedistribute(int idx)
{
    InternalNode *child = new InternalNode(this->tree_pointers[idx]);
    InternalNode *prevChild = new InternalNode(this->tree_pointers[idx - 1]);
    while(child->size < (FANOUT + 1) / 2)
    {
        TreePtr here = prevChild->tree_pointers.back();
        prevChild->tree_pointers.pop_back();
        prevChild->keys.pop_back();
        child->tree_pointers.push_back(here);
        TreeNode *grandChild = this->tree_node_factory(here);
        Key key = grandChild->max();
        child->keys.push_back(key);
        for(int a = child->keys.size() - 1; a >= 1; a--)
            swap(child->keys[a], child->keys[a - 1]);
        for(int a = child->tree_pointers.size() - 1; a >= 1; a--)
            swap(child->tree_pointers[a], child->tree_pointers[a - 1]);
        this->keys[idx - 1] = prevChild->max();
        child->size++;
        prevChild->size--;
    }
    child->dump();
    prevChild->dump();
}

void InternalNode::leafLeftMerge(int idx)
{
    LeafNode *child = new LeafNode(this->tree_pointers[idx]);
    TreePtr todel = this->tree_pointers[idx];
    LeafNode *prevChild = new LeafNode(this->tree_pointers[idx - 1]);
    while(child->size)
    {
        RecordPtr here = child->data_pointers.rbegin()->second;
        Key key = child->data_pointers.rbegin()->first;
        child->delete_key(key);
        prevChild->insert_key(key, here);
    }
    for(int a = idx; a < this->tree_pointers.size() - 1; a++)
        swap(this->tree_pointers[a], this->tree_pointers[a + 1]);
    this->tree_pointers.pop_back();
    for(int a = idx - 1; a < this->keys.size() - 1; a++)
        swap(this->keys[a], this->keys[a + 1]);
    this->keys.pop_back();
    this->size--;
    child->dump();
    prevChild->dump();
    delete_file(todel);
}

void InternalNode::internalLeftMerge(int idx)
{
    InternalNode *child = new InternalNode(this->tree_pointers[idx]);
    TreePtr todel = this->tree_pointers[idx];
    InternalNode *prevChild = new InternalNode(this->tree_pointers[idx - 1]);

    TreeNode *grandChild = this->tree_node_factory(prevChild->tree_pointers.back());
    prevChild->keys.push_back(grandChild->max());
    for(int a = 0; a < child->tree_pointers.size(); a++)
    {
        prevChild->tree_pointers.push_back(child->tree_pointers[a]);
        prevChild->size++;
    }
    for(int a = 0; a < child->keys.size(); a++)
        prevChild->keys.push_back(child->keys[a]);
    while(child->keys.size())
        child->keys.pop_back();
    while(child->tree_pointers.size())
        child->tree_pointers.pop_back();
    // remove child from this
    for(int a = idx; a < this->tree_pointers.size() - 1; a++)
        swap(this->tree_pointers[a], this->tree_pointers[a + 1]);
    this->tree_pointers.pop_back();
    for(int a = idx - 1; a < this->keys.size() - 1; a++)
        swap(this->keys[a], this->keys[a + 1]);
    this->keys.pop_back();
    this->size--;
    child->dump();
    prevChild->dump();
    delete_file(todel);
}

void InternalNode::leafRightRedistribute(int idx)
{
    LeafNode *child = new LeafNode(this->tree_pointers[idx]);
    LeafNode *nextChild = new LeafNode(this->tree_pointers[idx + 1]);
    while(child->size < (FANOUT + 1) / 2)
    {
        RecordPtr here = nextChild->data_pointers.begin()->second;
        Key key = nextChild->data_pointers.begin()->first;
        nextChild->delete_key(key);
        child->insert_key(key, here);
        this->keys[idx] = child->max();
    }
    child->dump();
    nextChild->dump();
}

void InternalNode::internalRightRedistribute(int idx)
{
    InternalNode *child = new InternalNode(this->tree_pointers[idx]);
    InternalNode *nextChild = new InternalNode(this->tree_pointers[idx + 1]);
    while(child->size < (FANOUT + 1) / 2)
    {
        TreeNode *grandChild = this->tree_node_factory(child->tree_pointers.back());
        child->keys.push_back(grandChild->max());
        child->tree_pointers.push_back(nextChild->tree_pointers[0]);
        for(int a = 0; a < nextChild->tree_pointers.size() - 1; a++)
            swap(nextChild->tree_pointers[a], nextChild->tree_pointers[a + 1]);
        nextChild->tree_pointers.pop_back();
        for(int a = 0; a < nextChild->keys.size() - 1; a++)
            swap(nextChild->keys[a], nextChild->keys[a + 1]);
        nextChild->keys.pop_back();
    }
    child->dump();
    nextChild->dump();
}

void InternalNode::leafRightMerge(int idx)
{
    LeafNode *child = new LeafNode(this->tree_pointers[idx]);
    LeafNode *nextChild = new LeafNode(this->tree_pointers[idx + 1]);
    while(child->size)
    {
        RecordPtr here = child->data_pointers.rbegin()->second;
        Key key = child->data_pointers.rbegin()->first;
        child->delete_key(key);
        nextChild->insert_key(key, here); 
    }
    for(int a = idx; a < this->tree_pointers.size() - 1; a++)
        swap(this->tree_pointers[a], this->tree_pointers[a + 1]);
    this->tree_pointers.pop_back();
    for(int a = idx; a < this->keys.size() - 1; a++)
        swap(this->keys[a], this->keys[a + 1]);
    this->keys.pop_back();
    this->size--;
    child->dump();
    nextChild->dump();
}

void InternalNode::internalRightMerge(int idx)
{
    InternalNode *child = new InternalNode(this->tree_pointers[idx]);
    InternalNode *nextChild = new InternalNode(this->tree_pointers[idx + 1]);
    TreePtr todel = this->tree_pointers[idx + 1];

    for(int a = 0; a < nextChild->tree_pointers.size(); a++)
    {
        TreeNode *grandChild = this->tree_node_factory(child->tree_pointers.back());
        child->keys.push_back(grandChild->max());
        child->tree_pointers.push_back(nextChild->tree_pointers[a]);
        child->size++;
    }
    while(nextChild->tree_pointers.size())
        nextChild->tree_pointers.pop_back();
    while(nextChild->keys.size())
        nextChild->keys.pop_back();
    for(int a = idx; a < this->keys.size() - 1; a++)
        swap(this->keys[a], this->keys[a + 1]);
    this->keys.pop_back();
    for(int a = idx + 1; a < this->tree_pointers.size() - 1; a++)
        swap(this->tree_pointers[a], this->tree_pointers[a + 1]);
    this->tree_pointers.pop_back();
    this->size--;
    child->dump();
    nextChild->dump();
    delete_file(todel);
}

//runs range query on subtree rooted at this node
void InternalNode::range(ostream &os, const Key &min_key, const Key &max_key) const {
    BLOCK_ACCESSES++;
    for(int i = 0; i < this->size - 1; i++){
        if(min_key <= this->keys[i]){
            auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
            child_node->range(os, min_key, max_key);
            delete child_node;
            return;
        }
    }
    auto* child_node = TreeNode::tree_node_factory(this->tree_pointers[this->size - 1]);
    child_node->range(os, min_key, max_key);
    delete child_node;
}

//exports node - used for grading
void InternalNode::export_node(ostream &os) {
    TreeNode::export_node(os);
    for(int i = 0; i < this->size - 1; i++)
        os << this->keys[i] << " ";
    os << endl;
    for(int i = 0; i < this->size; i++){
        auto child_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        child_node->export_node(os);
        delete child_node;
    }
}

//writes subtree rooted at this node as a mermaid chart
void InternalNode::chart(ostream &os) {
    string chart_node = this->tree_ptr + "[" + this->tree_ptr + BREAK;
    chart_node += "size: " + to_string(this->size) + BREAK;
    chart_node += "]";
    os << chart_node << endl;

    for(int i = 0; i < this->size; i++) {
        auto tree_node = TreeNode::tree_node_factory(this->tree_pointers[i]);
        tree_node->chart(os);
        delete tree_node;
        string link = this->tree_ptr + "-->|";

        if(i == 0)
            link += "x <= " + to_string(this->keys[i]);
        else if (i == this->size - 1) {
            link += to_string(this->keys[i-1]) + " < x";
        } else {
            link += to_string(this->keys[i-1]) + " < x <= " + to_string(this->keys[i]);
        }
        link += "|" + this->tree_pointers[i];
        os << link << endl;
    }
}

ostream& InternalNode::write(ostream &os) const {
    TreeNode::write(os);
    for(int i = 0; i < this->size - 1; i++){
        if(&os == &cout)
            os << "\nP" << i+1 << ": ";
        os << this->tree_pointers[i] << " ";
        if(&os == &cout)
            os << "\nK" << i+1 << ": ";
        os << this->keys[i] << " ";
    }
    if(&os == &cout)
        os << "\nP" << this->size << ": ";
    os << this->tree_pointers[this->size - 1];
    return os;
}

istream& InternalNode::read(istream& is){
    TreeNode::read(is);
    this->keys.assign(this->size - 1, DELETE_MARKER);
    this->tree_pointers.assign(this->size, NULL_PTR);
    for(int i = 0; i < this->size - 1; i++){
        if(&is == &cin)
            cout << "P" << i+1 << ": ";
        is >> this->tree_pointers[i];
        if(&is == &cin)
            cout << "K" << i+1 << ": ";
        is >> this->keys[i];
    }
    if(&is == &cin)
        cout << "P" << this->size;
    is >> this->tree_pointers[this->size - 1];
    return is;
}
