#include "parser.h"

namespace wiz {
	std::pair<bool, int> Reserve(std::ifstream& inFile, const int num, bool* isFirst, const wiz::LoadDataOption& option,
		char*& _buffer, long long* _buffer_len, long long*& _llptr, long long* _llptr_len, UT_IT_NUM*& _num_ptr)
	{
		if (inFile.eof()) {
			return { false, 0 };
		}

		int a = clock();

		std::string temp;
		char* buffer = nullptr;
		std::vector<long long> start(1, 0);
		std::vector<long long> last(1, 0);
		std::vector<long long> llptr_len(1, 0);
		long long file_length;

		inFile.seekg(0, inFile.end);
		unsigned long long length = inFile.tellg();
		inFile.seekg(0, inFile.beg);

		file_length = length;

		buffer = new char[file_length + 1]; // 

									   // read data as a block:
		inFile.read(buffer, file_length);
		buffer[file_length] = '\0';

		//	inFile.seekg(0, inFile.end);
		//	char temp;
		//	inFile >> temp;

		start[0] = 0;
		last[0] = file_length;

		int b = clock();
		std::cout << "load file to char array " << b - a << "ms\n";

		long long* llptr = nullptr;
		UT_IT_NUM* num_ptr = (UT_IT_NUM*)calloc(file_length, sizeof(UT_IT_NUM));

		llptr = new long long[file_length];
		long long len;

		Scanner scanner(buffer + start[0], buffer + last[0], &option, llptr, start[0], &len, num_ptr, 0);

		scanner();

		*_llptr_len = len;
		_buffer = buffer;
		_llptr = llptr;
		*_buffer_len = file_length;
		_num_ptr = num_ptr;

		return{ true, 1 };
	}

	std::string& check_syntax_error1(std::string& str, const wiz::LoadDataOption& opt) {
		if (1 == str.size() && (opt.Left == str[0] || opt.Left2 == str[0] || opt.Right == str[0] ||
			opt.Right2 == str[0] || opt.Assignment == str[0])) {
			throw "check syntax error 1 : " + str;
		}
		return str;
	}
	long long FindRight(const char* buffer, const long long* llptr, long long start, long long last, const wiz::LoadDataOption& option)
	{
		for (long long a = last; a >= start; --a) {
			long long len = GetLength(llptr[a]);
			long long val = GetType(llptr[a]); // % 4  

			if (len == 1 && (-1 != Equal2(2, val) || -1 != Equal2(2, val))) { // right
				return a;
			}

			bool pass = false;
			if (len == 1 && (-1 != Equal2(1, val) || -1 != Equal2(1, val))) { // left
				return a;
			}
			else if (len == 1 && -1 != Equal2(3, val)) { // assignment
				//
				pass = true;
			}

			if (a < last && pass == false) {
				long long len = GetLength(llptr[a + 1]);
				long long val = GetType(llptr[a + 1]); // % 4

				if (!(len == 1 && -1 != Equal2(3, val))) // assignment
				{                // NOT
					return a;
				}
			}
		}
		return -1;
	}
	bool __LoadData(const char* buffer, long long llptr_len, const long long* llptr, UserType* _global,
		const wiz::LoadDataOption* _option,
		int start_state, int last_state, UserType** next, wiz::UT_IT_NUM* num_ptr, int no) // first, strVec.empty() must be true!!
	{
		std::vector<wiz::DataType> varVec;
		std::vector<wiz::DataType> valVec;


		if (llptr_len <= 0) {
			return false;
		}

		UserType& global = *_global;
		const wiz::LoadDataOption& option = *_option;

		int state = start_state;
		int braceNum = 0;
		std::vector< UserType* > nestedUT(1);
		std::string var, val;

		nestedUT.reserve(10);
		nestedUT[0] = &global;


		long long count = 0;
		const long long* x = llptr;
		const long long* x_next = x;

		for (long long i = 0; i < llptr_len; ++i) {
			x = x_next;
			{
				x_next = x + 1;
			}
			if (count > 0) {
				count--;
				continue;
			}
			long long len = GetLength(llptr[i]);

			switch (state)
			{
			case 0:
			{
				// Left 1
				if (len == 1 && (-1 != Equal2(1, GetType(llptr[i])) || -1 != Equal2(1, GetType(llptr[i])))) {
					//i += 1;

					if (!varVec.empty()) {

						nestedUT[braceNum]->AddItem((varVec), (valVec), varVec.size());

						varVec.clear();
						valVec.clear();
					}

					UserType temp("");

					nestedUT[braceNum]->AddUserTypeItem(temp);
					UserType* pTemp = nullptr;
					nestedUT[braceNum]->GetLastUserTypeItemRef("", pTemp);

					braceNum++;

					/// new nestedUT
					if (nestedUT.size() == braceNum) { /// changed 2014.01.23..
						nestedUT.push_back(nullptr);
					}

					/// initial new nestedUT.
					nestedUT[braceNum] = pTemp;
					///

					state = 0;

					if (long long idx = GetIdx(llptr[i]); num_ptr[idx].valid) {
						nestedUT[braceNum]->ReserveUserTypeList(num_ptr[idx].utNum);
						nestedUT[braceNum]->ReserveItemList(num_ptr[idx].itNum - num_ptr[idx].eqNum);

						//std::cout << "chk start " << num_ptr[idx].itNum - num_ptr[idx].eqNum << "\n";

						nestedUT[braceNum]->ReserveIList(num_ptr[idx].utNum + num_ptr[idx].itNum - num_ptr[idx].eqNum);
					}
				}
				// Right 2
				else if (len == 1 && (-1 != Equal2(2, GetType(llptr[i])) || -1 != Equal2(2, GetType(llptr[i])))) {
					//i += 1;

					state = 0;

					if (!varVec.empty()) {
						// UserTypeListsize?

						//nestedUT[braceNum]->ReserveIList(num_ptr[*x].utNum + num_ptr[*x].itNum - num_ptr[*x].eqNum);
						//nestedUT[braceNum]->ReserveUserTypeList(num_ptr[*x].utNum);
						//nestedUT[braceNum]->ReserveItemList(num_ptr[*x].itNum - num_ptr[*x].eqNum);

						{
							nestedUT[braceNum]->AddItem(varVec, valVec, varVec.size());
							//std::cout << "ends " << nestedUT[braceNum]->GetItemListSize() << "\n";
						}


						varVec.clear();
						valVec.clear();
					}


					if (braceNum == 0) {
						UserType ut;
						ut.AddUserTypeItem(UserType("#")); // json -> "var_name" = val  // clautext, # is line comment delimiter.
						UserType* pTemp = nullptr;
						ut.GetLastUserTypeItemRef("#", pTemp);
						int utCount = 0;
						int itCount = 0;
						auto max = nestedUT[braceNum]->GetIListSize();
						for (auto i = 0; i < max; ++i) {
							if (nestedUT[braceNum]->IsUserTypeList(i)) {
								ut.GetUserTypeList(0)->AddUserTypeItem(std::move(*(nestedUT[braceNum]->GetUserTypeList(utCount))));
								utCount++;
							}
							else {
								ut.GetUserTypeList(0)->AddItemList(std::move(nestedUT[braceNum]->GetItemList(itCount)));
								itCount++;
							}
						}

						nestedUT[braceNum]->Remove();
						nestedUT[braceNum]->AddUserTypeItem(std::move(*(ut.GetUserTypeList(0))));

						braceNum++;
					}

					{
						if (braceNum < nestedUT.size()) {
							nestedUT[braceNum] = nullptr;
						}
						braceNum--;
					}
				}
				else {
					//std::pair<bool, Token2> bsPair;

					//	if (i < last_idx)
					//	{
					//		bsPair = std::make_pair(true, strVec[i + 1]);
					//	}
					//	else {
					//		bsPair = std::make_pair(false, Token2());
					//	}
					if (x < llptr + llptr_len - 1) {
						long long _len = GetLength(llptr[i + 1]); //llptr[*(x + 1)];
						// EQ 3
						if (_len == 1 && -1 != Equal2(3, GetType(llptr[i + 1]))) { //buffer[*(x + 1)])) {
							// var2
							var = std::string(buffer + GetIdx(llptr[i]), len);
							state = 1;
							//i += 1;

							//i += 1;
							{
								count = 1;
							}
						}
						else {
							// var1
							if (x <= llptr + llptr_len - 1) {

								val = std::string(buffer + GetIdx(llptr[i]), len);

								varVec.push_back(check_syntax_error1(var, option));
								valVec.push_back(check_syntax_error1(val, option));
								//nestedUT[braceNum]->AddItem("", ""); // std::move(val));
								val = "";

								state = 0;
								//i += 1;

							}
						}
					}
					else
					{
						// var1
						if (x <= llptr + llptr_len - 1)
						{
							val = std::string(buffer + GetIdx(llptr[i]), len);
							varVec.push_back(check_syntax_error1(var, option));
							valVec.push_back(check_syntax_error1(val, option));
							//nestedUT[braceNum]->AddItem("", "");// std::move(val));
							val = "";

							state = 0;
							//i += 1;

						}
					}
				}
			}
			break;
			case 1:
				// LEFT 1
			{
				if (len == 1 && (-1 != Equal2(1, GetType(llptr[i])) || -1 != Equal2(1, GetType(llptr[i])))) {
					//i += 1;

					nestedUT[braceNum]->AddItem((varVec), (valVec), varVec.size());

					varVec.clear();
					valVec.clear();


					///
					{
						nestedUT[braceNum]->AddUserTypeItem(UserType(var));
						UserType* pTemp = nullptr;
						nestedUT[braceNum]->GetLastUserTypeItemRef(var, pTemp);
						var = "";
						braceNum++;

						/// new nestedUT
						if (nestedUT.size() == braceNum) {
							nestedUT.push_back(nullptr);
						}

						/// initial new nestedUT.
						nestedUT[braceNum] = pTemp;
					}
					///
					state = 0;


					if (long long idx = GetIdx(llptr[i]); num_ptr[idx].valid) {
						nestedUT[braceNum]->ReserveUserTypeList(num_ptr[idx].utNum);
						nestedUT[braceNum]->ReserveItemList(num_ptr[idx].itNum - num_ptr[idx].eqNum);
						nestedUT[braceNum]->ReserveIList(num_ptr[idx].utNum + num_ptr[idx].itNum - num_ptr[idx].eqNum);
					}
				}
				else {
					if (x <= llptr + llptr_len - 1) {
						val = std::string(buffer + GetIdx(llptr[i]), len);
						//std::cout << "val " << val << "\n";
						//std::cout << "len " << len << " val.size() " << val.size() << "\n";

						//i += 1;

						varVec.push_back(check_syntax_error1(var, option));
						valVec.push_back(check_syntax_error1(val, option));
						//nestedUT[braceNum]->AddItem("", ""); // std::move(var), std::move(val));
						var = ""; val = "";

						state = 0;
					}
				}
			}
			break;
			default:
				// syntax err!!
				throw "syntax error ";
				break;
			}
		}

		if (next) {
			*next = nestedUT[braceNum];
		}

		if (varVec.empty() == false) {
			nestedUT[braceNum]->AddItem(varVec, valVec, varVec.size());

			varVec.clear();
			valVec.clear();
		}

		if (state != last_state) {
			throw std::string("error final state is not last_state!  : ") + std::to_string(state);
		}
		if (x > llptr + llptr_len) {
			throw std::string("error x > buffer + buffer_len: ");
		}

		return true;
	}
	bool _LoadData(InFileReserver& reserver, UserType& global, const wiz::LoadDataOption& option) // first, strVec.empty() must be true!!
	{
		const int pivot_num = 0;
		char* buffer = nullptr;
		long long* llptr = nullptr;
		long long buffer_total_len;
		long long llptr_len = 0;
		wiz::UT_IT_NUM* num_ptr = nullptr;

		bool end = false;
		{
			int a = clock();
			end = !reserver(option, buffer, &buffer_total_len, llptr, &llptr_len, num_ptr);
			int b = clock();
			std::cout << b - a << "ms \n";

			if (llptr_len <= 0) {
				return true;
			}
		}

		UserType _global;
		UserType* next;

		int a = clock();

		const long long num = llptr_len; //

		{
			__LoadData(buffer, llptr_len, llptr, &_global, &option, 0, 0, &next, num_ptr, 0);
		}

		int b = clock();
		delete[] buffer;
		if (llptr) {
			delete[] llptr;
		}
		if (num_ptr) { free(num_ptr); }
		int d = clock();

		global = std::move(_global);

		std::cout << "parsing " << b - a << "ms" << "\n";
		std::cout << "delete and free " << d - b << "ms\n";

		return true;
	}

	bool LoadDataFromFile(const std::string& fileName, UserType& global)
	{
		bool success = true;
		std::ifstream inFile;
		inFile.open(fileName, std::ios::binary);


		if (true == inFile.fail())
		{
			inFile.close(); return false;
		}

		UserType globalTemp;

		try {

			InFileReserver ifReserver(inFile);
			wiz::LoadDataOption option;
			option.Assignment = ('=');
			option.Left = '{';  option.Left2 = ('{');
			option.Right = '}'; option.Right2 = ('}');
			option.LineComment = ('#');
			option.Removal = ' ';

			char* buffer = nullptr;
			ifReserver.Num = 1 << 19;
			//	strVec.reserve(ifReserver.Num);
			// cf) empty file..
			if (false == _LoadData(ifReserver, globalTemp, option))
			{
				inFile.close();
				return false; // return true?
			}

			inFile.close();
		}
		catch (const char* err) { std::cout << err << std::endl; inFile.close(); return false; }
		catch (const std::string& e) { std::cout << e << std::endl; inFile.close(); return false; }
		catch (std::exception e) { std::cout << e.what() << std::endl; inFile.close(); return false; }
		catch (...) { std::cout << "not expected error" << std::endl; inFile.close(); return false; }


		global = std::move(globalTemp);

		return true;
	}

	bool LoadDataFromString(const std::string& str, UserType& ut)
	{
		UserType utTemp;

		char* buffer = new char[str.size() + 1];

		for (int i = 0; i < str.size(); ++i) {
			buffer[i] = str[i];
		}
		buffer[str.size()] = '\0';

		wiz::LoadDataOption option;
		option.Assignment = ('=');
		option.Left = '{';  option.Left2 = ('{');
		option.Right = '}'; option.Right2 = ('}');
		option.LineComment = ('#');
		option.Removal = ' ';
		long long* llptr = new long long[str.size()];
		long long llptr_len = 0;
		wiz::UT_IT_NUM* num_ptr = (wiz::UT_IT_NUM*)calloc(str.size(), sizeof(wiz::UT_IT_NUM));
		UserType* next;

		Scanner scanner(buffer, buffer + str.size(), &option, llptr, 0, &llptr_len, num_ptr, 0);

		scanner();

		try {
			__LoadData(buffer, llptr_len, llptr, &utTemp, &option, 0, 0, &next, num_ptr, 0);
		}
		catch (const char* err) {
			delete[] buffer; delete[] llptr; free(num_ptr); std::cout << err << std::endl; return false;
		}
		catch (std::exception& e) {
			delete[] buffer; delete[] llptr; free(num_ptr); std::cout << e.what() << std::endl; return false;
		}
		catch (const std::string& e) {
			delete[] buffer; delete[] llptr; free(num_ptr); std::cout << str << std::endl; return false;
		}
		catch (...) {
			delete[] buffer; delete[] llptr; free(num_ptr); std::cout << "not expected error" << std::endl; return  false;
		}

		delete[] buffer; delete[] llptr; free(num_ptr);
		{
			long long itCount = 0;
			long long utCount = 0;

			for (int i = 0; i < utTemp.GetIListSize(); ++i) {
				if (utTemp.IsItemList(i)) {
					ut.AddItemList(std::move(utTemp.GetItemList(itCount)));
					itCount++;
				}
				else {
					ut.AddUserTypeItem(std::move(*utTemp.GetUserTypeList(utCount)));
					utCount++;
				}
			}
		}
		return true;
	}

}
