#pragma once
#include <cassert>


template<typename T>
class Vector;


namespace iter
{
	// Basic Iterator
	template<typename Vector>
	class Iterator
	{
	public:
		using ValueType = typename Vector::ValueType;
		using PointerType = ValueType*;
		using ReferenceType = ValueType&;
	public:
		Iterator(PointerType ptr) : ptr(ptr) {}


		ReferenceType operator[] (const size_t idx)
		{
			return *(ptr + idx);
		}
		ReferenceType operator*()
		{
			return *ptr;
		}
		PointerType operator->()
		{
			return ptr;
		}

		bool operator==(const Iterator& rhs) const
		{
			return(ptr == rhs.ptr);
		}
		bool operator!=(const Iterator& rhs) const
		{
			return !(*this == rhs);
		}


	public:
		PointerType ptr = nullptr;
	};
}

// Forwarded iterator for begin() and end()
template <typename Vector>
class Forward_Iterator : public iter::Iterator<Vector>
{
public:
	using ValueType = typename Vector::ValueType;
	using PointerType = ValueType*;
	using ReferenceType = ValueType&;
public:
	Forward_Iterator(PointerType ptr) : iter::Iterator<Vector>(ptr), ptr(ptr) {}

	Forward_Iterator& operator++()
	{
		*this = Forward_Iterator(++ptr);
		return *this;
	}
	Forward_Iterator& operator++(int)
	{
		Forward_Iterator temp = *this;
		*this = Forward_Iterator(++ptr);
		return temp;
	}
	Forward_Iterator& operator--()
	{
		*this = Forward_Iterator(--ptr);
		return *this;
	}
	Forward_Iterator& operator--(int)
	{
		Forward_Iterator temp = *this;
		*this = Forward_Iterator(--ptr);
		return temp;
	}


	// WARNIN! IT'S JUST FOR DUBUGG, MAY CAUSE A BUG!
	Forward_Iterator& operator+(const size_t num)
	{
		*this = Forward_Iterator(ptr + num);
		return *this;
	}
	
	// WARNIN! IT'S JUST FOR DUBUGG, MAY CAUSE A BUG!
	Forward_Iterator& operator-(const size_t num)
	{
		*this = Forward_Iterator(ptr - num);
		return *this;
	}

	Forward_Iterator operator=(const iter::Iterator<Vector>& rhs)
	{
		this->ptr = rhs.ptr;
		return Forward_Iterator(rhs.ptr);
	}

public:
	PointerType ptr = nullptr;
};

// Reversed iterator for rbegin() and rend()
template <typename Vector>
class Reversed_Iterator : public iter::Iterator<Vector>
{
public:
	using ValueType = typename Vector::ValueType;
	using PointerType = ValueType*;
	using ReferenceType = ValueType&;
public:
	Reversed_Iterator(PointerType ptr) : iter::Iterator<Vector>(ptr), ptr(ptr) {}

	Reversed_Iterator& operator++()
	{
		*this = Reversed_Iterator(--ptr);
		return *this;
	}
	Reversed_Iterator& operator++(int)
	{
		Reversed_Iterator temp = *this;
		*this = Reversed_Iterator(--ptr);
		return temp;
	}
	Reversed_Iterator& operator--()
	{
		*this = Reversed_Iterator(++ptr);
		return *this;
	}
	Reversed_Iterator& operator--(int)
	{
		Reversed_Iterator temp = *this;
		*this = Reversed_Iterator(++ptr);
		return temp;
	}

	// WARNIN! IT'S JUST FOR DUBUGG, MAY CAUSE A BUG!
	Reversed_Iterator& operator+(const size_t num)
	{
		*this = Reversed_Iterator(ptr - num);
	}

	// WARNIN! IT'S JUST FOR DUBUGG, MAY CAUSE A BUG!
	Reversed_Iterator& operator-(const size_t num)
	{
		*this = Reversed_Iterator(ptr + num);
	}

	Reversed_Iterator operator=(const iter::Iterator<Vector>& rhs)
	{
		this->ptr = rhs.ptr;
		return Reversed_Iterator(rhs.ptr);
	}
public:
	PointerType ptr = nullptr;
};



// vector class 
template <typename T>
class Vector
{
public:
	using ValueType = T;
	using Iterator = Forward_Iterator<Vector<T>>;   // iterator 
	using Reversed_Iterator = Reversed_Iterator<Vector<T>>;  // reversed iterator

	// Iterators
	constexpr Iterator begin() const noexcept
	{
		if (Size <= 0)  // if there's no first element in here 
			return nullptr;  // return null 

		return Iterator(data_array);  // return iterator to the first element 
	}
	constexpr Iterator end() const noexcept
	{
		if (Size <= 0)  // if there's no last element in here
			return nullptr; // return null
		return Iterator(data_array + Size);  // return pointer to last element. Ex: Size = 5, *T + 5 
	}
	constexpr Reversed_Iterator rbegin() const noexcept
	{
		if (Size <= 0)
			return nullptr;
		return Reversed_Iterator(data_array + Size - 1);
	}
	constexpr Reversed_Iterator rend() const noexcept
	{
		if (Size <= 0)
			return nullptr;
		return Reversed_Iterator(data_array);
	}
	/////////
public:
	Vector()
	{
		this->alloc();   // just allocate memmory
	}
	Vector(const T& value) noexcept
	{
		this->alloc();  // allocate memmory 
		this->push_back(value);  // and push a value 
	}
	Vector(const size_t Size_, const T& value)
	{
		this->alloc(Size_);  // allocate exact amount of memmory
		for (auto i = 0; i < Size_; ++i)  // and fill it with value 
			this->push_back(value);
	}
	Vector(const Vector& vect)
	{
		this->capacity = vect.capacity;
		this->alloc(vect.capacity);

		for (auto i(0); i < vect.Size; i++)
			new (data_array + i) T(std::move(vect.data_array[i]));  // swap data again 

		Size = vect.Size;

	} // copy constructor 
	Vector(Vector&& vect) noexcept
	{
		this->capacity = vect.capacity;

		this->alloc(vect.capacity);

		for (auto i(0); i < vect.Size; i++)
			new (this->data_array + i) T(std::move(vect.data_array[i]));  // place old data to a new temp array

		this->Size = vect.Size;
		vect.Size = 0;
		vect.capacity = 0;

		

	} // move constructor  
	~Vector()
	{
		this->clear();  // basicly clear Vector 
		if(capacity!=0)
			::operator delete(data_array, capacity * sizeof(T));  // and delete it 
	}

	void clear()
	{
		// So, it's easy to clean up for basic types, 
		// but if this vector contains complicated types, destructor should be called for every one of the elements

		for (auto i = 0; i < Size; ++i)
			data_array[i].~T();
		Size = 0;
	}

	void push_back(const T& value)
	{
		if (Size >= capacity)
			this->alloc(capacity * 2);  // need to reallocate memmory

		new (data_array + Size) T(std::move(value));  // place an addition value to the Vector
		++Size;  // increment size 
	}
	void pop_back()
	{
		--Size; 
		data_array[Size].~T();  // delete last element
	}
	void reserve(const size_t new_capacity)
	{
		if (capacity >= new_capacity)  // you don't need reserve more space
			return; 
		this->alloc(new_capacity);   // reallocate with a new capacity
	}
	void shrink_to_fit()
	{
		this->alloc(Size);  // just reallocate with a new capacity
	}

	bool is_empty()
	{
		if (Size == 0)
			return true;
		return false;
	}

	// this function will construct object in place
	template<typename... Args, typename Iter>
	Iter emplace(Iter& position, Args&&... args)
	{
		// example 
		// iter = vect.begin();
		// iter + 2;
		// emplace(iter, 10)
		// vector = { 1, 2, 3, 4, 5, 6, 7,};
		// result_vecotr = {1, 2, 10, 4, 5, 6, 7}


		if (position == nullptr)  // if argument iterator is null 
			return nullptr;
		if (*position > Size)
			return nullptr;

		if (position == Iter(data_array + Size - 1))  // emplace in back 
		{
			this->emplace_back(args...);
			return Iter(data_array + Size - 1);  // return iterator to the last element
		}
		if (position == Iter(data_array))  // emplace in front 
		{
			this->emplace_front(args...);
			return Iter(data_array);  // return iterator to the first element
		}

		// 1. check if there's place for a new element  

		if (Size >= capacity)  // need to reallocate memmory
			this->alloc(capacity * 2);

		// 2. shift all data starting from position-1 to the right by 1
		for (auto i = Size; i >= 1; --i)
		{
			new (data_array + i) T(std::move(data_array[i - 1]));  // shift all elements to the right by 1 
		}
		new(data_array + (*position)) T(std::forward<Args>(args)...);  // build element and place it
		++Size;
	}
	template<typename... Args>
	Iterator emplace_back(Args&&... args)
	{
		if (Size >= capacity)
			this->alloc(this->capacity * 2);

		new(data_array + Size) T(std::forward<Args>(args)...);
		++Size;

		return --this->end();
	}
	template<typename... Args>
	Iterator emplace_front(Args&&... args)
	{
		//check if there is place 
		if (Size >= capacity)
			this->alloc(this->capacity * 2);

		for (auto i = Size; i >= 1; --i)
		{
			new (data_array + i) T(std::move(data_array[i - 1]));  // shift all elements to the right by 1 
		}

		new(data_array) T(std::forward<Args>(args)...);  // build element and place it in front 
		++Size;

		return this->begin();   // return iterator to the first element
	}

	///////////////

	constexpr size_t Capacity() const noexcept { return capacity; }
	constexpr size_t size() const noexcept { return Size; }
	constexpr size_t max_size() const noexcept { return capacity; }



	// Overloading 

	T operator[](size_t idx)
	{
		if (idx >= Size)   // if out of range 
			assert(false);  // for debugging 

		return *(data_array + idx);  // return that value 
	}  // operator[] overloading 
	bool operator==(Vector& rhs)
	{
		if (this->size() != rhs.size())  // if different sizes, it's deffenetly not equal to each other 
			return false;

		for (auto i(0); i < this->size(); ++i)
			if (this->data_array[i] != rhs.data_array[i])  // just compair every element 
				return false;

		return true;
	}
	bool operator!=(Vector& rhs)
	{
		return !(this->operator==(rhs));  // return opposite of operator==
	}

	Vector<int>& operator=(Vector& rhs)
	{
		this->alloc(rhs.capacity());
		this->Size = rhs.size();
		for (auto i = 0; i < rhs.capacity(); ++i)
			new (this->data_array + i) T(std::move(rhs.data_array[i]));
	}


private:
	T* data_array = nullptr; 

	size_t Size = 0;   
	size_t capacity = 0; 
private:
	void alloc(size_t new_cap = 100)
	{
		T* temp_arr = (T*)::operator new(new_cap * sizeof(T));  // allocate memmory for a temp array

		for (auto i(0); i < Size; i++)
			new (temp_arr + i) T(std::move(data_array[i]));  // place old data to a new temp array

		data_array = (T*)::operator new(new_cap * sizeof(T));  // allocate memmory for array of a new size 


		for (auto i(0); i < Size; i++)
			new (data_array + i) T(std::move(temp_arr[i]));  // swap data again 

		for (auto i = 0; i < Size; i++)  // need to delete temp array
			temp_arr[i].~T();

		temp_arr = nullptr;
		::operator delete(temp_arr, capacity * sizeof(T));

		capacity = new_cap;

	}  // This function will allocate or reallocate memmory 
};
