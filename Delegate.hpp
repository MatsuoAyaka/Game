#pragma once
#include "main.h"

template<class retType, class ...paramTypes>
class DelegateBase
{
public:
	virtual retType Execute(paramTypes ...params) = 0;
};

template<class objectType, class retType, class ...paramsType>
class DelegateObject : public DelegateBase<retType, paramsType...>
{
public:
	DelegateObject(objectType* Inobj, retType(objectType::*InFunc)(paramsType...)) : objPtr(Inobj), bindFunc(InFunc){}
	virtual retType Execute(paramsType... params)
	{
		return (objPtr->*bindFunc)(params...);
	}

	bool CheckObjPtr(objectType* obj)
	{
		return (obj == objPtr);
	}
	
private:
	objectType* objPtr;
	retType(objectType::* bindFunc)(paramsType...);
};

template<class retType, class ...paramTypes>
class DelegateFunction : public DelegateBase<retType, paramTypes...>
{
public:
	DelegateFunction() : bindFunc(nullptr) {}
	DelegateFunction(retType(*InFunc)(paramTypes...)) : bindFunc(InFunc) {}
	virtual retType Execute(paramTypes...params)
	{
		return (*bindFunc)(params...);
	}

	void operator += (retType(*func)(paramTypes...))
	{
		bindFunc = func;
	}

	void operator += (DelegateFunction<retType, paramTypes...> action)
	{
		bindFunc = action.bindFunc;
	}
private:
	retType(*bindFunc)(paramTypes...);
};

//struct testobj
//{
//	std::string RunDelegate(int val, std::string str)
//	{
//		int v = val;
//		std::string s = str;
//		return s;
//	}
//};

//std::string testfunction(int val, const char* str)
//{
//	//int v = val;
//	//std::string s = str;
//
//	std::string tempstr(str);
//	return tempstr;
//}


template<class retType, class ...paramTypes>
void operator+= (DelegateFunction<retType, paramTypes...>& action1, DelegateFunction<retType, paramTypes...>& action2)
{
	action1.bindFunc = action2.bindFunc;
}

template<class retType, class... paramTypes>
class Delegate
{
public:
	Delegate() : delegatePtr(nullptr) {};

	template<typename objType>
	void Bind(objType* InObj, retType(objType::* InFunc)(paramTypes...))
	{
		CheckDelete();
		delegatePtr = new DelegateObject<objType, retType, paramTypes...>(InObj, InFunc);
	}

	void Bind(retType(*InFunc)(paramTypes...))
	{
		CheckDelete();
		delegatePtr = new DelegateFunction<retType, paramTypes...>(InFunc);
	}

	void CheckDelete()
	{
		if (delegatePtr)
		{
			delete delegatePtr;
			delegatePtr = nullptr;
		}
	}

	template<typename objType>
	bool CheckObjPtr(objType* InObj)
	{
		return ((DelegateObject<objType, retType, paramTypes...>*)delegatePtr)->CheckObjPtr(InObj);
	}

	retType Execute(paramTypes... params)
	{
		return delegatePtr->Execute(params...);
	}

private:
	DelegateBase<retType, paramTypes...>* delegatePtr;
	
};

template<class retType, class ...paramTypes>
class MultiCastDelegate
{
public:
	typedef Delegate<retType, paramTypes...> DelegateType;
	MultiCastDelegate() {}

	template<class objType>
	void Add(objType* InObj, retType(objType::* InFunc)(paramTypes...))
	{
		delegateContainer.push_back(DelegateType());
		DelegateType& InDelegate = delegateContainer.back();
		InDelegate.Bind(InObj, InFunc);
	}

	void Add(retType(*InFunc)(paramTypes...))
	{
		delegateContainer.push_back(DelegateType());
		DelegateType& InDelegate = delegateContainer.back();
		InDelegate.Bind(InFunc);
	}

	void Remove()
	{
		for (auto& action : delegateContainer)
		{
			action.CheckDelete();
		}
	}

	template<class objType>
	void Execute(objType* InObj)
	{
		for (auto& delegate : delegateContainer)
		{
			if (delegate.InObj);
		}
	}

	void BroadCast(paramTypes... info)
	{
		for (auto& elem : delegateContainer)
		{
			elem.Execute(info...);
		}
	}

private:
	vector<Delegate<retType, paramTypes...>> delegateContainer;
};
