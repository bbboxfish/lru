#ifndef SJTU_LRU_HPP
#define SJTU_LRU_HPP

#include "utility.hpp"
#include "exceptions.hpp"
#include "class-integer.hpp"
#include "class-matrix.hpp"
class Hash {
public:
	unsigned int operator () (Integer lhs) const {
		int val = lhs.val;
		return std::hash<int>()(val);
	}
};
class Equal {
public:
	bool operator () (const Integer &lhs, const Integer &rhs) const {
		return lhs.val == rhs.val;
	}
};

namespace sjtu {
template<class T> class double_list{
	public:
			struct node
			{
				T* data;
				node *prev;
				node *next;
				node(const T &data) : data(new T(data)), prev(nullptr), next(nullptr) {}
				~node(){
					delete data;
				}
			};
			node *head;
			node *tail;
			int l_size;
	/**
	 * elements
	 * add whatever you want
	*/

// --------------------------
	/**
	 * the follows are constructors and destructors
	 * you can also add some if needed.
	*/
	double_list(): head(nullptr), tail(nullptr)
	{}
	double_list(const double_list<T> &other): head(nullptr), tail(nullptr)
	{
		node *cur = other.head;
    	while (cur != nullptr) {
        insert_tail(*(cur->data));
        cur = cur->next;
    	}
	}
	~double_list(){
		node *p = head, *q;
			while (p)
			{
				q = p->next;
				delete p;
				p = q;
			}
	}
	double_list & operator=(const double_list &other){
		node *p = head, *q;
		while (p)
		{
			q = p->next;
			delete p;
			p = q;
		}
		head = tail = nullptr;
		node *cur = other.head;
    	while (cur != nullptr) {
			insert_tail(*(cur->data));
			cur = cur->next;
    	}
		return *this;
	}
	class iterator{
	public:
    	node *it;
	    // --------------------------
        /**
		 * the follows are constructors and destructors
		 * you can also add some if needed.
		*/
		iterator():it(nullptr){}
		iterator(const iterator &t):it(t.it){
		}
		iterator(node* t):it(t){}
		~iterator(){}
        /**
		 * iter++
		 */
		iterator operator++(int) {
			iterator tmp = *this;
			it = it -> prev;
			return *this;
		}
        /**
		 * ++iter
		 */
		iterator &operator++() {
			it = it->prev;
			return *this;
		}
        /**
		 * iter--
		 */
		iterator operator--(int) {
			iterator tmp = *this;
			it = it -> next;
			return *this;
		}
        /**
		 * --iter
		 */
		iterator &operator--() {
			it = it->next;
			return *this;
		}
		/**
		 * if the iter didn't point to a value
		 * throw " invalid"
		*/
		T &operator*() const {
			if (!it) {
            	throw "Iterator points to nothing";
        	}
			return *(it->data);
		}
        /**
         * other operation
        */
		T *operator->() const noexcept {
			return &(operator *());
		}
		bool operator==(const iterator &rhs) const {
			return it == rhs.it;
    	}
		bool operator!=(const iterator &rhs) const {
			return !(it == rhs.it);
		}
	};
	/**
	 * return an iterator to the beginning
	 */
	iterator begin(){
		return iterator(head);
	}
	iterator tail_(){
		return iterator(tail);
	}
	/**
	 * return an iterator to the ending
	 * in fact, it returns the iterator point to nothing,
	 * just after the last element.
	 */
	iterator end(){
		return iterator();
	}
	/**
	 * if the iter didn't point to anything, do nothing,
	 * otherwise, delete the element pointed by the iter
	 * and return the iterator point at the same "index"
	 * e.g.
	 * 	if the origin iterator point at the 2nd element
	 * 	the returned iterator also point at the
	 *  2nd element of the list after the operation
	 *  or nothing if the list after the operation
	 *  don't contain 2nd elememt.
	*/
	iterator erase(iterator pos){
		if(!pos.it)	return end();
		node *p = pos.it;
		if(p->prev){
			p->prev->next = p->next;
		}
		else{
			head = p->next;
		}
		if(p->next){
			p->next->prev = p->prev;
		}
		else{
			tail = p->prev;
		}
		node *q = p->next;
		delete p;
		return iterator(q);
	}

	/**
	 * the following are operations of double list
	*/
	void clear(){
		node *p = head, *q;
			while (p)
			{
				q = p->next;
				delete p;
				p = q;
			}
		head = tail = nullptr;
	}
	void insert_head(const T &val){
		node *newNode = new node(val);
		if (head == nullptr) {
			head = tail = newNode;
		} 
		else{
			head->prev = newNode;
			newNode->next = head;
			head = newNode;
		}
		++l_size;
	}
	void insert_tail(const T &val){
		node *newNode = new node(val);
		if (head == nullptr) {
			head = tail = newNode;
		} 
		else {
			tail->next = newNode;
			newNode->prev = tail;
			tail = newNode;
		}
    	++l_size;
	}
	void delete_head(){
		if(head == nullptr)	return;
		node *tmp = head;
		head = head->next;
		if (head) {
			head->prev = nullptr;
		} 
		else {
			tail = nullptr; 
		}
		delete tmp;
		--l_size;
	}
	void delete_tail(){
		if(tail == nullptr)	return;
		node *tmp = tail;
		tail = tail->prev;
		if (tail){
			tail->next = nullptr;
		}
		else{
			head = nullptr;
		}
		delete tmp;
		--l_size;
	}
	/**
	 * if didn't contain anything, return true, 
	 * otherwise false.
	 */
	bool empty(){
		return l_size == 0;
	}
};

template<
	class Key,
	class T,
	class Hash = std::hash<Key>, 
	class Equal = std::equal_to<Key>
> class hashmap{
public:
	using value_type = pair<const Key, T>;
	using list = double_list<value_type>;
	size_t c,size ;
	const size_t initial_c = 11;//当前大小
	double f = 0.75;//modify
	list* cap;
	Hash hash;
	Equal equal;
// --------------------------
	hashmap() {
		size = 0;
		c = initial_c;
		cap = new list[c];
	}
	hashmap(const hashmap &other){		
		c = other.c;
		size = other.size;
		cap = new list[c];
		for(int i=0;i<c;i++){
			cap[i] = other.cap[i];
		}
	}
	~hashmap(){
		delete [] cap;
	}
	hashmap & operator=(const hashmap &other){
		delete [] cap;
		size = other.size;
		c = other.c;
		cap = new list[c];
		for(int i=0;i<c;i++){
			cap[i] = other.cap[i];
		}
		return *this;
	}

	class iterator{
	public:
		using list = double_list<value_type>;
    	list *pos;
		typename list::iterator it;
// --------------------------
		iterator():pos(nullptr),it(){
		}
		iterator(const iterator &t):pos(t.pos),it(t.it){
		}
		iterator(list* pos_,typename list::iterator it_):pos(pos_),it(it_){}
		~iterator(){}

        /**
		 * if point to nothing
		 * throw 
		*/
		value_type &operator*() const {
			if (pos == nullptr ||it == pos->end()) {
				throw "303 star invalid";
			}
			return *it;
		}

        /**
		 * other operation
		*/
		value_type *operator->() const noexcept {
			return &(operator *());
		}
		bool operator==(const iterator &rhs) const {
			return pos == rhs.pos && it == rhs.it;
    	}
		bool operator!=(const iterator &rhs) const {
			return pos != rhs.pos || it != rhs.it;
		}
	};

	void clear(){
		size = 0;
		for(int i=0;i<c;i++){
			cap[i].clear();
		}
	}
	/**
	 * you need to expand the hashmap dynamically
	*/
	void expand(){
		size_t newc = 2*c+1;
		list *newcap = new list[newc];
		for(int i=0;i<c;i++){
			for(auto &t:cap[i]){
				newcap[hash(t.first) % newc].insert_head(t);
			}
		}
		delete[] cap;
		cap = newcap;
		c = newc;
	}

    /**
     * the iterator point at nothing
    */
	iterator end() const{
		return iterator();
	}
	/**
	 * find, return a pointer point to the value
	 * not find, return the end (point to nothing)
	*/
	iterator find(const Key &key)const{
		size_t v = hash(key) % c;
		list* tar = &(cap[v]);
		for(auto t = tar->begin();t != tar->end();t++)
		{
			if(equal(key,t->first))
			{//std::cout<<"find!"<<std::endl;
			return iterator(tar,t);}
		}
		return iterator();
	}
	/**
	 * already have a value_pair with the same key
	 * -> just update the value, return false
	 * not find a value_pair with the same key
	 * -> insert the value_pair, return true
	*/
	sjtu::pair<iterator,bool> insert(const value_type &value_pair){
		if ((double)size /c > f) {
        	expand();
    	}
		iterator tmp = find(value_pair.first);
		if(tmp == end()){
			auto h = hash(value_pair.first)%c;
			cap[h].insert_head(value_pair);
			size++;
			return pair<iterator,bool>(iterator(cap+h,(cap+h)->begin()),true);
		}
		(*(tmp.it.it->data)).second = value_pair.second;
		return pair<iterator,bool>(tmp,false);
	}
	/**
	 * the value_pair exists, remove and return true
	 * otherwise, return false
	*/
	bool remove(const Key &key){
		iterator tmp = find(key);
		if(tmp.it == tmp.pos->end())
		return false;
		size--;
		tmp.pos->erase(tmp.it);
		return true;
	}
	void print(){
		for(int i = 0;i<c;i++){
			std::cout<<i<<"th cap:";
			for(auto j:cap[i]){
				std::cout<<j.first<<" , "<<j.second<<"   ";
			}
			std::cout<<std::endl;
		}
	}
};

template<
	class Key,
	class T,
	class Hash = std::hash<Key>, 
	class Equal = std::equal_to<Key>
> class linked_hashmap {
public:
	typedef pair<const Key, T> value_type;
	typedef double_list<value_type> list;
	typedef typename list::iterator addr;
	list order;//存pair
	typename sjtu::double_list<addr> *data;//存迭代器
	size_t size_,cap;
	const double f = 0.5;
	const size_t initial_cap = 11;
	Hash hash;
	Equal equal;
	/**
	 * elements
	 * add whatever you want
	*/
// --------------------------
	class const_iterator;
	class hash_iterator{//data的
	public:
		typename  sjtu::double_list<addr> *pos;//哪一个链表
		typename  sjtu::double_list<addr>::iterator it_;//元素在链表中的位置
		hash_iterator():pos(nullptr),it_(){}
		hash_iterator(const hash_iterator &other):pos(other.pos),it_(other.it_){}
		hash_iterator(typename sjtu::double_list<addr> *a_pos,typename sjtu::double_list<addr>::iterator a_it_):pos(a_pos),it_(a_it_){}
		~hash_iterator(){}

		hash_iterator operator++(int) {
			hash_iterator temp = *this;
			++it_;
			return temp;
		}

		hash_iterator& operator++() {
			++it_;
			return *this;
		}

		hash_iterator operator--(int) {
			hash_iterator temp = *this;
			--it_;
			return temp;
		}

		hash_iterator& operator--() {
			--it_;
			return *this;
		}

		typename list::iterator &operator*() const{/////
			if(pos == nullptr) throw std::runtime_error("453 star invalid");
			return *(it_);
		}
		typename list::iterator *operator->() const noexcept{/////
			return &(operator*());
		}
		bool operator==(const hash_iterator &rhs) const{
			return pos == rhs.pos && it_ == rhs.it_;
		}
		bool operator!=(const hash_iterator &rhs) const{
			return pos != rhs.pos || it_ != rhs.it_;
		}
		hash_iterator exist(const Key& key){//是否存在
			size_t v = hash(key) % cap;
			typename sjtu::double_list<addr> &pos = data[v];
			for(auto &item : pos){
				value_type &p = *item;
				if(equal(key,p.first))
				return hash_iterator(pos,item);
			}
			return hash_iterator(pos,pos->end());//not found
		}
	};
	class iterator{//order的
	public:
    	/**
         * elements
         * add whatever you want
        */
		list::iterator it;
	   	
    // --------------------------
		iterator():it(){
		}
		iterator(const iterator &other):it(other.it){
		}
		iterator(typename list::iterator it_):it(it_){}
		~iterator(){
		}

		/**
		 * iter++
		 */
		iterator operator++(int) {
			iterator temp = *this;
        	if (it == iterator())
			throw std::runtime_error("invalid operation");
            ++it;
        	return temp;
		}
		/**
		 * ++iter
		 */
		iterator &operator++() {
			if(it == iterator())
			throw std::runtime_error("invalid operation");
            ++it;
        	return *this;
		}
		/**
		 * iter--
		 */
		iterator operator--(int) {
			iterator temp = *this;
        	if (it!= iterator())
            --it;
        	return temp;
		}
		/**
		 * --iter
		 */
		iterator &operator--() {
			if (it == nullptr)
			throw std::runtime_error("invalid operation");
            --it;
        	return *this;
		}
		/**
		 * if the iter didn't point to a value
		 * throw "star invalid"
		*/
		value_type &operator*() const {
			if (it == nullptr) {
            throw "535 star invalid";
        	}
        	return *it;
		}
		value_type *operator->() const noexcept {
			return &(operator*());
		}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return it == rhs.it ;
		}
		bool operator!=(const iterator &rhs) const {
			return it != rhs.it ;
		}
		bool operator==(const const_iterator &rhs) const {
			return it == rhs.it ;
		}
		bool operator!=(const const_iterator &rhs) const {
			return it != rhs.it ;
		}
	};
 
	class const_iterator {
		public:
		list::iterator it;
    // --------------------------   
		const_iterator():it() {
		}
		const_iterator(const iterator &other) :it(other.it){
		}

		/**
		 * iter++
		 */
		const_iterator operator++(int) {
			const_iterator temp = *this;
        	if (it != const_iterator())
            ++it;
        	return temp;
		}
		/**
		 * ++iter
		 */
		const_iterator &operator++() {
			if(it == const_iterator() )
			throw std::runtime_error("invalid operation");
            ++it;
        	return *this;
		}
		/**
		 * iter--
		 */
		const_iterator operator--(int) {
			const_iterator temp = *this;
        	if (it != const_iterator())
            --it;
        	return temp;
		}
		/**
		 * --iter
		 */
		const_iterator &operator--() {
			if (it == const_iterator())
			throw std::runtime_error("invalid operation");
            --it;
        	return *this;
		}

		/**
		 * if the iter didn't point to a value
		 * throw 
		*/
		const value_type &operator*() const {
			if (it == const_iterator()) {
            throw "Iterator points to nothing";
        	}
        	return *it;
		}
		const value_type *operator->() const noexcept {
			return &(operator*());
		}

		/**
		 * operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return it == rhs.it ;
		}
		bool operator!=(const iterator &rhs) const {
			return it != rhs.it ;
		}
		bool operator==(const const_iterator &rhs) const {
			return it == rhs.it ;
		}
		bool operator!=(const const_iterator &rhs) const {
			return it != rhs.it ;
		}
	};
 
	linked_hashmap(){
		size_ = 0;
		cap = initial_cap;
		order = list();
		data = new typename sjtu::double_list<addr>[cap];
	}
	linked_hashmap(const linked_hashmap &other){
		size_ = other.size_;
		cap = other.cap;
		data = new double_list<addr>[cap];
		order = other.order;
		for(int i=0;i<cap;i++){
			data[i] = other.data[i];
		}
	}
	~linked_hashmap() {
		clear();
		delete[] data;
	}
	linked_hashmap & operator=(const linked_hashmap &other) {
		this->clear();
		delete[] this->data;
		cap = other.cap;
		size_ = other.size_;
		data = new double_list<addr>[cap];//cap = 0?
		order = other.order;
		for(int i=0;i<cap;i++){
			data[i] = other.data[i];
		}
		return *this;
	}

 	/**
	 * return the value connected with the Key(O(1))
	 * if the key not found, throw 
	*/
	T & at(const Key &key) {
		iterator tar = find(key);
		if(tar == end()) throw "key not found";
		return tar->second;
	}
	const T & at(const Key &key) const {
		iterator tar = find(key);
		if(tar == end()) throw "key not found";
		return tar->second;
	}
	T & operator[](const Key &key) {
		iterator tar = find(key);
		if(tar == end()) throw "key not found";
		return tar->second;
	}
	const T & operator[](const Key &key) const {
		iterator tar = find(key);
		if(tar == end()) throw "key not found";
		return tar->second;
	}

	/**
	 * return an iterator point to the first 
	 * inserted and existed element
	 */
	iterator begin() {
		return iterator(order.tail_());
	}
	const_iterator cbegin() {
		return const_iterator(order.tail_());
	}
    /**
	 * return an iterator after the last inserted element
	 */
	iterator end() {
		return iterator(order.end());
	}
	const_iterator cend() {
		return const_iterator(order.end());
	}
  	/**
	 * if didn't contain anything, return true, 
	 * otherwise false.
	 */
	bool empty() const {
		return size_ == 0;
	}

    void clear(){
		if(cap == 0) return;
		for (int i = 0; i < cap; i++)
            data[i].clear();
		cap = 0;
		size_ = 0;
		order.clear();
	}

	size_t size() const {
		return size_;
	}
 	/**
	 * insert the value_piar
	 * if the key of the value_pair exists in the map
	 * update the value instead of adding a new element，
     * then the order of the element moved from inner of the 
     * list to the head of the list
	 * and return false
	 * if the key of the value_pair doesn't exist in the map
	 * add a new element and return true
	*/
	hash_iterator exist(const Key& key){//是否存在
		size_t v = hash(key) % cap;
		typename sjtu::double_list<addr> &pos = data[v];
		for(auto item = pos.begin();item != pos.end();item--){
			value_type &p = **item;
			if(equal(key,p.first))
			return hash_iterator(&pos,item);
		}
		return hash_iterator(&pos,pos.end());//not found
	}

	pair<iterator, bool> insert(const value_type &value) {
		if(size_ >= f * cap)  db();
		hash_iterator tmp = exist(value.first);
		if(tmp.it_ != tmp.pos->end()){//找到
			order.erase(*(tmp.it_));
			order.insert_head(value);//更新
			size_t h = hash(value.first) %cap;
			tmp.pos->erase(tmp.it_);
			tmp.pos->insert_head(order.begin());
			return pair<iterator,bool>(iterator(begin()),false);
		}
		else{
			size_++;
			order.insert_head(value);
			size_t h = hash(value.first) %cap;
			data[h].insert_head(order.begin());//
			return pair<iterator,bool>(iterator(begin()),true);
		}
	}
 	/**
	 * erase the value_pair pointed by the iterator
	 * if the iterator points to nothing
	 * throw 
	*/
	void remove(iterator pos) {
		if(pos.it == order.end()){
			throw "invalid remove";
		}
		hash_iterator tmp = exist(pos->first);
		tmp.pos->erase(tmp.it_);//从哈希表对应链表里删去迭代器
		order.erase(pos.it);
		size_--;
	}
	/**
	 * return how many value_pairs consist of key
	 * this should only return 0 or 1
	*/
	size_t count(const Key &key) {
		return find(key) != end();
	}
	/**
	 * find the iterator points at the value_pair
	 * which consist of key
	 * if not find, return the iterator 
	 * point at nothing
	*/
	iterator find(const Key &key) {
		hash_iterator tmp = exist(key);
		if(tmp.it_ == tmp.pos->end()) return end();
		return iterator(*tmp);
	}
	void db(){//double
		size_t newcap = cap * 2 + 1;
		delete[] data;
		data = new typename sjtu::double_list<addr>[newcap];
		cap = newcap;
		for (auto t = order.begin(); t != order.end(); --t){
			value_type &pr = (*t);
			data[hash(pr.first)%cap].insert_head(t);
		}
	}

};

class lru{
    using lmap = sjtu::linked_hashmap<Integer,Matrix<int>,Hash,Equal>;
    using value_type = sjtu::pair<const Integer, Matrix<int> >;
	using list= double_list<value_type>;
	using addr = list::iterator;
public:
	lmap lhm;
	int memory;
    lru(int size):memory(size),lhm(){
    }
    ~lru(){
    }
    /**
     * save the value_pair in the memory
     * delete something in the memory if necessary
    */
    void save(const value_type &v) {
		auto tmp = lhm.insert(v);
		if(lhm.size() > memory){
			lhm.remove(lhm.begin());
		}
    }
    /**
     * return a pointer contain the value
    */
    Matrix<int>* get(const Integer &v) {
		auto it = lhm.find(v);
		if (it == lhm.end())
			return nullptr;
		lhm.insert(value_type(v, it->second));
		return &(it->second);
    }
    /**
     * just print everything in the memory
     * to debug or test.
     * this operation follows the order, but don't
     * change the order.
    */
    void print(){
		for(auto i= lhm.order.begin() ;i!=lhm.order.end();i--){
			std::cout<<(*i).first.val<<" "<<(*i).second<<std::endl;
		}
    }
};
}

#endif