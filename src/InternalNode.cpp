#include "InternalNode.hpp"

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
    for(int a = 0; a < this->keys.size(); a++)
        if(this->keys[a] >= key)
        {
            child_ptr = this->tree_pointers[a];        
            child_idx = a;
            break;
        }
    // Inserting the key into the child pointer
    TreeNode *child_node = this->tree_node_factory(child_ptr);
    TreePtr child_ret = child_node->insert_key(key, record_ptr);
    if(child_ret != NULL_PTR)
    {
        TreeNode *new_child_node = this->tree_node_factory(child_ret);
        this->keys.push_back(new_child_node->max());
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
    cout << "InternalNode::delete_key not implemented" << endl;
    this->dump();
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
