#ifndef PARSER_H
#define PARSER_H

#include "global.h"
#include "types.h"

// Scanning
namespace wiz {
	class Scanner
	{
	private:
		char* start;
		char* last;
	public:
		const wiz::LoadDataOption* option;
		long long* llptr;
		long long* llptr_len;
		UT_IT_NUM* num_ptr;
		long long num;
		int no;
	public:
		Scanner(char* start, char* last, const wiz::LoadDataOption* option,
			long long* llptr, long long num, long long* llptr_len, UT_IT_NUM* num_ptr,
			int no)
			: start(start), last(last), option(option), num_ptr(num_ptr), no(no)
		{
			this->llptr = llptr;
			this->llptr = llptr;
			this->num = num;
			this->llptr_len = llptr_len;
		}
		~Scanner() {
			//
		}
	private:

		long long func(bool make) {
			{
				std::vector<UT_IT_NUM*> _vecStack;
				_vecStack.reserve(1024);

				long long llptr_count = 0;
				long long start_idx = 0;
				long long last_idx = 0;
				long long count = 0;
				long long num_ptr_count = 0;
				char* token_first = start;
				char* token_last = start;
				int state = 0;

				long long now_idx = 0;
				for (long long i = 0; start + i < last; ++i, ++now_idx) {
					char* x = start + i;
					long long offset = 0;
					int idx;

					if (0 == state && '\'' == *x) {
						state = 2;
						token_last = x;
						last_idx = now_idx;
					}
					else if (2 == state && '\\' == *(x - 1) && '\'' == *x) {
						token_last = x;
						last_idx = now_idx;
					}
					else if (2 == state && '\'' == *x) {
						state = 0; token_last = x;
						last_idx = now_idx;
					}
					else if (2 == state && '\n' == *x) {
						throw "Error : \n is included in \' ~ \' ";
					}
					else if (0 == state && '\"' == *x) {
						state = 1;
						token_last = x;
						last_idx = now_idx;
					}
					else if (1 == state && '\\' == *(x - 1) && '\"' == *x) {
						token_last = x;
						last_idx = now_idx;
					}
					else if (1 == state && '\n' == *x) {
						throw "error : \n is included.. in \" ~ \" ";
					}
					else if (1 == state && '\"' == *x) {
						state = 0; token_last = x;
						last_idx = now_idx;
					}
					else if (0 == state && -1 != (idx = Equal2(option->Removal, *x)))
					{
						token_last = x - 1;
						last_idx = now_idx - 1;

						if (token_last >= 0 && token_last - token_first + 1 > 0) {
							llptr[llptr_count] = ((start_idx + num) << 32) + ((token_last - token_first + 1) << 2) + 0;
							llptr_count++;

							{
								if (token_last - token_first + 1 == 1) {
									if (start[start_idx] == option->Left) {
										llptr[llptr_count - 1] += 1;
									}
									if (start[start_idx] == option->Right) {
										llptr[llptr_count - 1] += 2;
									}
									if (start[start_idx] == option->Assignment) {
										llptr[llptr_count - 1] += 3;
									}
								}
							}

							if (!_vecStack.empty()) {
								_vecStack.back()->itNum++;
							}

							token_first = x + 1;
							start_idx = now_idx + 1;
						}
						else {
							token_first = token_first + 1;
							start_idx = start_idx + 1;
						}
						continue;
					}
					else if (0 == state && -1 != (idx = Equal2(option->Assignment, *x))) {
						token_last = x - 1;
						last_idx = now_idx - 1;

						if (token_last >= 0 && token_last - token_first + 1 > 0) {
							llptr[llptr_count] = ((start_idx + num) << 32) + ((token_last - token_first + 1) << 2) + 0;
							llptr_count++;

							{
								if (token_last - token_first + 1 == 1) {
									if (start[start_idx] == option->Left) {
										llptr[llptr_count - 1] += 1;
									}
									if (start[start_idx] == option->Right) {
										llptr[llptr_count - 1] += 2;
									}
									if (start[start_idx] == option->Assignment) {
										llptr[llptr_count - 1] += 3;
									}
								}
							}
							if (!_vecStack.empty()) {
								_vecStack.back()->itNum++;
							}
						}
						else {
							count++;
						}

						token_first = x + 1;
						start_idx = now_idx + 1;
						if (make) {
							llptr[llptr_count] = ((i + num) << 32) + ((1) << 2) + 3;
							llptr_count++;

							if (!_vecStack.empty()) {
								_vecStack.back()->eqNum++;
							}
						}
						else {

							llptr[llptr_count] = ((i + num) << 32) + ((1) << 2) + 3;
							llptr_count++;


							token_first = token_first + 1;
							start_idx = start_idx + 1;

						}
					}
					else if (0 == state && isWhitespace(*x)) { // isspace ' ' \t \r \n , etc... ?
						token_last = x - 1;
						last_idx = now_idx - 1;
						if (token_last >= 0 && token_last - token_first + 1 > 0) {

							llptr[llptr_count] = ((start_idx + num) << 32) + ((token_last - token_first + 1) << 2) + 0;
							llptr_count++;

							{
								if (token_last - token_first + 1 == 1) {
									if (start[start_idx] == option->Left) {
										llptr[llptr_count - 1] += 1;
									}
									if (start[start_idx] == option->Right) {
										llptr[llptr_count - 1] += 2;
									}
									if (start[start_idx] == option->Assignment) {
										llptr[llptr_count - 1] += 3;
									}
								}
							}
							if (!_vecStack.empty()) {
								_vecStack.back()->itNum++;
							}

							token_first = x + 1;

							start_idx = now_idx + 1;
						}
						else
						{
							token_first = token_first + 1;
							start_idx = start_idx + 1;
						}
					}
					else if (0 == state && -1 != (idx = Equal2(option->Left, *x))) {
						if (num_ptr) {
							num_ptr[now_idx] = wiz::UT_IT_NUM();
							num_ptr[now_idx].valid = 1;

							_vecStack.push_back(num_ptr + now_idx);
							num_ptr_count++;

							if (_vecStack.size() >= 2) {
								_vecStack[_vecStack.size() - 2]->utNum++;
							}
						}

						token_last = x - 1;
						last_idx = now_idx - 1;
						if (token_last >= 0 && token_last - token_first + 1 > 0) {
							llptr[llptr_count] = ((start_idx + num) << 32) + ((token_last - token_first + 1) << 2) + 0;
							llptr_count++;

							{
								if (token_last - token_first + 1 == 1) {
									if (start[start_idx] == option->Left) {
										llptr[llptr_count - 1] += 1;
									}
									if (start[start_idx] == option->Right) {
										llptr[llptr_count - 1] += 2;
									}
									if (start[start_idx] == option->Assignment) {
										llptr[llptr_count - 1] += 3;
									}
								}
							}
							if (_vecStack.size() >= 2) {
								_vecStack[_vecStack.size() - 2]->itNum++;
							}

							token_first = x + 1;

							start_idx = now_idx + 1;

							llptr[llptr_count] = ((i + num) << 32) + ((1) << 2) + 1;

							llptr_count++;



						}
						else {

							llptr[llptr_count] = ((i + num) << 32) + ((1) << 2) + 1;
							llptr_count++;


							token_first = token_first + 1;
							start_idx = start_idx + 1;
						}
					}
					else if (0 == state && -1 != (idx = Equal2(option->Right, *x))) {

						UT_IT_NUM* top = nullptr;
						if (num_ptr) {
							if (!_vecStack.empty()) {
								num_ptr[now_idx] = *_vecStack.back();

								top = _vecStack.back();
								_vecStack.pop_back();
							}
						}
						token_last = x - 1;
						last_idx = now_idx - 1;
						if (token_last >= 0 && token_last - token_first + 1 > 0) {
							llptr[llptr_count] = ((start_idx + num) << 32) + ((token_last - token_first + 1) << 2) + 0;
							llptr_count++;
							{
								if (token_last - token_first + 1 == 1) {
									if (start[start_idx] == option->Left) {
										llptr[llptr_count - 1] += 1;
									}
									if (start[start_idx] == option->Right) {
										llptr[llptr_count - 1] += 2;
									}
									if (start[start_idx] == option->Assignment) {
										llptr[llptr_count - 1] += 3;
									}
								}
							}
							if (!_vecStack.empty()) {
								_vecStack.back()->itNum++;
							}

							token_first = x + 1;
							start_idx = now_idx + 1;

							llptr[llptr_count] = ((i + num) << 32) + ((1) << 2) + 2;
							llptr_count++;

						}
						else {

							llptr[llptr_count] = ((i + num) << 32) + ((1) << 2) + 2;
							llptr_count++;

							token_first = token_first + 1;
							start_idx = start_idx + 1;

						}
					}
					else if (0 == state &&
						-1 != Equal2(*x, option->LineComment)) { // different from load_data_from_file
						token_last = x - 1;
						last_idx = now_idx - 1;
						if (token_last >= 0 && token_last - token_first + 1 > 0) {
							llptr[llptr_count] = ((start_idx + num) << 32) + ((token_last - token_first + 1) << 2) + 0;
							llptr_count++;
							{
								if (token_last - token_first + 1 == 1) {
									if (start[start_idx] == option->Left) {
										llptr[llptr_count - 1] += 1;
									}
									if (start[start_idx] == option->Right) {
										llptr[llptr_count - 1] += 2;
									}
									if (start[start_idx] == option->Assignment) {
										llptr[llptr_count - 1] += 3;
									}
								}
							}

							x = token_last + 1;
							now_idx = last_idx + 1;
							token_first = token_last + 1;
							start_idx = last_idx + 1;
							token_last = token_last + 1;

							last_idx = last_idx + 1;
						}

						for (; x <= last; ++x) {
							if (*x == '\n' || *x == '\0') // cf) '\r' ? '\0'?
							{
								break;
							}

							i++;
							now_idx++;
							token_last++;

							last_idx++;
						}

						token_first = x + 1;
						start_idx = now_idx + 1;

						token_last = x + 1;
						last_idx = now_idx + 1;
						continue;
					}
					else {
						//
					}

					token_last = x + offset;

					last_idx = now_idx + offset;

				}

				if (token_first < last)
				{
					if (last - 1 - token_first + 1 > 0) {
						llptr[llptr_count] = ((start_idx + num) << 32) + ((last - 1 - token_first + 1) << 2) + 0;
						llptr_count++;

						{
							if (last - 1 - token_first + 1 == 1) {
								if (start[start_idx] == option->Left) {
									llptr[llptr_count - 1] += 1;
								}
								if (start[start_idx] == option->Right) {
									llptr[llptr_count - 1] += 2;
								}
								if (start[start_idx] == option->Assignment) {
									llptr[llptr_count - 1] += 3;
								}
							}
						}
					}
				}

				if (this->llptr_len) {
					*(this->llptr_len) = llptr_count;
				}

				if (state != 0) {
					throw "Scanning Error in qouted string";
				}
				return count;
			}
		}
	public:
		void operator() () {
			func(true);
		}
	};

	std::pair<bool, int> Reserve(std::ifstream& inFile, const int num, bool* isFirst, const wiz::LoadDataOption& option,
		char*& _buffer, long long* _buffer_len, long long*& _llptr, long long* _llptr_len, UT_IT_NUM*& _num_ptr);

	class InFileReserver
	{
	private:
		std::ifstream* pInFile;
		bool isFirst;
	public:
		int Num;
	public:
		explicit InFileReserver(std::ifstream& inFile)
		{
			pInFile = &inFile;
			Num = 1;
			isFirst = true;
		}
		bool end()const { return pInFile->eof(); } //
	public:
		bool operator() (const wiz::LoadDataOption& option, char*& buffer, long long* buffer_len, long long*& llptr, long long* llptr_len,
			wiz::UT_IT_NUM*& num_ptr)
		{
			bool x = Reserve(*pInFile, Num, &isFirst, option, buffer, buffer_len, llptr, llptr_len, num_ptr).second > 0;
			return x;
		}
	};

	class NoneReserver
	{
	private:
		int count;
	public:
		explicit NoneReserver() : count(0) { }

		bool operator() (const wiz::LoadDataOption&)
		{
			count = 1;
			return false;
		}
		bool operator() (const wiz::LoadDataOption&, int, char*& buffer)
		{
			count = 1;
			return false;
		}

		bool end() const { return 1 == count; }
	};
}

// Parsing
namespace wiz {
	inline std::string& check_syntax_error1(std::string& str, const wiz::LoadDataOption& opt);

	long long FindRight(const char* buffer, const long long* llptr, long long start, long long last, const wiz::LoadDataOption& option);

	bool __LoadData(const char* buffer, long long llptr_len, const long long* llptr, UserType* _global,
		const wiz::LoadDataOption* _option,
		int start_state, int last_state, UserType** next, wiz::UT_IT_NUM* num_ptr, int no);
	
	bool _LoadData(InFileReserver& reserver, UserType& global, const wiz::LoadDataOption& option);

	bool LoadDataFromFile(const std::string& fileName, UserType& global);

	bool LoadDataFromString(const std::string& str, UserType& ut);
}


#endif
