#ifndef CLAUTEXT_H
#define CLAUTEXT_H

#include "parser.h"

#include "small_map.h" // using vector, arr
#include "array_stack.h"

namespace wiz {
	template <class T, class S>
	using SMALL_MAP = wiz::SmallMap<T, S>; // array_map with std::vector

	template <class T>
	using ARRAY_STACK = wiz::ArrayStack<T>;
}

namespace wiz {
	class EventInfo
	{
	public:
		wiz::UserType* eventUT;
		wiz::ARRAY_STACK< wiz::UserType* > nowUT; //
		wiz::ARRAY_STACK<int> userType_idx;
		wiz::SMALL_MAP<std::string, std::string> parameters;
		wiz::SMALL_MAP<std::string, std::string> locals;
		std::string id; //
		wiz::ARRAY_STACK<std::string> conditionStack;
		wiz::ARRAY_STACK<int> state;
		std::string return_value;
		std::string option;
	public:
		EventInfo() : eventUT(nullptr), return_value("")
		{

		}
	};

	class ExcuteData
	{
	public:
		wiz::UserType* pEvents;
		EventInfo info; // chk!
		bool chkInfo;

		long long depth;

		bool noUseInput;
		bool noUseOutput;
	public:
		explicit ExcuteData()
			: pEvents(nullptr), chkInfo(false), depth(0),
			noUseInput(false), noUseOutput(false)
		{
			//
		}
	};


	class Option
	{
	public:
		std::string* module_value = nullptr;

		wiz::MAP<std::string, std::pair<std::vector<std::string>, bool>>* _map = nullptr; // todo - fixed max size? and rename
		wiz::ARRAY_STACK<EventInfo>* eventStack = nullptr;
		wiz::MAP<std::string, int>* convert = nullptr;
		std::vector<wiz::UserType*>* _events = nullptr;
		wiz::UserType* events = nullptr;
		wiz::UserType* Main = nullptr;
		wiz::StringBuilder* builder = nullptr;

	public:
		virtual ~Option()
		{
			if (_map) {
				delete _map;
			}
			if (eventStack) {
				delete eventStack;
			}
			if (convert) {
				delete convert;
			}
			if (_events) {
				delete _events;
			}
			if (events) {
				delete events;
			}
			if (Main) {
				delete Main;
			}
			if (builder) {
				delete builder;
			}
		}
	};

	namespace load_data {
		class EXCUTE_MODULE_BASE
		{
		public:
			virtual std::string excute_module(const std::string& MainStr,
				wiz::UserType* _global, const ExcuteData& excuteData, Option& opt, int chk)
			{
				return ""; // error!
			}
		};
	}

	class ClauText : public wiz::load_data::EXCUTE_MODULE_BASE {
	public:
		ClauText(const ClauText&) = delete;
		ClauText& operator=(const ClauText&) = delete;

		ClauText()
		{
			//
		}
		virtual ~ClauText()
		{
			//
		}

	public:

		virtual std::string excute_module(const std::string& MainStr, wiz::UserType* _global, const ExcuteData& excuteData, Option& option, int chk);
	};
}

inline wiz::ClauText clauText;


#endif
