#include "utils.hpp"

#include <string.h>
#include <stdlib.h>

using namespace std;

int char2num(char ch)
{
    if(ch >= '0' && ch <= '9')
        return (ch - '0');
    if(ch >= 'a' && ch <= 'f')
        return (ch- 'a' + 10);
    if(ch >= 'A' && ch <= 'F')
        return (ch- 'A' + 10);
    return -1;
}

void URLDecode(const my_sso_string input, my_sso_string& output)
{
    output.clear();
    size_t i = 0;
    size_t len = input.size();
    int num1, num2;
    while(i < len)
    {
        char ch = input[i];
        switch(ch)
        {
            case '+':
                output.append(1, ' ');
                i++;
                break;
            case '%':
                if(i+2 < len)
                {
                    num1 = char2num(input[i+1]);
                    num2 = char2num(input[i+2]);
                    if(num1 != -1 && num2 != -1)
                    {
                        char res = (char)((num1 << 4) | num2);
                        output.append(1, res);
                        i += 3;
                        break;
                    }
                }
                //go through
            default:
                output.append(1, ch);
                i++;
                break;
        }
    }
}



int parseCity(const my_sso_string &userArea, my_sso_string &province, my_sso_string &city, my_sso_string &district) {
	city = userArea;
	my_sso_string::size_type pos1 = userArea.find("\t");
	my_sso_string::size_type pos2 = userArea.rfind("\t");
	if (pos1 != my_sso_string::npos && pos2 != my_sso_string::npos) 
	{
		if (pos1 != pos2) 
		{  //省市区
			district = userArea.substr(pos2 + 1);
			city = userArea.substr(pos1 + 1, pos2 - pos1 -1);
			province = userArea.substr(0, pos1);
		} 
		else 
		{
			my_sso_string shortProvince = "";
			if (userArea.size() >= 4) 
				shortProvince = userArea.substr(0, 4);
			if (shortProvince == "北京" 
					|| shortProvince == "天津" 
					|| shortProvince == "上海" 
					|| shortProvince == "重庆" 
					|| shortProvince == "台湾" 
					|| shortProvince == "香港" 
					|| shortProvince == "澳门") 
			{  //北京 天津 上海 重庆 台湾 香港 澳门 + **区
				district = userArea.substr(pos2 + 1);
				city = userArea.substr(0, pos1);
				province = "";
			} 
			else 
			{  //省市
				district = "";
				city = userArea.substr(pos2 + 1);
				province = userArea.substr(0, pos1);
			}
		}
	}
	return 0;
}

bool isSameCity(const my_sso_string &cityA, const my_sso_string &cityB) {
	my_sso_string longCity;
	my_sso_string shortCity;
	if (cityA.size() > cityB.size()) {
		longCity = cityA;
		shortCity = cityB;
	} else {
		longCity = cityB;
		shortCity = cityA;
	}
	if (longCity.find(shortCity) != my_sso_string::npos) {
		return true;
	} else {
		return false;
	}
}
