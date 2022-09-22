#include <iostream> 
#include <vector>
#include <unordered_set>



using namespace std;


namespace N {
    class Hash {
        public:

        // entries of hash table
        int size;

        // number of hashes for each key
        int k;

        // number of flows
        int flows;

        // underlying array
        vector<vector<int>> table;

        // randomly generated modifiers for hashing
        unordered_set<int> modifiers;

        // constructor
        Hash(int size, int k);

        // insert functionality
        bool insert(int key);

        // print out table content
        void print(bool suppressList = true);


    };






    class CuckooHash : public Hash{
        private:

        bool move(int i, int s);

        public :
                
        // Cuckoo steps
        int s;

        // Drived constructor for CuckooHash
        // size, number of different hashes, most number of depth of recursion stack for the moving step
        CuckooHash(int size, int k, int s) : Hash(size, k) {
            this->s = s;
        };

        bool insert(int key);


    };






    class D_Left : public Hash {
        
        private:

        bool receive(int key);




        public:

        bool insert(int key);

        D_Left(int size, int k) : Hash(size, k) {};

    };
};


using namespace N;




Hash::Hash(int size, int k) {
    this->size = size;
    this->table = vector<vector<int>>(size, vector<int>(2, 0));
    this->flows = 0;
    this->k = k;
    int i = 0;
    while (i < k) {
        int c = rand() % 100;
        if (this->modifiers.find(c) == this->modifiers.end()) {
            this->modifiers.insert(c);
            i++;
        }
    }


} 

bool Hash::insert(int key) {
    for (int modifier : this->modifiers) {
        int i = (key ^ modifier) % this->size;

        // table[i][1] to be zero to take care of the edge case where key is hashed to 0.
        // if the key is hashed to zero and we dont check for table[i][1]
        // we will mistakenly think that we have never seen this hash before
        if ((this->table[i][0] == 0 && this->table[i][1] == 0)|| this->table[i][0] == key) {    // no key has mapped to this index or exacatly the same key
                                                                                                // has been mapped to the index
            this->table[i][0] = key;
            this->table[i][1] += 1;
            this->flows += 1;
            return true;
        }
    }
    return false;
}

void Hash:: print(bool suppressList) {
    cout << "Number of flows: " << this->flows << endl;
    if (! suppressList) {
        for (int i = 0; i < this->size; i++) {
            int flow = this->table[i][0];
            cout << i << ": " << flow << " " << this->table[i][1] << endl; 
        }
    }
}






// CuckooHash methods
bool CuckooHash::insert(int key)  {
    if (!Hash::insert(key)) {                                                      // if all k hashes have collesions 
        for (int modifier : this->modifiers) {
            int i = (key ^ modifier) % this->size;
            if (move(i, s)) {                                                       // we see if each of the collided entries can be moved
                return Hash::insert(key);                                           // if so, we try to insert again
            }
        }
        return false;
    }
    return true;                                                       
}

bool CuckooHash::move(int i, int s) {
    int movingFlowID = this->table[i][0];
    for (int modifier : this->modifiers) {
        int i_ = (movingFlowID ^ modifier) % this->size;
        // we added table[i_][0] for the same reason in Hash::insert
        if (i_ != i && table[i_][0] == 0 && table[i_][1] == 0) {
            swap(table[i], table[i_]);
            return true;
        }
    }

    if (s > 1) {
        for (int modifier : this->modifiers) {
            int i_ = (movingFlowID ^ modifier) % this->size;
            if (i_ != i && CuckooHash::move(i_, s - 1)) {
                this->table[i_][0] = movingFlowID;
                this->table[i_][1]++;
                return true;
            }
        }        
    }
    return false;
}





// D_Left hash methods
bool D_Left::receive(int key) {
    for ( int d = 0; d < this->k; d++) {
        int i = (int)(((key ^ (*this->modifiers.begin() + d)) % (int)(this->size / this->k)) + (d * this->size / this->k));
        if (this->table[i][0] == key) {
            this->table[i][1]++;
            this->flows++;
            return true;
        }
    }
    return false;
}


bool D_Left::insert(int key) {
    if (!D_Left::receive(key)) {
        for (int d = 0; d < this -> k; d++) {
            int i = (int)(((key ^ (*this->modifiers.begin() + d)) % (this->size / this->k)) + (d * this->size / this->k));
            if (this->table[i][0] == 0) {
                this->table[i][0] = key;
                this->table[i][1]++;
                this->flows++;
                return true;
            }
        }
        return false;
    }
    

    return true;
    
}




// suppressList suppress long list output of detailed content of the hash table



void runHash(bool suppressList = true) {
    Hash myHash = Hash(1000, 3);

    srand (time(0));  
                        // number of entries
    for (int i = 0; i < 1000; i++) {
        myHash.insert(rand());
    }
    myHash.print(suppressList);
}


void runCuckoo(bool suppressList = true) {
    CuckooHash myHash = CuckooHash(1000, 3, 3);

    srand (time(0));
    for (int i = 0; i < 1000; i++) {
        myHash.insert(rand());
    }
    myHash.print(suppressList);
}


void runD_Left(bool suppressList = true) {
    D_Left myHash = D_Left(1000, 4);

    srand (time(0));
    for (int i = 0; i < 1000; i++) {
        myHash.insert(rand());
    }
    myHash.print(suppressList);
}



int main() {
    runHash(false);
    runCuckoo(false);
    runD_Left(false);
    return 0;
 

}



