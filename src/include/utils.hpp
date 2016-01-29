#include <vector>
#include <string>
#include "sso_string/sso_string.h"

int char2num(char ch);
void URLDecode(const my_sso_string input, my_sso_string& output);
int parseCity(const my_sso_string &userArea, my_sso_string &province, my_sso_string &city, my_sso_string &district);
bool isSameCity(const my_sso_string &cityA, const my_sso_string &cityB);
