#ifndef __LIST_OF_OBJECTS__
#define __LIST_OF_OBJECTS__

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

class CGoodObject : public CObject
{
	DECLARE_SERIAL(CGoodObject);
public:
	CGoodObject() {};
	virtual CGoodObject *New(CGoodObject *pObject) {return NULL;};
	CGoodObject(CGoodObject *pObject) {New(pObject);};
};

class CListOfObjects : public CObList
{
	DECLARE_SERIAL(CListOfObjects);
public :
	CListOfObjects() {}; 
	CListOfObjects(CListOfObjects &ListOfObjects) {}; 
	void Clear()
	{
		for ( POSITION pos=GetHeadPosition(); pos; GetNext(pos) )
		{
			CObject *pObject=(CObject *)GetAt(pos);
			delete pObject;
		}
		RemoveAll();
	};
	virtual ~CListOfObjects() { Clear(); };
	CListOfObjects operator=(CListOfObjects &ListOfObjects)
	{
		Clear();
		*this+=ListOfObjects;
		return *this;
	};
	CListOfObjects operator+=(CListOfObjects &ListOfObjects)
	{
		CGoodObject *pOldObject, *pNewObject;
		for (POSITION pos=ListOfObjects.GetHeadPosition(); pos; ListOfObjects.GetNext(pos))
		{
			pOldObject=(CGoodObject *)ListOfObjects.GetAt(pos);
			pNewObject=pOldObject->New(pOldObject);
			AddTail(pNewObject);
		}
		return *this;
	};
	CListOfObjects operator+(CListOfObjects &ListOfObjects)
	{
		*this+=ListOfObjects;
		return *this;
	};
};

class CListOfPointers : public CPtrList
{
public :
	CListOfPointers() {}; 
	void Clear()
	{
		for ( POSITION pos=GetHeadPosition(); pos; GetNext(pos) )
			delete GetAt(pos);
		RemoveAll();
	};
	virtual ~CListOfPointers() { Clear(); };
	void *IndexToPointer(int index)
	{
		POSITION pos=FindIndex(index);
		ASSERT(pos);
		if (!pos) return NULL;
		return (void *)GetAt(pos);
	}
	void *RemoveIndex(int index)
	{
		POSITION pos=FindIndex(index);
		ASSERT(pos);
		if (!pos) return NULL;
		void *pData=(void *)GetAt(pos);
		RemoveAt(pos);
		return pData;
	}
};
#endif //__LIST_OF_OBJECTS__

