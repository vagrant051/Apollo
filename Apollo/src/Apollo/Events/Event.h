#pragma once

#include "pch.h"
#include "Apollo/core.h"

namespace Apollo 
{
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication          = BIT(0),
		EventCategoryInput                = BIT(1),
		EventCategoryKeyboard             = BIT(2),
		EventCategoryMouse                = BIT(3),
		EventCategoryMouseButton          = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class APOLLO_API Event
	{
	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;	// 声明function，接受返回类型bool，参数是T&的函数
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}
		template<typename T>
		bool Dispatch(EventFn<T> func)				// function参数接收函数指针
		{
			if (m_Event.GetEventType() == T::GetStaticType())	// 拦截的事件和想处理的事件类型是否匹配
			{
				m_Event.Handled = func(*(T*)&m_Event);		// 处理拦截的事件
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;								// 拦截的事件
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

}
