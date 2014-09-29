/**************************************************************************
 *	HyStaticArray.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyStaticArray_h__
#define __HyStaticArray_h__

template<intx iSize>
class HyStaticArray
{
private:
	uint32 m_Data[iSize];
	intx m_Size;

public:
	HyStaticArray() : m_Size(0) { }

public:
	// set value at position p_Pos, growing the array if necessary
	void SetAtGrow(intx p_Pos, uint32 p_Val)
	{
		HyAssert(p_Pos < iSize, "StaticArray::SetAtGrow() exceeds maxium size of " << iSize);
		if(p_Pos >= m_Size)
			m_Size = p_Pos + 1;
		m_Data[p_Pos] = p_Val;
	}

	// set count values starting at position p_Pos
	void SetAtArray(intx p_Pos, intx p_Count, uint32 p_Val)
	{
		HyAssert(p_Pos + p_Count <= m_Size, "HyStaticArray::SetAtArray() invalid arguments");
		for(intx i = 0; i < p_Count; ++i)
			m_Data[p_Pos + i] = p_Val;
	}

	// set a new array size
	void SetSize(intx p_Size)
	{
		HyAssert(p_Size <= iSize, "HyStaticArray::SetSize() invalid arguments");
		m_Size = p_Size;
	}

	// remove an element from position p_Pos
	void RemoveAt(intx p_Pos, intx p_Count)
	{
		HyAssert(p_Pos + p_Count <= m_Size, "HyStaticArray::RemoveAt() invalid arguments");
		for(intx i = 0; i < p_Count; ++i)
			m_Data[p_Pos + i] = m_Data[p_Pos + p_Count + i];
		m_Size -= p_Count;
	}

	// returns true if the array is empty
	bool IsEmpty() const { return (m_Size == 0); }

	// return the current array size
	intx GetSize() const { return m_Size; }

	// return the maximum array size
	intx GetMaxSize() const { return iSize; }

	// []-indexing operator
	uint32& operator[](intx p_Pos)				{ HyAssert(p_Pos < m_Size, "HyStaticArray::operator[] invalid arguments"); return m_Data[p_Pos]; }
	const uint32& operator[](intx p_Pos) const  { HyAssert(p_Pos < m_Size, "HyStaticArray::operator[] invalid arguments"); return m_Data[p_Pos]; }

	// return the array pointer
	uint32* GetData() { return m_Data; }
	const uint32* GetData() const  { return m_Data; }
};

#endif /* __HyStaticArray_h__ */