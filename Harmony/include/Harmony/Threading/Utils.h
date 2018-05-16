//*****************************************************************************
// Description:  Util functions and classes
//*****************************************************************************
#ifndef TREADINGFX_UTILS_H
#define TREADINGFX_UTILS_H

#include "Afx/HyStdAfx.h"
#include "PlatformInterop.h"

class Utils
{
	public:
		// set the thread name for visual studio debugger to see the name in the debugging environment
		static void SetDebugThreadName(uint32 p_ThreadId, PCSTR p_ThreadName);

		// beep with time and frequency
		static void Beep(uint32 dwDurationMs, uint32 dwFreq);

		// beep for wait object errors
		static void BeepErrorWaitObject();

		// converts a string into a wstring
		static std::wstring StringToWString(const std::string& p_Str);

		// converts a wstring into a string
		static std::string WStringToString(const std::wstring& p_Str);
};

//*****************************************************************************
// Description:
//  Static Array class
// Remarks:
//  Abstracts a static array of type uint32 with a fixed, predefined
//  maximum size.
//-----------------------------------------------------------------------------
template<intx S>
class StaticUIntArray
{
	private:
		uint32 m_Data[S];
		intx m_Size;

	public:
		StaticUIntArray() : m_Size(0) { }

	public:
		// set value at position p_Pos, growing the array if necessary
		void SetAtGrow(intx p_Pos, uint32 p_Val)
		{
			ASSERT_EXPR(p_Pos < S);
			if(p_Pos >= m_Size)
				m_Size = p_Pos + 1;
			m_Data[p_Pos] = p_Val;
		}

		// set count values starting at position p_Pos
		void SetAtArray(intx p_Pos, intx p_Count, uint32 p_Val)
		{
			ASSERT_EXPR(p_Pos + p_Count <= m_Size);
			for(intx i = 0; i < p_Count; ++i)
				m_Data[p_Pos + i] = p_Val;
		}

		// set a new array size
		void SetSize(intx p_Size)
		{
			ASSERT_EXPR(p_Size <= S);
			m_Size = p_Size;
		}

		// remove an element from position p_Pos
		void RemoveAt(intx p_Pos, intx p_Count)
		{
			ASSERT_EXPR(p_Pos + p_Count <= m_Size);
			for(intx i = 0; i < p_Count; ++i)
				m_Data[p_Pos + i] = m_Data[p_Pos + p_Count + i];
			m_Size -= p_Count;
		}

		// returns true if the array is empty
		bool IsEmpty() const { return (m_Size == 0); }

		// return the current array size
		intx GetSize() const { return m_Size; }

		// return the maximum array size
		intx GetMaxSize() const { return S; }

		// []-indexing operator
		uint32& operator[](intx p_Pos) { ASSERT_EXPR(p_Pos < m_Size); return m_Data[p_Pos]; }
		const uint32& operator[](intx p_Pos) const  { ASSERT_EXPR(p_Pos < m_Size); return m_Data[p_Pos]; }

		// return the array pointer
		uint32* GetData() { return m_Data; }
		const uint32* GetData() const  { return m_Data; }
};

//*****************************************************************************
// Description:
//  reference counting value
// Remarks:
//  Simply abstracts an integer value reference count and restricts the
//  default constructor.
//  A reference count implementation class must initialize this counter,
//  otherwise a compiler error will occure.
//-----------------------------------------------------------------------------
class RefCountValue
{
	private:
		volatile intx m_Value;

		// hide default ctor
		RefCountValue();

		// ctor
	public:
		inline RefCountValue(intx p_Value) : m_Value(p_Value) {}

		// interface
	public:

		// increment reference count by one
		inline intx AddRefST()
		{
			return ++m_Value;
		}

		// decrement reference count by one
		inline intx ReleaseST()
		{
			return --m_Value;
		}

		// increment reference count by one (thread safe)
		inline intx AddRefMT()
		{
			return (intx)InteropInterlockedIncrement(m_Value);
		}

		// decrement reference count by one (thread safe)
		inline intx ReleaseMT()
		{
			return InteropInterlockedDecrement(m_Value);
		}

		// get current reference count value
		inline intx RefCount() const
		{
			return m_Value;
		}
};

// define helper macros to implement reference counting in a class
#define REFERENCE_COUNT_IMPL_ST() \
	private: \
		RefCountValue m_RefCounter;	\
	public: \
		inline intx AddRef()      { return(m_RefCounter.AddRefST()); } \
		inline intx Release()     { intx count = m_RefCounter.ReleaseST(); if(!count) delete(this); return(count); } \
		inline intx RefCount() const { return(m_RefCounter.RefCount()); }

// define helper macros to implement thread-safe reference counting in a class
#define REFERENCE_COUNT_IMPL_MT() \
	private: \
		RefCountValue m_RefCounter;	\
	public: \
		inline intx AddRef()      { return(m_RefCounter.AddRefMT()); } \
		inline intx Release()     { intx count = m_RefCounter.ReleaseMT(); if(!count) delete(this); return(count); } \
		inline intx RefCount() const { return(m_RefCounter.RefCount()); }


// shared pointer helpers to work with reference counted objects
// helper function to correctly Release ref counted objects on destruction
template<class T> struct refcounted_deleter
{
    void operator()(T * p)
    {
        if(p) (p)->Release();
    }
};

// helper function to create a new shared pointer and increasing the ref counter
template<class T>
inline std::shared_ptr<T> Make_Shared_AddRef(T * p)
{
    if(p) (p)->AddRef();
    std::shared_ptr<T> px(p, refcounted_deleter<T>());
    return px;
}

// helper function to create a new shared pointer without increasing the ref counter
template<class T>
inline std::shared_ptr<T> Make_Shared_NoAddRef(T * p)
{
    std::shared_ptr<T> px(p, refcounted_deleter<T>());
    return px;
}

// defines the shared pointer class PtrClass and two helper function PtrClassAddRef/PtrClassNoAddRef
#define DEFINE_SHARED_PTR(Class, PtrClass) \
	typedef std::shared_ptr<Class> PtrClass; \
	inline PtrClass PtrClass##NoAddRef(Class* p_Ptr) \
	{ return Make_Shared_NoAddRef(p_Ptr); } \
	inline PtrClass PtrClass##AddRef(Class* p_Ptr) \
	{ return Make_Shared_AddRef(p_Ptr); }

#endif // TREADINGFX_UTILS_H
