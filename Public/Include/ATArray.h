#ifndef AT_ARRAY_H
#define AT_ARRAY_H

template <class T> struct ATArray
{
	ATArray()
		:
		m_itemCount(0),
		m_currentItemMax(0),
		m_data(0)
	{
	}

	ATArray(size_t initialSize)
		:
		m_itemCount(0),
		m_currentItemMax(initialSize)
	{
		m_data = (T*)ATAlloc(sizeof(T) * m_currentItemMax);
	}

	~ATArray()
	{
		RemoveAll();
	}

	AT_FORCE_INLINE T& operator [](size_t index)
	{
		return m_data[index];
	}

	void operator=(const ATArray<T>& list)
	{
		if (m_currentItemMax < list.m_itemCount)
		{
			RemoveAll();
			m_data = (T*)ATAlloc(sizeof(T) * list.m_currentItemMax);
			m_currentItemMax = list.m_currentItemMax;
		}
		else
		{
			Clear();
		}
		m_itemCount = list.m_itemCount;
		for (size_t i = 0; i < m_itemCount; ++i)
		{
			m_data[i] = T(list.m_data[i]);
		}
	}

	void ReserveAdditionalSpace()
	{
		if (m_currentItemMax <= 0)
		{
			m_currentItemMax = 4;
			m_data = (T*)ATAlloc(sizeof(T) * m_currentItemMax);
			ATMemSet(m_data, 0, sizeof(T) * m_currentItemMax);
		}
		else
		{
			size_t oldSize = m_currentItemMax * sizeof(T);
			m_currentItemMax *= 2;
			T* newMem = (T*)ATAlloc(sizeof(T) * m_currentItemMax);
			ATMemCopy(newMem, m_data, oldSize);
			ATFree(m_data);
			m_data = newMem;
		}
	}

	void Add(const T& element)
	{
		if (m_itemCount == m_currentItemMax)
		{
			ReserveAdditionalSpace();
		}
		m_data[m_itemCount] = element;
		++m_itemCount;
	}

	void Remove(size_t index)
	{
		m_data[index].~T();
		ATMemMove(&m_data[index], &m_data[index + 1], (--m_itemCount - index) * sizeof(T));
	}

	void Clear()
	{
		for (size_t i = 0; i < m_itemCount; ++i)
		{
			m_data[i].~T();
		}
		m_itemCount = 0;
	}

	void RemoveAll()
	{
		Clear();
        if (m_data != nullptr)
		{
			ATFree(m_data);
            m_data = nullptr;
		}
		m_currentItemMax = 0;
	}

	AT_FORCE_INLINE size_t Count() const
	{
		return m_itemCount;
	}

private:
	size_t m_itemCount;
	size_t m_currentItemMax;
	T *m_data;
};

#endif // AT_ARRAY_H
