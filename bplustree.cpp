// Searching on a B+ tree in C++
#include <iostream>

using namespace std;
int d = 1;

// BP node
class B_plus {
  bool IS_LEAF;
  int *key, size;
  B_plus **ptr;
  friend class BPTree;

   public:
  B_plus();
};

// BP tree
class BPTree {
  B_plus *root;
  void insertInternal(int, B_plus *, B_plus *);
  B_plus *findParent(B_plus *, B_plus *);

   public:
  BPTree();
  void search(int);
  void insert(int);
  void display(B_plus *);
  B_plus *getRoot();
};

B_plus::B_plus() {
  key = new int[2*d+1];
  ptr = new B_plus *[(2*d+1) + 1];
}

BPTree::BPTree() {
  root = NULL;
}

// Search operation
void BPTree::search(int x) {
  if (root == NULL) {
    cout << "Tree is empty\n";
  } else {
    B_plus *cursor = root;
    while (cursor->IS_LEAF == false) {
      for (int i = 0; i < cursor->size; i++) {
        if (x < cursor->key[i]) {
          cursor = cursor->ptr[i];
          break;
        }
        if (i == cursor->size - 1) {
          cursor = cursor->ptr[i + 1];
          break;
        }
      }
    }
    for (int i = 0; i < cursor->size; i++) {
      if (cursor->key[i] == x) {
        cout << "Found\n";
        return;
      }
    }
    cout << "Not found\n";
  }
}

// Insert Operation
void BPTree::insert(int x) {
  if (root == NULL) {
    root = new B_plus;
    root->key[0] = x;
    root->IS_LEAF = true;
    root->size = 1;
  } else {
    B_plus *cursor = root;
    B_plus *parent;
    while (cursor->IS_LEAF == false) {
      parent = cursor;
      for (int i = 0; i < cursor->size; i++) {
        if (x < cursor->key[i]) {
          cursor = cursor->ptr[i];
          break;
        }
        if (i == cursor->size - 1) {
          cursor = cursor->ptr[i + 1];
          break;
        }
      }
    }
    if (cursor->size < 2*d+1) {
      int i = 0;
      while (x > cursor->key[i] && i < cursor->size)
        i++;
      for (int j = cursor->size; j > i; j--) {
        cursor->key[j] = cursor->key[j - 1];
      }
      cursor->key[i] = x;
      cursor->size++;
      cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
      cursor->ptr[cursor->size - 1] = NULL;
    } else {
      B_plus *newLeaf = new B_plus;
      int virtualNode[2*d+1 + 1];
      for (int i = 0; i < 2*d+1; i++) {
        virtualNode[i] = cursor->key[i];
      }
      int i = 0, j;
      while (x > virtualNode[i] && i < 2*d+1)
        i++;
      for (int j = 2*d+1 + 1; j > i; j--) {
        virtualNode[j] = virtualNode[j - 1];
      }
      virtualNode[i] = x;
      newLeaf->IS_LEAF = true;
      cursor->size = (2*d+1 + 1) / 2;
      newLeaf->size = 2*d+1 + 1 - (2*d+1 + 1) / 2;
      cursor->ptr[cursor->size] = newLeaf;
      newLeaf->ptr[newLeaf->size] = cursor->ptr[2*d+1];
      cursor->ptr[2*d+1] = NULL;
      for (i = 0; i < cursor->size; i++) {
        cursor->key[i] = virtualNode[i];
      }
      for (i = 0, j = cursor->size; i < newLeaf->size; i++, j++) {
        newLeaf->key[i] = virtualNode[j];
      }
      if (cursor == root) {
        B_plus *newRoot = new B_plus;
        newRoot->key[0] = newLeaf->key[0];
        newRoot->ptr[0] = cursor;
        newRoot->ptr[1] = newLeaf;
        newRoot->IS_LEAF = false;
        newRoot->size = 1;
        root = newRoot;
      } else {
        insertInternal(newLeaf->key[0], parent, newLeaf);
      }
    }
  }
}

// Insert Operation
void BPTree::insertInternal(int x, B_plus *cursor, B_plus *child) {
  if (cursor->size < 2*d+1) {
    int i = 0;
    while (x > cursor->key[i] && i < cursor->size)
      i++;
    for (int j = cursor->size; j > i; j--) {
      cursor->key[j] = cursor->key[j - 1];
    }
    for (int j = cursor->size + 1; j > i + 1; j--) {
      cursor->ptr[j] = cursor->ptr[j - 1];
    }
    cursor->key[i] = x;
    cursor->size++;
    cursor->ptr[i + 1] = child;
  } else {
    B_plus *newInternal = new B_plus;
    int virtualKey[2*d+1 + 1];
    B_plus *virtualPtr[2*d+1 + 2];
    for (int i = 0; i < 2*d+1; i++) {
      virtualKey[i] = cursor->key[i];
    }
    for (int i = 0; i < 2*d+1 + 1; i++) {
      virtualPtr[i] = cursor->ptr[i];
    }
    int i = 0, j;
    while (x > virtualKey[i] && i <2*d+1)
      i++;
    for (int j = 2*d+1 + 1; j > i; j--) {
      virtualKey[j] = virtualKey[j - 1];
    }
    virtualKey[i] = x;
    for (int j = 2*d+1 + 2; j > i + 1; j--) {
      virtualPtr[j] = virtualPtr[j - 1];
    }
    virtualPtr[i + 1] = child;
    newInternal->IS_LEAF = false;
    cursor->size = (2*d+1 + 1) / 2;
    newInternal->size = 2*d+1 - (2*d+1 + 1) / 2;
    for (i = 0, j = cursor->size + 1; i < newInternal->size; i++, j++) {
      newInternal->key[i] = virtualKey[j];
    }
    for (i = 0, j = cursor->size + 1; i < newInternal->size + 1; i++, j++) {
      newInternal->ptr[i] = virtualPtr[j];
    }
    if (cursor == root) {
      B_plus *newRoot = new B_plus;
      newRoot->key[0] = cursor->key[cursor->size];
      newRoot->ptr[0] = cursor;
      newRoot->ptr[1] = newInternal;
      newRoot->IS_LEAF = false;
      newRoot->size = 1;
      root = newRoot;
    } else {
      insertInternal(cursor->key[cursor->size], findParent(root, cursor), newInternal);
    }
  }
}

// Find the parent
B_plus *BPTree::findParent(B_plus *cursor, B_plus *child) {
  B_plus *parent;
  if (cursor->IS_LEAF || (cursor->ptr[0])->IS_LEAF) {
    return NULL;
  }
  for (int i = 0; i < cursor->size + 1; i++) {
    if (cursor->ptr[i] == child) {
      parent = cursor;
      return parent;
    } else {
      parent = findParent(cursor->ptr[i], child);
      if (parent != NULL)
        return parent;
    }
  }
  return parent;
}

// Print the tree
void BPTree::display(B_plus *cursor) {
  if (cursor != NULL) {
    for (int i = 0; i < cursor->size; i++) {
      cout << cursor->key[i] << " ";
    }
    cout << "\n";
    if (cursor->IS_LEAF != true) {
      for (int i = 0; i < cursor->size + 1; i++) {
        display(cursor->ptr[i]);
      }
    }
  }
}

// Get the root
B_plus *BPTree::getRoot() {
  return root;
}

int main() {
 BPTree B;
  int n,x;
  cin>>n;
  while(n!=3){
  if(n==1) //insert
   {  cin>>x;  //taking in key(data) value
      B.insert(x);
   }
   else if(n==2){
      B.display(B.getRoot());
   }
   cin>>n;
  }
  return 0;
}
