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
	//DelegateFunction<retType, paramTypes...> actions;
	vector<Delegate<retType, paramTypes...>*> actions;
	Delegate<retType, paramTypes...> dActions;

	/*void AddListener(retType(*action)(paramTypes...))
	{
		actions += action;
	}

	EventInfoT(DelegateFunction<retType, paramTypes...> action)
	{
		actions += action;
	}*/

	EventInfoT(Delegate<retType, paramTypes...>& action)
	{
		dActions = action;
	}
};

template<class objType, class retType, class ...paramTypes>
class MultiEventInfoT : IEventInfo
{
private:
	MultiCastDelegate<retType, paramTypes...> actions;
public:
	void AddListener(objType* InObj, retType(objType::* InFunc)(paramTypes...))
	{
		actions.Add(InObj, InFunc);
	}

	void RemoveListener()
	{
		actions.Remove();
	}

	void BroadCast(paramTypes... info)
	{
		actions.BroadCast(info...);
	}

	MultiEventInfoT(MultiCastDelegate<retType, paramTypes...>& action)
	{
		actions = action;
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
			t->actions.push_back(del);
			//delete t;
			//t = new EventInfoT<retType, paramTypes...>(*del);
		}
		else
		{
			eventDic.insert({ name, (IEventInfo*)new EventInfoT<retType, paramTypes...>(*del) });
			EventInfoT<retType, paramTypes...>* t = ((EventInfoT<retType, paramTypes...>*)eventDic[name]);
			t->actions.push_back(del);
		}
	}

	template<typename objType, typename retType, typename... paramTypes>
	void AddMultiEventListener(objType* InObj, std::string name, retType(objType::* InFunc)(paramTypes...))
	{
		if (eventDic.find(name) != eventDic.end())
		{
			((MultiEventInfoT<objType, retType, paramTypes...>*)eventDic[name])->AddListener(InObj, InFunc);
		}
		else
		{
			MultiCastDelegate<retType, paramTypes...>* del = new MultiCastDelegate<retType, paramTypes...>();
			del->Add(InObj, InFunc);
			eventDic.insert({ name, (IEventInfo*)new MultiEventInfoT<objType, retType, paramTypes...>(*del) });
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

	template<typename objType, typename retType, typename ...paramTypes>
	retType EventTrigger(objType* obj, std::string name, paramTypes... info)
	{
		if (eventDic.find(name) != eventDic.end())
		{
			if (eventDic[name] != nullptr)
			{
				for (auto& del : (((EventInfoT<retType, paramTypes...>*)eventDic[name])->actions))
				{
					if (del->CheckObjPtr<objType>(obj))
						return del->Execute(info...);
				}
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

	template<typename objType, typename retType, typename ...paramTypes>
	retType MultiEventTrigger(std::string name, paramTypes... info)
	{
		if (eventDic.find(name) != eventDic.end())
		{
			if (eventDic[name] != nullptr)
			{
				return((MultiEventInfoT<objType, retType, paramTypes...>*)eventDic[name])->BroadCast(info...);
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

	template<typename objType, typename retType, typename... paramTypes>
	void RemoveEventListener(objType* obj, std::string name)
	{
		if (eventDic.find(name) != eventDic.end())
		{
			vector<Delegate<retType, paramTypes...>*>& actions = ((EventInfoT<retType, paramTypes...>*)eventDic[name])->actions;
			for (auto iter = actions.begin(); iter != actions.end(); ++iter)
			{
				if ((*iter)->CheckObjPtr<objType>(obj))
				{
					(*iter)->CheckDelete();
					actions.erase(iter);
					break;
				}
			}

			//for (auto& del : actions)
			//{
			//	if (del->CheckObjPtr<objType>(obj))
			//	{
			//		del->CheckDelete();
			//		actions.erase()
			//		break;
			//	}
			//}
			
		}
	}

	template<typename objType, typename retType, typename... paramTypes>
	void RemoveMultiEventListener(std::string name)
	{
		if (eventDic.find(name) != eventDic.end())
		{
			((MultiEventInfoT<objType, retType, paramTypes...>*)eventDic[name])->RemoveListener();
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


