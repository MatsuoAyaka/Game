#pragma once
#include "main.h"
#include "Delegate.hpp"

struct IEventInfo
{

};

class EventInfo : IEventInfo
{
public:
	DelegateFunction<void> actions;

	EventInfo(void(*action)(void))
	{
		actions += action;
	}
};


template<class retType, class ...paramTypes>
class EventInfoT : IEventInfo
{
public:
	DelegateFunction<retType, paramTypes...> actions;
	Delegate<retType, paramTypes...> dActions;

	void AddListener(retType(*action)(paramTypes...))
	{
		actions += action;
	}

	EventInfoT(DelegateFunction<retType, paramTypes...> action)
	{
		actions += action;
	}

	EventInfoT(Delegate<retType, paramTypes...>& action)
	{
		dActions = action;
	}
};

class EventCenter
{
public:
	//template<typename retType, typename ...paramTypes>
	//void AddEventListener(std::string name, DelegateFunction<retType, paramTypes...> action)
	//{
	//	if (eventDic.find(name) != eventDic.end())
	//	{
	//		//((EventInfoT*)eventDic[name])->actions += action;
	//		EventInfoT<retType, paramTypes...>* t = ((EventInfoT<retType, paramTypes...>*)eventDic[name]);
	//		t->actions += action;
	//	}
	//	else
	//	{
	//		eventDic.insert({ name, (IEventInfo*)new EventInfoT<retType, paramTypes...>(action) });
	//	}
	//}

	template<typename retType, typename... paramTypes>
	void AddEventListener(std::string name, Delegate<retType, paramTypes...> action)
	{
		if (eventDic.find(name) != eventDic.end())
		{
			EventInfoT<retType, paramTypes...>* t = ((EventInfoT<retType, paramTypes...>*)eventDic[name]);
			t->dActions = action;
		}
		else
		{
			eventDic.insert({ name, (IEventInfo*)new EventInfoT<retType, paramTypes...>(action) });
		}
	}

	template<typename objType, typename retType, typename... paramTypes>
	void AddEventListener(objType* InObj, std::string name, retType(objType::* InFunc)(paramTypes...))
	{
		Delegate<retType, paramTypes...>* del = new Delegate<retType, paramTypes...>();
		del->Bind<objType>(InObj, InFunc);

		if (eventDic.find(name) != eventDic.end())
		{
			EventInfoT<retType, paramTypes...>* t = ((EventInfoT<retType, paramTypes...>*)eventDic[name]);
			delete t;
			t = new EventInfoT<retType, paramTypes...>(*del);
		}
		else
		{
			eventDic.insert({ name, (IEventInfo*)new EventInfoT<retType, paramTypes...>(*del) });
		}
	}

	template<typename retType, typename... paramTypes>
	void AddEventListener(std::string name, retType(*action)(paramTypes...))
	{
		Delegate<retType, paramTypes...>* del = new Delegate<retType, paramTypes...>();
		del->Bind(action);

		if (eventDic.find(name) != eventDic.end())
		{
			EventInfoT<retType, paramTypes...>* t = ((EventInfoT<retType, paramTypes...>*)eventDic[name]);
			delete t;
			t = new EventInfoT<retType, paramTypes...>(*del);
		}
		else
		{
			eventDic.insert({ name, (IEventInfo*)new EventInfoT<retType, paramTypes...>(*del) });
		}
	}

	void AddEventListener(std::string name, void(*action)())
	{
		if (eventDic.find(name) != eventDic.end())
		{
			((EventInfo*)eventDic[name])->actions += action;
		}
		else
		{
			eventDic.insert({ name, (IEventInfo*)new EventInfo(action) });
		}
	}
	
	template<typename retType, typename ...paramTypes>
	retType EventTrigger(std::string name, paramTypes... info)
	{
		if (eventDic.find(name) != eventDic.end())
		{
			if (eventDic[name] != nullptr)
			{
				return((EventInfoT<retType, paramTypes...>*)eventDic[name])->dActions.Execute(info...);
			}
		}
	}

	void EventTrigger(std::string name)
	{
		if (eventDic.find(name) != eventDic.end())
		{
			if (eventDic[name] != nullptr)
			{
				((EventInfo*)eventDic[name])->actions.Execute();
				//eventDic[name].actions
			}
		}
	}
	template<typename retType, typename... paramTypes>
	void RemoveEventListener(std::string name)
	{
		if (eventDic.find(name) != eventDic.end())
		{
			((EventInfoT<retType, paramTypes...>*)eventDic[name])->dActions.CheckDelete();
		}
	}
	template<typename retType, typename... paramTypes>
	void RemoveEventListener(std::string name, Delegate<retType, paramTypes...> action)
	{
		if (eventDic.find(name) != eventDic.end())
		{
			((EventInfoT<retType, paramTypes...>*)eventDic[name])->dActions.CheckDelete();
		}
	}

	


	void Clear()
	{
		eventDic.clear();
	}

private:
	std::map<std::string, IEventInfo*> eventDic;
private:
	static EventCenter* instance;
	EventCenter() {}
	EventCenter(const EventCenter&) = delete;
	EventCenter& operator=(const EventCenter&) = delete;
public:
	static EventCenter* GetInstance()
	{
		if (instance == NULL)
			instance = new EventCenter();
		return instance;
	}
};
EventCenter* EventCenter::instance = NULL;


