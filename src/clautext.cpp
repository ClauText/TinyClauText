#include "clautext.h"

namespace wiz {

	template <class T> // T has clear method, default constructor.
	class Node
	{
	public:
		T* data;
		Node* left;
		Node* right;
	public:
		static T* f(T* ptr = nullptr) {
			T* result = nullptr;
			if (nullptr == ptr) {
				result = new T();
			}
			else {
				ptr->clear();
				result = ptr;
			}

			return result;
		}
	};

	static std::string Find(wiz::UserType* ut, const std::string& str, StringBuilder* builder)
	{ // std::string ´ë½Å vector<std::string> ??
		int count = 0;
		int idx = -1;
		for (int i = str.size() - 1; i >= 0; --i) {
			if ('/' == str[i]) {
				if (count == 0) {
					idx = i;
				}
				count++;
			}
		}

		std::string result;

		if (count == 1)
		{
			return "";
		}
		else {
			auto x = wiz::UserType::Find(ut,
				wiz::String::substring(str, 0, idx), builder);
			if (x.first == false) { return ""; }
			for (int i = 0; i < x.second.size(); ++i) {
				std::string itemName = wiz::String::substring(str, idx + 1);
				if (wiz::String::startsWith(itemName, "$it") && itemName.size() >= 4) {
					int itemIdx = stoi(wiz::String::substring(itemName, 3));

					result += ToString(x.second[i]->GetItemList(itemIdx).Get(0));
					if (i < x.second.size() - 1) {
						result += "\n";
					}
				}
				else {
					if (itemName == "_") {
						itemName = "";
					}
					auto temp = x.second[i]->GetItem(itemName);
					if (!temp.empty()) {
						for (int j = 0; j < temp.size(); ++j) {
							result += ToString(temp[j].Get(0));
							if (j < temp.size() - 1) {
								result += " ";
							}
						}
						if (i < x.second.size() - 1) {
							result += "\n";
						}
					}
				}
			}
		}
		return result;
	}



	std::string FindParameters(const wiz::SMALL_MAP<std::string, std::string>& parameters, const std::string& operand)
	{
		wiz::SMALL_MAP<std::string, std::string>::const_iterator x;
		for (int i = 0; i < parameters.size(); ++i) {
			if (wiz::String::startsWith(operand, "$parameter.")
				&& (x = parameters.find(wiz::String::substring(operand, 11))) != parameters.end()) {
				return x->second;
			}
		}
		return "";
	}
	inline std::string FindLocals(const wiz::SMALL_MAP<std::string, std::string>& locals, const std::string& operand)
	{
		if (wiz::String::startsWith(operand, "$local.") && locals.end() != locals.find(wiz::String::substring(operand, 7)))
		{
			return locals.at(wiz::String::substring(operand, 7));
		}
		return "";
	}

	bool operation(wiz::UserType* now, wiz::UserType& global, const WIZ_STRING_TYPE& str,
		wiz::ArrayStack<WIZ_STRING_TYPE>& operandStack, const ExcuteData& excuteData, wiz::StringBuilder* builder)
	{
		if (!operandStack.empty() && operandStack.top() == "ERROR") {
			return false;
		}

		int operandNum = 0;
		int count = 0; // for brace!
		{
			for (int i = operandStack.size() - 1; i >= 0; --i) {
				if ("{" == operandStack[i]) { // option.Left
					count++;
				}
				if ("}" == operandStack[i]) { // option.Right
					count--;
					if (count < 0) {
						break;
					}
					else {
						operandNum++;
					}
				}
				else {
					operandNum++;
				}
			}
		}

		for (int i = 0; i < operandNum; ++i) {
			if ("_" == operandStack[operandStack.size() - 1 - i]) {
				operandStack[operandStack.size() - 1 - i] = "";
			}
		}

		if (1) {
			//
		}
		else {
			if (wiz::String::startsWith(str.ToString(), "$") && str.ToString().size() >= 2) {
				std::cout << "no exist in load-data " << str.ToString() << std::endl;
				return false;
			}
			return true;
		}
		return true;
	}



	std::string _Convert(wiz::UserType& global, const wiz::SMALL_MAP<std::string, std::string>& parameters, std::string&& temp,
		const EventInfo& info, wiz::StringBuilder* builder) /// has bug!
	{
		wiz::StringTokenizer tokenizer(std::move(temp), std::vector<std::string>{ "/" }, builder, 1);
		std::vector<std::string> tokenVec;
		std::string result;

		tokenVec.reserve(tokenizer.countTokens());
		while (tokenizer.hasMoreTokens()) {
			tokenVec.push_back(tokenizer.nextToken());
		}

		for (int i = 0; i < tokenVec.size(); ++i)
		{
			result += "/";
			if (wiz::String::startsWith(tokenVec[i], "$parameter.")) {
				int last = -1;
				for (int j = 0; j < tokenVec[i].size(); ++j)
				{
					if (wiz::isWhitespace(tokenVec[i][j]) || tokenVec[i][j] == '{' || tokenVec[i][j] == '}' || tokenVec[i][j] == '=') {
						last = j - 1;
						break;
					}
				}
				if (last != -1)
				{
					std::string _temp = FindParameters(parameters, wiz::String::substring(tokenVec[i], 0, last));

					if (!_temp.empty()) {
						tokenVec[i] = wiz::String::replace(wiz::String::substring(tokenVec[i], 0, last), wiz::String::substring(tokenVec[i], 0, last), std::move(_temp))
							+ wiz::String::substring(tokenVec[i], last + 1);
					}
				}
				else
				{
					std::string _temp = FindParameters(parameters, tokenVec[i]);
					if (!_temp.empty()) {
						tokenVec[i] = std::move(_temp);
					}
				}
			}
			else if (wiz::String::startsWith(tokenVec[i], "$local.")) {
				int last = -1;
				for (int j = 0; j < tokenVec[i].size(); ++j)
				{
					if (wiz::isWhitespace(tokenVec[i][j]) || tokenVec[i][j] == '{' || tokenVec[i][j] == '}' || tokenVec[i][j] == '=') {
						last = j - 1;
						break;
					}
				}
				if (last != -1)
				{
					std::string _temp = FindLocals(info.locals, wiz::String::substring(tokenVec[i], 0, last));

					if (!_temp.empty()) {
						tokenVec[i] = wiz::String::replace(wiz::String::substring(tokenVec[i], 0, last), wiz::String::substring(tokenVec[i], 0, last), std::move(_temp))
							+ wiz::String::substring(tokenVec[i], last + 1);
					}
				}
				else
				{
					std::string _temp = FindLocals(info.locals, tokenVec[i]);
					if (!_temp.empty()) {
						tokenVec[i] = std::move(_temp);
					}
				}
			}

			result += std::move(tokenVec[i]);
		}
		return result;
	}
	std::string Convert(wiz::UserType* now, wiz::UserType& global, const std::string& temp, const ExcuteData& excuteData,
		wiz::StringBuilder* builder)
	{
		std::string result = temp;

		if (false == result.empty() && ('^' == result[0])) { // for pass ToBool4
			result.erase(result.begin());
			return result;
		}

		wiz::ArrayStack<std::string> resultStack;
		//wiz::UserType ut;
		bool chk = false;
		int count_change = 0;

		bool flag_A = false;
		if (result.size() > 1 && result[0] == '/')
		{
			flag_A = true;
		}
		bool flag_B = false;
		for (int i = 0; i < result.size(); ++i) {
			if (result[i] == '/') {
				flag_B = true;
				break;
			}
		}
		if (flag_B) {
			result = _Convert(global, excuteData.info.parameters, std::move(result), excuteData.info, builder);
		}
		if (result.empty()) { return ""; }
		if (!flag_A && flag_B) {
			result = std::string(result.c_str() + 1, result.size() - 1);
		}
		//wiz::load_data::LoadDataFromString(result, ut);
		//result = ut.ToString();
		if (result.empty()) { return result; }

		//if (ut.empty()) {
		//	return "";
		//}
		//	if (ut.GetUserTypeListSize() == 0 && ut.GetItemListSize() == 1) /// chk
		if (!flag_B) { // chk
			if ('/' == result[0] && result.size() > 1)
			{
				std::string _temp = Find(&global, result, builder);

				if (!_temp.empty()) {
					result = std::move(_temp);
					return result;
				}
			}
			else if (wiz::String::startsWith(result, "$local.")) {
				std::string _temp = FindLocals(excuteData.info.locals, result);
				if (!_temp.empty()) {
					result = std::move(_temp);
					return result;
				}
			}
			else if (wiz::String::startsWith(result, "$parameter.")) {
				std::string _temp = FindParameters(excuteData.info.parameters, result);
				if (!_temp.empty()) {
					result = std::move(_temp);
					return result;
				}
			}

		}

		std::vector<std::string> tokenVec;
		tokenVec.reserve(result.size());

		{
			wiz::StringTokenizer tokenizer(std::move(result), { " ", "\n", "\t", "\r" }, builder, 1); // , "{", "=", "}" }); //

			tokenVec.reserve(tokenizer.countTokens());
			while (tokenizer.hasMoreTokens()) {
				tokenVec.push_back(tokenizer.nextToken());
			}


			for (int i = tokenVec.size() - 1; i >= 0; --i)
			{
				std::string before = tokenVec[i];
				if ('/' == tokenVec[i][0] && tokenVec[i].size() > 1)
				{
					std::string _temp = Find(&global, tokenVec[i], builder);

					if ("" != _temp) {
						tokenVec[i] = std::move(_temp);
					}
				}
				else if (wiz::String::startsWith(tokenVec[i], "$local.")) { // && length?
					std::string _temp = FindLocals(excuteData.info.locals, tokenVec[i]);
					if (!_temp.empty()) {
						tokenVec[i] = std::move(_temp);
					}
				}
				else if (wiz::String::startsWith(tokenVec[i], "$parameter.")) { // && length?
					std::string _temp = FindParameters(excuteData.info.parameters, tokenVec[i]);
					if (!_temp.empty()) {
						tokenVec[i] = std::move(_temp);
					}
				}
			}

		}

		wiz::ArrayStack<WIZ_STRING_TYPE> operandStack;
		wiz::ArrayStack<std::string> operatorStack;

		operandStack.reserve(tokenVec.size());
		operatorStack.reserve(tokenVec.size());

		for (int i = tokenVec.size() - 1; i >= 0; --i) {
			// todo - chk first? functions in Event
			if (String::startsWith(tokenVec[i], "$parameter.") ||
				String::startsWith(tokenVec[i], "$.") ||
				String::startsWith(tokenVec[i], "$local.") ||
				'$' != tokenVec[i][0] || ('$' == tokenVec[i][0] && tokenVec[i].size() == 1)
				) {
				operandStack.push(tokenVec[i]);
			}
			else
			{
				// cout << tokenVec[i] << endl;
				operandStack.pop(); // =
				operandStack.pop(); // {
				operatorStack.push(tokenVec[i]);

				if (false == operation(now, global, tokenVec[i], operandStack, excuteData, builder)) // chk!!
				{
					exit(-1);
				}

				operandStack[operandStack.size() - 2] = operandStack[operandStack.size() - 1];
				operandStack.pop(); // } 
			}
		}


		{
			builder->Clear();

			for (int i = operandStack.size() - 1; i >= 0; --i) {
				{
					//result = result + temp + " "; 
					// temp -> strVec
					builder->Append(operandStack[i].ToString().c_str(), operandStack[i].ToString().size());
					builder->Append(" ", 1);
				}
			}
		}
		result = std::string(builder->Str(), builder->Size());
		if (!result.empty()) {
			result.erase(result.begin() + result.size() - 1);
		}

		return std::move(result);
	}

	std::string ClauText::excute_module(const std::string& MainStr, wiz::UserType* _global, const ExcuteData& excuteData, Option& opt, int chk)
	{
		wiz::MAP<std::string, std::pair<std::vector<std::string>, bool>>* __map = opt._map;
		opt._map = Node<wiz::MAP<std::string, std::pair<std::vector<std::string>, bool>>>::f(__map);
		wiz::MAP<std::string, std::pair<std::vector<std::string>, bool>>& _map = *opt._map;
		//
		wiz::UserType& global = *_global;

		std::string* _module_value = opt.module_value;
		opt.module_value = Node<std::string>::f(_module_value);
		std::string& module_value = *opt.module_value;

		// data, event load..
		wiz::ArrayStack<EventInfo>* _eventStack = opt.eventStack;
		opt.eventStack = Node<wiz::ArrayStack<EventInfo>>::f(_eventStack);
		wiz::ArrayStack<EventInfo>& eventStack = *opt.eventStack;

		wiz::MAP<std::string, int>* _convert = opt.convert;
		opt.convert = Node<wiz::MAP<std::string, int>>::f(_convert);
		wiz::MAP<std::string, int>& convert = *opt.convert;

		std::vector<wiz::UserType*>* __events = opt._events;
		opt._events = Node<std::vector<wiz::UserType*>>::f(__events);
		std::vector<wiz::UserType*>& _events = *opt._events;

		wiz::UserType* _events_ = opt.events;
		opt.events = Node<wiz::UserType>::f(_events_);
		wiz::UserType& events = *opt.events;

		wiz::UserType* eventPtr = nullptr;

		wiz::UserType* _Main = opt.Main;
		opt.Main = Node<wiz::UserType>::f(_Main);
		wiz::UserType& Main = *opt.Main;

		wiz::StringBuilder* _builder = opt.builder;
		opt.builder = Node<wiz::StringBuilder>::f(_builder);
		wiz::StringBuilder& builder = *opt.builder;


		// start from Main.
		if (excuteData.chkInfo == false) { /// chk smartpointer.
			if (global.GetUserTypeItem("Main").empty() && MainStr.empty())
			{
				std::cout << "do not exist Main" << std::endl;
				return "ERROR -1";
			}

			wiz::UserType* _Main = nullptr;

			if (MainStr.empty()) {
				_Main = global.GetCopyUserTypeItem("Main")[0];
				Main.LinkUserType(_Main);
				global.RemoveUserTypeList("Main");
			}
			else {
				wiz::LoadDataFromString(MainStr, Main);
			}


			EventInfo info;
			info.eventUT = Main.GetUserTypeList(0);
			info.userType_idx.push(0);
			std::pair<std::string, std::string> id_data =
				std::make_pair<std::string, std::string>("id", wiz::ToString(info.eventUT->GetUserTypeItem("$call")[0]->GetItem("id")[0].Get(0)));
			info.parameters.insert(
				id_data
			);
			info.id = info.parameters["id"];

			eventStack.push(info);

			if (nullptr == excuteData.pEvents) {
				_events = global.GetCopyUserTypeItem("Event");
				for (int i = 0; i < _events.size(); ++i) {
					events.LinkUserType(_events[i]);
				}
				global.RemoveUserTypeList("Event");
				eventPtr = &events;
			}
			else {
				eventPtr = excuteData.pEvents;
			}

			// event table setting
			for (int i = 0; i < eventPtr->GetUserTypeListSize(); ++i)
			{
				auto x = eventPtr->GetUserTypeList(i)->GetItem("id");
				if (!x.empty()) {
					//std::cout <<	x[0] << std::endl;
					auto temp = std::pair<std::string, int>(wiz::ToString(x[0].Get(0)), i);
					convert.insert(temp);
				}
				else {
					// error
				}
			}

			const int no = convert.at(info.id);
			for (int i = 0; i < eventPtr->GetUserTypeList(no)->GetUserTypeListSize(); ++i) {
				if (eventPtr->GetUserTypeList(no)->GetUserTypeList(i)->GetName() == "$local") {
					for (int j = 0; j < eventPtr->GetUserTypeList(no)->GetUserTypeList(i)->GetItemListSize(); ++j) {
						std::string name = wiz::ToString(eventPtr->GetUserTypeList(no)->GetUserTypeList(i)->GetItemList(j).Get(0));
						std::string value = "";
						std::pair<std::string, std::string> temp(name, value);
						info.locals.insert(temp);
					}
					break;
				}
			}
		}
		else {
			eventStack.push(excuteData.info);
		}

		// main loop
		while (!eventStack.empty())
		{
			// 
			EventInfo info = eventStack.top();
			std::string str;
			wiz::SMALL_MAP<std::string, std::string>::iterator x;
			for (int i = 0; i < info.parameters.size(); ++i) {
				if ((x = info.parameters.find("id")) != info.parameters.end()) {
					str = x->second;
					break;
				}
			}

			int no = convert.at(str);

			int state = 0;

			if (info.userType_idx.size() == 1 && info.userType_idx[0] >= eventPtr->GetUserTypeList(no)->GetUserTypeListSize())
			{
				if (eventStack.size() == 1)
				{
					module_value = eventStack.top().return_value;
				}

				eventStack.pop();
				continue;
			}

			{ /// has bug!! WARNNING!!
				wiz::UserType* val = nullptr;
				if (eventStack.top().userType_idx.size() == 1) {
					if (eventPtr->GetUserTypeList(no)->GetUserTypeListSize() > eventStack.top().userType_idx.top()) {
						val = eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top());

						if (eventStack.top().userType_idx.top() >= 1 && val->GetName() == "$else"
							&& wiz::ToString(eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top() - 1)->GetName()) != "$if") {
							return "ERROR not exist $if, front $else.";
						}
						if (eventStack.top().userType_idx.top() == 0 && val->GetName() == "$else") {
							return "ERROR not exist $if, front $else.";
						}
					}
					else {
						val = nullptr;
					}
				}
				else
				{
					// # of userType_idx == nowUT.size() + 1, and nowUT.size() == conditionStack.size()..
					while (!eventStack.top().nowUT.empty() && eventStack.top().nowUT.top()->GetUserTypeListSize() <= eventStack.top().userType_idx.top())
					{
						eventStack.top().nowUT.pop();
						eventStack.top().userType_idx.pop();
						eventStack.top().conditionStack.pop();
					}

					if (!eventStack.top().nowUT.empty() && eventStack.top().nowUT.top()->GetUserTypeListSize() > eventStack.top().userType_idx.top()) {
						val = eventStack.top().nowUT.top()->GetUserTypeList(eventStack.top().userType_idx.top());

						if (eventStack.top().userType_idx.top() >= 1 && val->GetName() == "$else"
							&& wiz::ToString(eventStack.top().nowUT.top()->GetUserTypeList(eventStack.top().userType_idx.top() - 1)->GetName()) != "$if") {
							return "ERROR not exist $if, front $else.";
						}
						if (eventStack.top().userType_idx.top() == 0 && val->GetName() == "$else") {
							return "ERROR not exist $if, front $else.";
						}
					}
					else // same to else if( eventSTack.top().nowUT.empty() ), also same to else if ( 1 == eventStack.top().userType_idx.size() )
					{
						if (eventPtr->GetUserTypeList(no)->GetUserTypeListSize() > eventStack.top().userType_idx.top()) {
							val = eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top());

							if (eventStack.top().userType_idx.top() >= 1 && val->GetName() == "$else"
								&& wiz::ToString(eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top() - 1)->GetName()) != "$if") {
								return "ERROR not exist $if, front $else.";
							}
							if (eventStack.top().userType_idx.top() == 0 && val->GetName() == "$else") {
								return "ERROR not exist $if, front $else.";
							}
						}
						else {
							val = nullptr;
						}
					}
				}

				while (val != nullptr)
				{
					if ("$if" == val->GetName()) // ToDo!!
					{
						ExcuteData _excuteData; _excuteData.depth = excuteData.depth;
						_excuteData.chkInfo = true;
						_excuteData.info = eventStack.top();
						_excuteData.pEvents = eventPtr;

						std::string temp = val->GetUserTypeList(0)->ToString();

						temp = wiz::Convert(nullptr, global, temp, _excuteData, &builder);


						if (!eventStack.top().conditionStack.empty())
						{
							if ("TRUE" == temp && eventStack.top().conditionStack.top() == "FALSE")
							{
								temp = "FALSE";
							}
							else if ("FALSE" == temp && eventStack.top().conditionStack.top() == "FALSE")
							{
								temp = "FALSE";
							}
							else if (!eventStack.top().nowUT.empty() && eventStack.top().userType_idx.top() + 1 < eventStack.top().nowUT.top()->GetUserTypeListSize()
								&& (eventStack.top().nowUT.top()->GetUserTypeList(eventStack.top().userType_idx.top() + 1)->GetName() == "$else"))
							{
								eventStack.top().conditionStack.push(temp);
							}
							else if ("TRUE" == temp)
							{
								eventStack.top().conditionStack.push(temp);
							}
						}
						else
						{
							if (eventStack.top().userType_idx.top() + 1 < eventPtr->GetUserTypeList(no)->GetUserTypeListSize() &&
								eventPtr->GetUserTypeList(no)->GetUserTypeList(eventStack.top().userType_idx.top() + 1)->GetName() == "$else")
							{
								eventStack.top().conditionStack.push(temp);
							}
							else if ("TRUE" == temp)
							{
								eventStack.top().conditionStack.push(temp);
							}
						}

						if ("TRUE" == temp)
						{
							eventStack.top().nowUT.push(val->GetUserTypeList(1));
							eventStack.top().userType_idx.top()++;
							eventStack.top().userType_idx.push(0);

							break;
						}
						else if ("FALSE" == temp)
						{
							eventStack.top().userType_idx.top()++;
							break;
						}
						else
						{
							// debug..
							std::cout << "Error Debug : " << temp << std::endl;
							return "ERROR -3";
						}
					}
					else if ("$else" == val->GetName())
					{
						if (!eventStack.top().conditionStack.empty() && "FALSE" == eventStack.top().conditionStack.top())
						{
							eventStack.top().conditionStack.top() = "TRUE";
							eventStack.top().nowUT.push(val->GetUserTypeList(0));

							eventStack.top().userType_idx.top()++;
							eventStack.top().userType_idx.push(0);
							break;
						}
						else
						{
							eventStack.top().userType_idx.top()++;
							break;
						}
					}
					else { //
						std::cout << "it does not work. : " << wiz::ToString(val->GetName()) << std::endl;

						eventStack.top().userType_idx.top()++;
						break;
					}
				}
			}
		}

		if (1 == chk && !events.empty()) {
			auto _events = events.GetCopyUserTypeItem("Event");
			for (int i = 0; i < _events.size(); ++i) {
				_global->LinkUserType(_events[i]);
				_events[i] = nullptr;
			}
		}
		return module_value;
	}
}

