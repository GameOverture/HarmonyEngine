//*****************************************************************************
// Description:  Defines a few exceptions and macros for error handling
//*****************************************************************************
#ifndef THREADINGFX_ERROR_H
#define THREADINGFX_ERROR_H

#if defined(HY_PLATFORM_WINDOWS)
class WinException : public std::exception
{
	public:
		WinException(DWORD p_LastError) : m_LastError(p_LastError) { }
	private:
		DWORD m_LastError;
};

#define THROW_LAST_WIN_ERROR() throw WinException(InteropGetLastError())
#define THROW_ERROR_LASTERROR() THROW_LAST_WIN_ERROR()

#elif defined(HY_PLATFORM_UNIX)

class UnixException : public std::exception
{
	public:
		UnixException(int p_ErrNo) : m_ErrNo(p_ErrNo) { }
	private:
		int m_ErrNo;
};

#define THROW_LAST_UNIX_ERROR() throw UnixException(InteropGetLastError())
#define THROW_UNIX_ERROR(err) throw UnixException(err)
#define THROW_ERROR_LASTERROR() THROW_LAST_UNIX_ERROR()

#endif


class CheckFailedException : public std::exception
{
	public:
		CheckFailedException() { }	
};

class NotSupportedException : public std::exception
{
	public:
		NotSupportedException() { }	
};

class OutOfResourcesException : public std::exception
{
	public:
		OutOfResourcesException() { }	
};

class InvalidArgumentException : public std::exception
{
	public:
		InvalidArgumentException() { }	
};

#define THROW_ERROR_NOTSUPPORTED() throw NotSupportedException()
#define THROW_ERROR_OUTOFRESOURCES() throw OutOfResourcesException()
#define THROW_ERROR_INVALIDARGUMENT() throw InvalidArgumentException()

// check expression macro for verifying function results
#define CHECK_EXPR(expr) if(!(expr)) throw CheckFailedException();

#endif // THREADINGFX_ERROR_H
