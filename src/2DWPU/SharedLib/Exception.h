#pragma once
#include "Global.h"
#include <string>


// Very basic exception
class Exception
{
	std::string msg;	// the message
	uint code;	// error message code

protected:
	void Append(std::string app)
	{
		msg += app;
	}

public:
	Exception(const char *msg, uint code)
	{
		this->msg = msg;
		this->code = code;
	}

	uint GetCode() { return code; }
	const char *GetMsg() { return msg.c_str(); }
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
