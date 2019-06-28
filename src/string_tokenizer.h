#ifndef STRING_TOKENIZER_H
#define STRING_TOKENIZER_H


#include "string_builder.h"
#include <vector>
#include <string>

namespace wiz {

	class StringTokenizer
	{
	private:
		std::vector<std::string> _m_str;
		int count;
		bool exist;
		static const std::vector<std::string> whitespaces;
		int option;
	private:
		int FastFind(const char* str, const int n, const std::string& pat, const std::vector<int>& f, int min) {
			int posP = 0, posS = 0;
			const int lengthP = pat.size();
			const int lengthS = n;

			while (posP < lengthP && posS < lengthS) {
				// optimized!
				if (posS - lengthP > min) {
					return -1;
				}

				if (pat[posP] == str[posS]) {
					++posP; ++posS;
				}
				else {
					if (0 == posP) {
						++posS;
					}
					else {
						posP = f[posP - 1] + 1;
					}
				}
			}
			if (posP < lengthP) { return -1; }
			else {
				return posS - lengthP;
			}
		}

		// maybe this function has bug, when separator[idx].size() > 1?
		//  "abc   ??
		void Init(const std::string& str, const std::vector<std::string>& separator, StringBuilder* builder) // assumtion : separators are sorted by length?, long -> short
		{
			const int sep_size = separator.size();
			const int str_size = str.size();
			if (separator.empty() || str.empty()) { return; } // if str.empty() == false then _m_str.push_back(str); // ?
															  //
			builder->Clear();
			builder->Append(str.c_str(), str.size());

			_m_str.reserve(str.size() / 2);
			int left = 0;
			int right = 0;
			int state = 0;
			this->exist = false;

			{
				std::vector<std::vector<int>> f(sep_size);
				for (int t = 0; t < sep_size; ++t) {
					f[t] = std::vector<int>(separator[t].size(), -1);
					const int lengthP = f[t].size();
					for (int j = 1; j < lengthP; ++j) {
						int i = f[t][j - 1];
						while (str[j] != str[i + 1] && i >= 0) {
							i = f[t][i];
						}
						if (str[j] == str[i + 1]) {
							f[t][j] = i + 1;
						}
						else {
							f[t][j] = -1;
						}
					}
				}

				for (int i = 0; i < str.size(); ++i) {
					right = i;
					int _select = -1;
					bool pass = false;

					if (1 == option && 0 != state && str.size() - 1 == i) {
						//
					}
					else if (1 == option && 0 == state && '\'' == str[i]) {
						if (i == 0) {
							state = 2;
							continue;
						}
						else if (i > 0 && '\\' == str[i - 1]) {
							throw "ERROR for Init 1 on StringTokenizer"; // error!
						}
						else if (i > 0) {
							state = 2;
							continue;
						}
					}
					else if (1 == option && 2 == state && '\'' == str[i]) {
						if ('\\' == str[i - 1]) {
							continue;
						}
						else {
							state = 0;
							if (i == str.size() - 1) {
								if (right - left + 1 > 0) {
									_m_str.emplace_back(builder->Divide(right - left + 1), right - left + 1);
									builder->LeftShift(right - left + 2);
								}
							}
							continue;
						}
						continue;
					}
					else if (1 == option && 2 == state) {
						continue;
					}
					else if (1 == option && 0 == state && '\"' == str[i]) {
						if (i == 0) {
							state = 1;
							continue;
						}
						else if (i > 0 && '\\' == str[i - 1]) {
							throw "ERROR for Init 2 on StringTokenizer"; // error!
						}
						else if (i > 0) {
							state = 1;
							continue;
						}
					}
					else if (1 == option && 1 == state && '\"' == str[i]) {
						if ('\\' == str[i - 1]) {
							continue;
						}
						else {
							state = 0;
							if (i == str.size() - 1) {
								if (right - left + 1 > 0) {
									_m_str.emplace_back(builder->Divide(right - left + 1), right - left + 1);
									builder->LeftShift(right - left + 1);
								}
							}
							continue;
						}
					}
					else if (1 == option && 1 == state) {
						continue;
					}


					int min = INT_MAX, min_idx = -1;

					for (int j = 0; j < sep_size; ++j) {
						const int idx = FastFind(str.c_str() + i, str.size() - i, separator[j], f[j], min);

						if (-1 < idx) {
							pass = true;
							if (-1 == min_idx) {
								min = idx;
								min_idx = j;
							}
							else if (min > idx) {
								min = idx;
								min_idx = j;
							}
						}
					}
					if (min_idx > -1) {
						i = i + min;
						_select = min_idx;
					}

					if (!pass) {
						i = str.size() - 1;
					}

					right = i;

					if (pass) {
						this->exist = true;

						if (right - left + 1 >= 1) {
							const char* temp = builder->Divide(right - left);

							if (right - left + 1 > 1) {
								_m_str.emplace_back(temp, right - left);
								builder->LeftShift(right - left + separator[_select].size());
							}
							else {
								builder->LeftShift(separator[_select].size());
							}
						}

						i = i + separator[_select].size() - 1;

						left = right + separator[_select].size();
						right = left;
					}
					else if (!pass && i == str.size() - 1) {
						if (right - left + 1 > 0) {
							_m_str.emplace_back(builder->Divide(right - left + 1), right - left + 1);
							//	std::cout << "chk " << separator[0] << " | " << str << " " << _m_str.back() << std::endl;
							builder->LeftShift(right - left + 1);
						}
						else {
							//GETCH();
						}
					}
				}
			}
			//cout << "str is " << str <<  " state  is " << state << endl;
		}
		void Init(std::string&& str, const std::vector<std::string>& separator, StringBuilder* builder) // assumtion : separators are sorted by length?, long -> short
		{
			const int sep_size = separator.size();
			const int str_size = str.size();
			if (separator.empty() || str.empty()) { return; } // if str.empty() == false then _m_str.push_back(str); // ?
															  //
			builder->Clear();
			builder->Append(str.c_str(), str.size());

			_m_str.reserve(str.size() / 2);
			int left = 0;
			int right = 0;
			int state = 0;
			this->exist = false;

			{
				std::vector<std::vector<int>> f(sep_size);
				for (int t = 0; t < sep_size; ++t) {
					f[t] = std::vector<int>(separator[t].size(), -1);
					const int lengthP = f[t].size();
					for (int j = 1; j < lengthP; ++j) {
						int i = f[t][j - 1];
						while (str[j] != str[i + 1] && i >= 0) {
							i = f[t][i];
						}
						if (str[j] == str[i + 1]) {
							f[t][j] = i + 1;
						}
						else {
							f[t][j] = -1;
						}
					}
				}

				for (int i = 0; i < str.size(); ++i) {
					right = i;
					int _select = -1;
					bool pass = false;

					if (1 == option && 0 != state && str.size() - 1 == i) {
						//
					}
					else if (1 == option && 0 == state && '\'' == str[i]) {
						if (i == 0) {
							state = 2;
							continue;
						}
						else if (i > 0 && '\\' == str[i - 1]) {
							throw "ERROR for Init 1 on StringTokenizer"; // error!
						}
						else if (i > 0) {
							state = 2;
							continue;
						}
					}
					else if (1 == option && 2 == state && '\'' == str[i]) {
						if ('\\' == str[i - 1]) {
							continue;
						}
						else {
							state = 0;
							if (i == str.size() - 1) {
								if (right - left + 1 > 0) {
									_m_str.emplace_back(builder->Divide(right - left + 1), right - left + 1);
									builder->LeftShift(right - left + 2);
								}
							}
							continue;
						}
						continue;
					}
					else if (1 == option && 2 == state) {
						continue;
					}
					else if (1 == option && 0 == state && '\"' == str[i]) {
						if (i == 0) {
							state = 1;
							continue;
						}
						else if (i > 0 && '\\' == str[i - 1]) {
							throw "ERROR for Init 2 on StringTokenizer"; // error!
						}
						else if (i > 0) {
							state = 1;
							continue;
						}
					}
					else if (1 == option && 1 == state && '\"' == str[i]) {
						if ('\\' == str[i - 1]) {
							continue;
						}
						else {
							state = 0;
							if (i == str.size() - 1) {
								if (right - left + 1 > 0) {
									_m_str.emplace_back(builder->Divide(right - left + 1), right - left + 1);
									builder->LeftShift(right - left + 1);
								}
							}
							continue;
						}
					}
					else if (1 == option && 1 == state) {
						continue;
					}

					int min = INT_MAX, min_idx = -1;

					for (int j = 0; j < sep_size; ++j) {
						const int idx = FastFind(str.c_str() + i, str.size() - i, separator[j], f[j], min);

						if (-1 < idx) {
							pass = true;
							if (-1 == min_idx) {
								min = idx;
								min_idx = j;
							}
							else if (min > idx) {
								min = idx;
								min_idx = j;
							}
						}
					}
					if (min_idx > -1) {
						i = i + min;
						_select = min_idx;
					}

					if (!pass) {
						i = str.size() - 1;
					}

					right = i;

					if (pass) {
						this->exist = true;

						if (right - left + 1 >= 1) {
							const char* temp = builder->Divide(right - left);

							if (right - left + 1 > 1) {
								_m_str.emplace_back(temp, right - left);
								builder->LeftShift(right - left + separator[_select].size());
							}
							else {
								builder->LeftShift(separator[_select].size());
							}
						}

						i = i + separator[_select].size() - 1;

						left = right + separator[_select].size();
						right = left;
					}
					else if (!pass && i == str.size() - 1) {
						if (right - left + 1 > 0) {
							_m_str.emplace_back(builder->Divide(right - left + 1), right - left + 1);
							//	std::cout << "chk " << separator[0] << " | " << str << " " << _m_str.back() << std::endl;
							builder->LeftShift(right - left + 1);
						}
						else {
							//GETCH();
						}
					}
				}
			}
			//cout << "str is " << str <<  " state  is " << state << endl;
		}
	public:
		explicit StringTokenizer() : count(0), exist(false), option(0) { }
		explicit StringTokenizer(const std::string& str, const std::string& separator, StringBuilder* builder, int option = 0)
			: count(0), exist(false), option(option)
		{
			std::vector<std::string> vec; vec.push_back(separator);
			Init(str, vec, builder);
		}
		explicit StringTokenizer(std::string&& str, const std::string& separator, StringBuilder* builder, int option = 0)
		{
			std::vector<std::string> vec; vec.push_back(separator);
			Init(std::move(str), vec, builder);
		}
		explicit StringTokenizer(const std::string& str, const std::vector<std::string>& separator, StringBuilder* builder, int option = 0)
			: count(0), exist(false), option(option)
		{
			Init(str, separator, builder);
		}
		explicit StringTokenizer(std::string&& str, const std::vector<std::string>& separator, StringBuilder* builder, int option = 0)
			: count(0), exist(false), option(option)
		{
			Init(std::move(str), separator, builder);
		}

		explicit StringTokenizer(const std::string& str, StringBuilder* builder, int option = 0)
			: count(0), exist(false), option(option)
		{
			Init(str, whitespaces, builder);
		}
		explicit StringTokenizer(std::string&& str, StringBuilder* builder, int option = 0)
			: count(0), exist(false), option(option)
		{
			Init(std::move(str), whitespaces, builder);
		}
		int countTokens()const
		{
			return _m_str.size();
		}
		std::string nextToken()
		{
			if (hasMoreTokens())
				return _m_str[count++];
			else
				throw "error in nextToken!";
		}
		bool hasMoreTokens()const
		{
			return count < _m_str.size();
		}

		bool isFindExist()const
		{
			return exist;
		}

	};
}

#endif
