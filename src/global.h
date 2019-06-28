#ifndef GLOBAL_H
#define GLOBAL_H

#include <ctime>

#include <iostream>
#include <fstream>

#include <list>
#include <deque>
#include <vector>
#include <map>
#include <set>
#include <string>

#include <regex>
#include <utility>

namespace wiz {
	template<class T>
	using ARRAY_QUEUE = std::deque<T>;// chk?
	template<class T>
	using VECTOR = std::vector<T>;
	template<class T, class S>
	using MAP = std::map<T, S>;
	template<class T, class S>
	using SMALL_MAP = std::map<T, S>; // array_map with std::vector

	bool IsInteger(const std::string& str);

	class DataType {
	private:
		mutable std::string str;
		long long integer = 0;
		mutable bool integer_changed = false;
		mutable	bool str_changed = true;
		enum class Type{ STR, INT };
		Type type = Type::STR;
	public:
		DataType() {}
		DataType(const char* cstr) {
			str = std::string(cstr);
		}
		DataType(const std::string& str) :
			str(str)
		{
			//
		}
		virtual ~DataType() { }

		void ConvertToInt() {
			if (!IsInteger() && wiz::IsInteger(str) && str_changed) {
				integer = std::stoll(str);
				integer_changed = true;
				str_changed = false;
				type = Type::INT;
			}
		}

		std::string ToString() const {
			if (IsInteger() && integer_changed) {
				str = std::to_string(integer);
				integer_changed = false;
				str_changed = false;
			}
			return str;
		}

		bool IsInteger() const
		{
			return type == Type::INT;
		}
	
		std::pair<bool, std::string> GetStr() const {
			return { type == Type::STR, str };
		}
		std::pair<bool, int> GetInt() const {
			return { type == Type::INT, integer };
		}

		void SetStr(const std::string& str) {
			if (type == Type::STR) {
				str_changed = true;
				this->str = str;
			}
		}
		void SetInt(const long long val) {
			if (type == Type::INT) {
				this->integer = val;
				integer_changed = true;
			}
		}

		bool operator==(const DataType& other) const
		{
			return (this->type == Type::STR && this->str == other.str)
				|| (this->type == Type::INT && this->integer == other.integer);
		}
		bool operator!=(const DataType& other) const
		{
			return !(*this == other);
		}
	};

	using WIZ_STRING_TYPE = DataType;

	inline std::string ToString(const WIZ_STRING_TYPE& other) {
		return other.ToString();
	}


	inline long long GetIdx(long long x)  noexcept {
		return (x >> 32) & 0x00000000FFFFFFFF;
	}
	inline long long GetLength(long long x)  noexcept {
		return (x & 0x00000000FFFFFFFC) >> 2;
	}
	inline long long GetType(long long x) noexcept {
		return x & 3; // % 4
	}

	class LoadDataOption
	{
	public:
		char LineComment = 0;	// # 
		char Left = 0, Left2 = 0, Right = 0, Right2 = 0;	// { } , [ ] <- json
		char Assignment = 0;	// = , :
		char Removal = 0;		// ',', empty. 
	};

	inline int Equal(const std::vector<char>& option, const char ch)
	{
		for (int i = 0; i < option.size(); ++i) {
			if (ch == option[i]) {
				return i;
			}
		}
		return -1;
	}

	inline int Equal(const std::vector<std::string>& option, const char ch)
	{
		for (int i = 0; i < option.size(); ++i) {
			if (option[i].size() == 1 && ch == option[i][0]) {
				return i;
			}
		}
		return -1;
	}

	inline int Equal2(const char& option, const char ch)
	{
		if (ch == option) {
			return 0;
		}

		return -1;
	}

	class UT_IT_NUM
	{
	public:
		int utNum = 0;
		int itNum = 0;
		int eqNum = 0;
		int valid = 0;
	public:
		UT_IT_NUM() {

		}
		UT_IT_NUM(int utNum, int itNum, int eqNum)
			: utNum(utNum), itNum(itNum), eqNum(eqNum)
		{

		}

		UT_IT_NUM& operator=(const UT_IT_NUM& other) = default;
	};

	inline bool isWhitespace(const char ch) noexcept
	{
		switch (ch)
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case '\v':
		case '\f':
			return true;
			break;
		}
		return false;
	}

}

#endif
