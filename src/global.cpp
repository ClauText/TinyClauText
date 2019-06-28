#include "global.h"

namespace wiz {
	bool IsInteger(const std::string& str) {
		int state = 0;
		for (int i = 0; i < str.size(); ++i) {
			switch (state)
			{
			case 0:
				if ('+' == str[i] || '-' == str[i]) {
					state = 0;
				}
				else if (str[i] >= '0' && str[i] <= '9')
				{
					state = 1;
				}
				else { return false; }
				break;
			case 1:
				if (str[i] >= '0' && str[i] <= '9') {
					state = 1;
				}
				else { return false; }
				break;
			}
		}
		return 1 == state; /// chk..
	}

}
