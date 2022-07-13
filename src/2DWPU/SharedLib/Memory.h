#pragma once
#include "Global.h"
#include "Exception.h"

/*
	This structure defines a memory - holds pointer to it
	and size of the data
*/

struct Memory
{
	byte *data;
	uint size;

	Memory()
	{
		data = 0;
		size = 0;
	}

	Memory(uint size)
	{
		this->size = size;
		this->data = new byte[size];
	}

	Memory(byte *data, uint size)
	{
		this->data = data;
		this->size = size;
	}
};

/*
	This is a stack built on top of the
	Memory structure
*/
template <typename T> class MemoryStack
{
	Memory data;
	byte *top;

	bool autofree;	// indicates if the memory should be freed when destructor is called
	bool copy;

public:
	// constructors

	// initialize with exiting memory
	/*MemoryStack(Memory data)
	{
		this->data = data;
		top = data.data;	// points to the beginning
		autofree = false;	// leave it alone
	}*/

	// automatically allocate memory
	MemoryStack(uint elements)
	{
		// allocate memory
		data.size = sizeof(T)*elements;
		data.data = new byte[data.size];

		// set the top to the beginning of the memory
		top = data.data;

		// automatically free allocated data
		autofree = true;

		copy = false;
	}

	MemoryStack(MemoryStack &ob)
	{
		*this = ob;
		copy = true;
	}

	~MemoryStack()
	{
		if(autofree && !copy)
			delete data.data;
		data.data = 0;	// just to be safe
	}

	// pushing and popping values
	void Push(T value)
	{
		// first check if  there's enough space to hold the element
		if( (data.size - (top - data.data)) < sizeof(T) )
			throw StackException(StackOverflow);

		// everything's fine, so just push the element
		*((T*)top) = value;
		top += sizeof(T);
	}

	T Pop()
	{
		// first check if there's any data left
		if(top == data.data)
			throw StackException(StackUnderflow);

		// everything is okay, pop the element now
		top -= sizeof(T);
		T temp = *((T*)top);

		return temp;
	}

	T Top()
	{
		// first, check if there are any data
		if(top == data.data)
			throw StackException(StackUnderflow);

		return *((T*)(top-sizeof(T)));
	}

	void Reset()
	{
		SetAddr(0);
	}

	// manually setting the address and reading
	void SetAddr(uint addr)
	{
		if(addr*sizeof(T) > data.size)
			throw StackException(StackInvalidAddress);

		// set the address
		top = data.data + (addr*sizeof(T));
	}

	// get the current address
	uint GetAddr()
	{
		return (top-data.data)/sizeof(T);
	}
};