#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "ATString.h"

// A string keyed hash table
struct ATHashElement
{
	void* element;
	ATHashElement* next;
	char key[112];
};

template <class T, size_t size>
struct ATHashTable
{
public:
	ATHashTable()
		:
		m_itemCount(0)
	{
		ATMemSet(&m_elements, 0, sizeof(m_elements));
	}

	void Add(const char *key, T *value)
	{
		ATHashElement* HE = (ATHashElement*)ATAlloc(sizeof(ATHashElement));
		HE->next = 0;
		HE->element = value;
		ATString_CopyCharacters(HE->key, sizeof(HE->key), key, sizeof(HE->key));
		++m_itemCount;

		size_t index = ATString_Hash(key, size);

		ATHashElement* element = m_elements[index];
		if (!element)
		{
			m_elements[index] = HE;
			return;
		}
		while (element->next)
		{
			ATASSERT(ATString_Compare(element->key, key) != 0, "Key already exists in hash table. Call get first to ensure entry does not already exists");
			element = element->next;
		}
		element->next = HE;
	}

	void Remove(const char *key)
	{
		size_t index = ATString_Hash(key, size);

		ATHashElement* prev = 0;
		ATHashElement* element = m_elements[index];
		while (element)
		{
			if (ATString_CompareCharacters(element->key, key, sizeof(element->key)) == 0)
			{
				if (prev)
				{
					prev->next = element->next;
				}
				else
				{
					m_elements[index] = element->next;
				}
				ATFree(element);
				--m_itemCount;
				return;
			}
			prev = element;
			element = element->next;
		}
	}

	T *Get(const char *key)
	{
		size_t index = ATString_Hash(key, size);

		ATHashElement* element = m_elements[index];
		while (element)
		{
			if (ATString_Compare(element->key, key) == 0)
			{
				return (T*)element->element;
			}
			element = element->next;
		}
		return 0;
	}

	AT_FORCE_INLINE size_t Count() const
	{
		return m_itemCount;
	}

private:
	ATHashElement *m_elements[size];
	size_t m_itemCount;
};

#endif //HASH_TABLE_H
