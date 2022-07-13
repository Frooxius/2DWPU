#pragma once
#include "Global.h"


// Very basic exception
class Exception
{
	const char *msg;	// the message
	uint code;	// error message code

public:
	Exception(const char *msg, uint code)
	{
		this->msg = msg;
		this->code = code;
	}

	uint GetCode() { return code; }
	const char *GetMsg() { return msg; }
};

// Stack exception
enum EStackException
{
	StackUnderflow,
	StackOverflow,
	StackInvalidAddress,
	StackMemoryNullPtr
};

// and the messages
extern const char *StrStackException[];

class StackException : private Exception
{
public:
	StackException(EStackException exception) : 
	  Exception(StrStackException[(uint)exception], (uint)exception)
	{	}

	  EStackException GetCode() { 
		  return (EStackException)Exception::GetCode(); }
	  const char *GetMsg() { return Exception::GetMsg(); }
};
