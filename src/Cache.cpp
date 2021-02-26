#include <bits/stdc++.h>

using namespace std;

typedef long long ll;

// macros 
#define all(x) begin(x), end(x)
#define rall(x) rbegin(x), rend(x)

// convert integer to binary string of size sz
string tobinary(ll v, ll sz);
// increase the size of string s but putting zero before MSB
string makesize(string s, ll sz);

const int MA = 1000000007;

class Associative{
public:
    vector<ll> tag;
    vector<vector<ll>> data;
    vector<int> count;
    vector<bool> empty;
    int cachelines;
    int size;
    int blocksize;
    
    Associative(int cacheline, int siz, int blocksiz) {
	this->size = siz;
	this->cachelines = cacheline;
	this->blocksize = blocksiz;
	
	data.resize(cachelines, vector<ll> (blocksize));
	tag.resize(cachelines);
	count.resize(cachelines, 0);
	empty.resize(cachelines, true);
    }
    
    void incre() {
	for (int i = 0; i < cachelines; ++i) {
	    if (!empty[i]) {
		if (count[i] == MA) continue;
		count[i]++;
	    }
	}
    }
    
    ll read(ll add, ll off) {
	
	incre();
	
	for (int i = 0; i < cachelines; ++i) {
	    // if empty and has the same tag
	    if (!empty[i] && tag[i] == add) {
		for (int j = 0; j < blocksize; ++j) {
		    if (j == off) {
			// make count zero
			count[i] = 0;
			return data[i][j];
		    }
		}
	    }
	}
	// read miss
	return -1;
    }
    
    
    void write(ll add, ll off, ll value) {
	
	incre();
	
	for (int i = 0; i < cachelines; ++i) {
	    // if empty and has the same tag
	    if (tag[i] == add && !empty[i]) {
		// make count zero
		data[i][off] = value;
		count[i] = 0;
		return;
	    }
	}
	int Max = 0;
	int index = -1;
	for (int i = 0; i < cachelines; ++i) {
	    if (empty[i]) {
		tag[i] = add;
		count[i] = 0;
		for (int j = 0; j < blocksize; ++j) {
		    if (j == off) {
			data[i][j] = value;
		    }else {
			// by default values are zero
			data[i][j] = 0;
		    }
		}
		empty[i] = false;
		return;
	    }else {
		if (count[i] > Max) {
		    // stores the max count
		    Max = count[i];
		    index = i;
		}
	    }
	}
	tag[index] = add;
	count[index] = 0;
	for (int i = 0; i < blocksize; ++i) {
	    if (i == off) {
		data[index][i] = value;
	    }else {
		// by default values are zero
		data[index][i] = 0;
	    }
	}
    }
    
    void print() {
	for (int i = 0; i < cachelines; ++i) {
	    
	    if (empty[i]) {
		cout << "empty" << endl;
		continue;
	    }
	    
	    cout << "tag: " << tobinary(tag[i], 32 - log2(blocksize)) << " Data: ";
	    
	    for (int j = 0; j < blocksize; ++j) {
		cout << data[i][j] << " ";
	    }
	    cout << endl;
	}
    }
    
};

class Direct {
public:
    vector<ll> tag;
    vector<vector<ll>> data;
    vector<bool> empty;
    int cachelines;
    int size;
    int blocksize;

    Direct(int Cachelines, int Size, int Blocksize) {
	this -> cachelines = Cachelines;
	this -> size = Size;
	this -> blocksize = Blocksize;
	
	data.resize(cachelines, vector<ll> (blocksize));
	tag.resize(cachelines);
	empty.resize(cachelines, true);
    }
    
    ll read(ll add, ll add2, ll off) {
	if (empty[add2]) {
	    // Read Miss
	    return -1;
	}else {
	    // Read Hit
	    return data[add2][off];
	}
    }
    
    void write(ll add, ll add2, ll off, ll value) {
	if (empty[add2]) {
	    empty[add2] = false;
	    tag[add2] = add;
	    data[add2][off] = value;
	}else {
	    if (tag[add2] == add) {
		data[add2][off] = value;
		return;
	    }
	    for (int i = 0; i < blocksize; ++i) {
		data[add2][i] = 0;
	    }
	    data[add2][off] = value;
	    tag[add2] = add;
	}
    }
    
    void print() {
	for (int i = 0; i < cachelines; ++i) {
	    
	    if (empty[i]) {
		cout << "empty" << endl;
		continue;
	    }
	    
	    cout << "tag :" << tobinary(tag[i], 32 - log2(blocksize) - log2(cachelines)) << " Data : ";
	    
	    for (int j = 0; j < blocksize; ++j) {
		cout << data[i][j] << " ";
	    }
	    cout << endl;
	}
    }

};

class Nway {
public:
    int N;
    vector<ll> tag;
    vector<vector<vector<ll>>> data;
    vector<int> count;
    vector<bool> empty;
    int cachelines;
    int size;
    int blocksize;

    Nway(int Cachelines, int Size, int Blocksize, int n) {
	this->cachelines = Cachelines;
	this->size = Size;
	this->blocksize = Blocksize;
	this->N = n;
	
	data.resize(Cachelines/N, vector<vector<ll>> (N, vector<ll> (blocksize)));
	count.resize(cachelines, 0);
	empty.resize(cachelines, true);
	tag.resize(cachelines);
    }
    
    void incre() {
	for (int i = 0; i < cachelines; ++i) {
	    if (!empty[i]) {
		if (count[i] == MA) continue;
		count[i]++;
	    }
	}
    }
    
    ll read(ll add, ll add2, ll off) {
	incre();
	for (int i = 0; i < N; ++i) {
	    if (!empty[add2 * N + i] && tag[add2 * N + i] == add) {
		count[add2 * N + i] = 0;
		return data[add2][i][off];
	    }
	}
	// Read Miss
	return -1;
    }
    
    void write(ll add, ll add2, ll off, ll value) {
	
	incre();
	
	for (int i = 0; i < N; ++i) {
	    if (!empty[add2 * N + i] && tag[add2 * N + i] == add) {
		data[add2][i][off] = value;
		count[add2 * N + i] = 0;
		return;
	    }
	}
	
	int Max = 0;
	int index = -1;
	for (int i = 0; i < N; ++i) {
	    // add2 * N + i map the i elements off add2-th set to its index in empty, tag, count array
	    if (empty[add2 * N + i]) {
		count[add2 * N + i] = 0;
		tag[add2 * N + i] = add;
		empty[add2 * N + i] = false;
		for (int j = 0; j < blocksize; ++j) {
		    if (j == off) {
			data[add2][i][j] = value;
		    }else {
			data[add2][i][j] = 0;
		    }
		}
		return;
	    }else {
		if (count[add2 * N + i] > Max) {
		    Max = count[add2 * N + i];
		    index = i;
		}
	    }
	}
	// add2 * N + i map the i elements off add2-th set to its index in empty tag and count array
	count[add2 * N + index] = 0;
	tag[add2 * N + index] = add;
	for (int i = 0; i < blocksize; ++i) {
	    if (i == off) {
		data[add2][index][i] = value;
	    }else {
		data[add2][index][i] = 0;
	    }
	}
    }
    
    void print() {
	for (int i = 0; i < cachelines; ++i) {
	    
	    if (empty[i]) {
		cout << "empty" << endl;
		continue;
	    }
	    
	    if (i % N == 0) {
		// differentiate between sets
		cout << endl;
	    }
	    
	    cout << "block " << tobinary(tag[i], 32 - log2(blocksize) - log2(cachelines/N)) << " Data: ";
	    
	    for (int j = 0; j < blocksize; ++j) {
		cout << data[i/N][i % N][j] << " ";
	    }
	    cout << endl;
	}
    }
};

// convert binary string into int
ll toint(string s) {
    ll ans = 0;
    ll n = s.length();
    for (int i = 0; i < n; ++i) {
	ans += (s[i] - '0') * pow(2, n - i - 1);
    }
    return ans;
}

string tobinary(ll v, ll sz) {
    string ans = "";
    if (!v) {
	return makesize("0", sz);
    }
    while(v > 0) {
	ans += char(v % 2 + 48);
	v/=2;
    }
    reverse(all(ans));
    return makesize(ans, sz);
}

string makesize(string s, ll sz) {
    string ans = "";
    for (int i = 0; i < (int)(sz - s.length()); ++i) ans += '0';
    ans += s;
    return ans;
}

int main() {
    
    cout << "<<<<<<<<===============================CACHE-SIMULATER===============================>>>>>>>>" << endl << endl;
    
    // game loop
    while(true) {
	
	int choice;
	cout << "<============================================>" << endl << endl;
	cout << "Please enter your choice..." << endl;
	cout << "1. Associative Cache" << endl;
	cout << "2. Direct Mapped Cache" << endl;
	cout << "3. N-way Set Associative Cache" << endl;
	cout << "4. Exit";
	cout << endl;
	cin >> choice;
	
	if (choice == 1) {
	    
	    ll sz, blsz, cl;
	    
	    cout << "Enter size of cache :";
	    cin >> sz;
	    
	    cout << "Enter size of a block :";
	    cin >> blsz;
	    
	    cout << "Enter number of cl :";
	    cin >> cl;
	    
	    if (cl != sz/blsz) {
		cout << "Incorrect Data Please Try Again" << endl << endl << endl;
		continue;
	    }
	    
	    Associative cache(cl, sz, blsz);
	    
	    cout << "Cache Created Succesfully" << endl;
	    
	    while (true) {
		
		cout <<  "Please enter your choice" << endl;
		cout << "1. Print Cache" << endl;
		cout << "2. Read data" << endl;
		cout << "3. Write data" << endl;
		cout << "4. Go to Main Menu" << endl;
		
		int nestedChoice;
		cin >> nestedChoice;
		
		if (nestedChoice == 1) {
		    cache.print();
		    cout << endl;
		}else if(nestedChoice == 2) {
		    string adr;
		    cout << "Enter the address in binary: ";
		    cin >> adr;
		    adr = makesize(adr, 32);
		    
		    ll of = toint(adr.substr(32 - (int)log2(blsz), (int)log2(blsz)));
		    ll ad = toint(adr.substr(0, 32 - (int)log2(blsz)));
		    
		    ll val = cache.read(ad, of);
		    
		    if (val == -1) {
			cout << "Read Miss" << endl;
		    }else {
			cout << "Read Hit Value: ";
			cout << val << endl;
		    }
		}else if(nestedChoice == 3) {
		    string adr;
		    cout << "Enter the address in Binary: ";
		    cin >> adr;
		    adr = makesize(adr, 32);
		    cout << "Enter the value to be written (0-255, 1 byte) in Decimal: "; 
		    ll val;
		    cin >> val;
		    // check for overflow
		    if (val > 255) {
			cout << "Incorrect Input Please Try Again" << endl;
		    }
		    ll of = toint(adr.substr(32 - (int)log2(blsz), (int)log2(blsz)));
		    ll ad = toint(adr.substr(0, 32 - (int)log2(blsz)));
		    
		    cache.write(ad, of, val);
		    
		}else if (nestedChoice == 4) {
		    cout << endl;
		    break;
		}else {
		    cout << "Invalid Choice Try Again..." << endl;
		}
	    }
	    cout << endl;
	}else if (choice == 2) {
	    
	    ll sz, blsz, cl;
	    
	    cout << "Enter size of cache :";
	    cin >> sz;
	    
	    cout << "Enter size of a block :";
	    cin >> blsz;
	    
	    cout << "Enter number of cl :";
	    cin >> cl;
	    
	    if (cl != sz/blsz) {
		cout << "Incorrect Data Please Try Again" << endl << endl << endl;
		continue;
	    }
	    
	    Direct cache(cl, sz, blsz);
	    
	    cout << "Cache Created Succesfully" << endl;
	    
	    while (true) {
		
		cout <<  "Please enter your choice" << endl;
		cout << "1. Print Cache" << endl;
		cout << "2. Read data" << endl;
		cout << "3. Write data" << endl;
		cout << "4. Go to Main Menu" << endl;
		
		int nestedChoice;
		cin >> nestedChoice;
		
		if (nestedChoice == 1) {
		    cache.print();
		    cout << endl;
		}else if(nestedChoice == 2) {
		    string adr;
		    cout << "Enter the address in Binary: ";
		    cin >> adr;
		    adr = makesize(adr, 32);
		    
		    ll of = toint(adr.substr(32 - (int)log2(blsz), (int)log2(blsz)));
		    ll ad2 = toint(adr.substr(32 - (int)log2(blsz) - (int)log2(cl), (int)log2(cl)));
		    ll ad = toint(adr.substr(0, 32 - (int)log2(blsz) - (int)log2(cl)));
		    
		    ll val = cache.read(ad, ad2, of);
		    
		    if (val == -1) {
			cout << "Read Miss" << endl;
		    }else {
			cout << "Read Hit Value: ";
			cout << val << endl;
		    }
		}else if(nestedChoice == 3) {
		    string adr;
		    cout << "Enter the address in Binary: ";
		    cin >> adr;
		    cout << "Enter the value to be written (0-255, 1 byte) in Decimal: "; 
		    adr = makesize(adr, 32);
		    ll val;
		    cin >> val;
		    // check for overflow
		    if (val > 255) {
			cout << "Incorrect Input Please Try Again" << endl;
		    }
		    ll of = toint(adr.substr(32 - (int)log2(blsz), (int)log2(blsz)));
		    ll ad2 = toint(adr.substr(32 - (int)log2(blsz) - (int)log2(cl), (int)log2(cl)));
		    ll ad = toint(adr.substr(0, 32 - (int)log2(blsz) - (int)log2(cl)));
		    
		    cache.write(ad, ad2, of, val);
		    
		}else if (nestedChoice == 4) {
		    cout << endl;
		    break;
		}else {
		    cout << "Invalid Choice Try Again..." << endl;
		}
	    }
	    cout << endl;
	}else if (choice == 3) {
	
	    ll sz, blsz, cl, n;
	    
	    cout << "Enter size of cache: ";
	    cin >> sz;
	    
	    cout << "Enter size of a block: ";
	    cin >> blsz;
	    
	    cout << "Enter number of cl: ";
	    cin >> cl;
	    
	    cout << "Enter size of set: " ;
	    cin >> n;
	    
	    
	    if (cl != sz/blsz) {
		cout << "Incorrect Data Please Try Again" << endl << endl << endl;
		continue;
	    }
	    
	    Nway cache(cl, sz, blsz, n);
	    
	    cout << "Cache Created Succesfully" << endl;
	    
	    while (true) {
		
		cout <<  "Please enter your choice" << endl;
		cout << "1. Print Cache" << endl;
		cout << "2. Read data" << endl;
		cout << "3. Write data" << endl;
		cout << "4. Go to Main Menu" << endl;
		int nestedChoice;
		cin >> nestedChoice;
		
		if (nestedChoice == 1) {
		    cache.print();
		    cout << endl;
		}else if(nestedChoice == 2) {
		    string adr;
		    cout << "Enter the address in Binary: ";
		    cin >> adr;
		    adr = makesize(adr, 32);
		    
		    ll of = toint(adr.substr(32 - (int)log2(blsz), (int)log2(blsz)));
		    ll ad2 = toint(adr.substr(32 - (int)log2(blsz) - (int)log2(cl/n), (int)log2(cl/n)));
		    ll ad = toint(adr.substr(0, 32 - (int)log2(blsz) - (int)log2(cl/n)));
		    
		    ll val = cache.read(ad, ad2, of);
		    
		    if (val == -1) {
			cout << "Read Miss" << endl;
		    }else {
			cout << "Read Hit Value: ";
			cout << val << endl;
		    }
		}else if(nestedChoice == 3) {
		    string adr;
		    cout << "Enter the address in Binary: ";
		    cin >> adr;
		    adr = makesize(adr, 32);
		    ll val;
		    cout << "Enter the value to be written (0-255, 1 byte) in Decimal: ";
		    cin >> val;
		    // check for overflow
		    if (val > 255) {
			cout << "Incorrect Input Please Try Again" << endl;
		    }
		    ll of = toint(adr.substr(32 - (int)log2(blsz), (int)log2(blsz)));
		    ll ad2 = toint(adr.substr(32 - (int)log2(blsz) - (int)log2(cl/n), (int)log2(cl/n)));
		    ll ad = toint(adr.substr(0, 32 - (int)log2(blsz) - (int)log2(cl/n)));
		    
		    cache.write(ad, ad2, of, val);
		    
		}else if (nestedChoice == 4) {
		    cout << endl;
		    break;
		}else {
		    cout << "Invalid Choice Try Again..." << endl;
		}
		cout << endl;
	    }
	 
	}else if (choice == 4) {
	    cout << "Exiting..." << endl;
	    break;
	}else {
	    cout << "Invalid Choice Try Again..." << endl;
	}
    }

    return 0;
}
