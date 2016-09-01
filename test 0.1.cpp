

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <utility>
#include <algorithm>
#include <string>
using namespace std;

#include <wiz/load_data.h>
#include <wiz/stacks.h>
#include <wiz/cpp_string.h>


class EventInfo
{
public:
	wiz::load_data::UserType* eventUT;
	wiz::load_data::UserType* nowUT; //
	int item_idx;
	int userType_idx;
	vector<pair<string, string>> parameters;
	string id; //
	wiz::ArrayStack<string> conditionStack;
	int depth;
	int num; // num of statements? for if, else statements.
public:	
	EventInfo() : eventUT(NULL), nowUT(NULL), depth(0), num(0)
	{

	}
};


// only one exist or do not exist?
inline string GetItem(const wiz::load_data::UserType* ut, const string& name) {
	return ut->GetItem(name)[0].Get(0);
}

inline wiz::load_data::UserType* GetUserType(const wiz::load_data::UserType* ut, const string& name)
{
	return ut->GetUserTypeItem(name)[0].Get(0);
}
string Find(wiz::load_data::UserType* ut, const string& str)
{
	int idx = -1;
	for (int i = str.size() - 1; i >= 0; --i) {
		if ('/' == str[i]) {
			idx = i;
			break;
		}
	}
	//
	// exception?
	//
	return wiz::load_data::UserType::Find(ut,
		wiz::String::substring(str, 0, idx)).second[0]->GetItem(wiz::String::substring(str, idx+1))[0].Get(0);
}
string reverse(string str) { /// to std::reverse ?
	std::reverse(str.begin(), str.end());
	return str;
}

string GetType(const string& str) {
	if (wiz::load_data::Utility::IsInteger(str)) { return "INTEGER"; }
	else if (wiz::load_data::Utility::IsDouble(str)) { return "DOUBLE"; }
	else if (wiz::load_data::Utility::IsDate(str)) { return "DATE"; }
	else return "STRING";
}
string Compare(const string& str1, const string& str2, const int type = 0)
{
	string type1 = GetType(str1);
	string type2 = GetType(str2);

	if (type1 != type2) {
		return "ERROR";
	}

	if ("STRING" == type1 || type == 1)
	{
		if (str1 < str2) {
			return "< 0";
		}
		else if (str1 == str2) {
			return "== 0";
		}
		return "> 0";
	}
	else if ("INTEGER" == type1)
	{
		if (wiz::load_data::Utility::IsMinus(str1) && !wiz::load_data::Utility::IsMinus(str2)) { return "< 0"; }
		else if (!wiz::load_data::Utility::IsMinus(str1) && wiz::load_data::Utility::IsMinus(str2)) { return "> 0"; }

		const bool minusComp = wiz::load_data::Utility::IsMinus(str1) && wiz::load_data::Utility::IsMinus(str2);

		if (false == minusComp) {
			string x = reverse(str1);
			string y = reverse(str2);

			if (x[0] == '+') { x = string(x.c_str() + 1); }
			if (y[0] == '+') { y = string(y.c_str() + 1); }

			if (x.size() < y.size()) {
				for (int i = 0; i < y.size() - x.size(); ++i) {
					x.push_back('0');
				}
			}
			else {
				for (int i = 0; i < x.size() - y.size(); ++i) {
					y.push_back('0');
				}
			}
			return Compare(reverse(x), reverse(y), 1);
		}
		else {
			return Compare(string(str2.c_str() + 1), string(str1.c_str() + 1));
		}
	}
	else if ("DOUBLE" == type1)
	{
		wiz::StringTokenizer tokenizer1(str1, ".");
		wiz::StringTokenizer tokenizer2(str2, ".");

		string x = tokenizer1.nextToken();
		string y = tokenizer2.nextToken();

		string z = Compare(x, y);
		if ("= 0" == z)
		{
			x = tokenizer1.nextToken();
			y = tokenizer2.nextToken();

			if (x.size() < y.size()) {
				for (int i = 0; i < y.size() - x.size(); ++i) {
					x.push_back('0');
				}
			}
			else {
				for (int i = 0; i < x.size() - y.size(); ++i) {
					y.push_back('0');
				}
			}
			return Compare(x, y, 1);
		}
		else
		{
			return z;
		}
	}
	else if ("DATE" == type1)
	{
		wiz::StringTokenizer tokenizer1(str1, ".");
		wiz::StringTokenizer tokenizer2(str2, ".");

		for (int i = 0; i < 3; ++i) {
			const string x = tokenizer1.nextToken();
			const string y = tokenizer2.nextToken();

			const string comp = Compare(x, y);

			if (comp == "< 0") { return comp; }
			else if (comp == "> 0") { return comp; }
		}
		return "== 0";
	}

	return "ERROR";
}

inline string FindParameters(const vector<pair<string, string>>& parameters, const string& operand)
{
	for (int i = 0; i < parameters.size(); ++i) {
		if (wiz::String::startsWith(operand, "$parameter.")
			&& wiz::String::substring(operand, 11) == parameters[i].first) {
			return parameters[i].second;
		}
	}
	return "";
}
void operation(wiz::load_data::UserType& global, const vector<pair<string,string>>& parameters, const string& str, wiz::ArrayStack<string>& operandStack)
{
	if (!operandStack.empty() && operandStack.top() == "ERROR") { return; }
	
	if ("$EQ" == str) {
		string x, y;
		int idx = -1;
		x = operandStack.pop();
		y = operandStack.pop();
		
		if ('/' == x[0])
		{
			x = Find(&global, x);
		}

		{
			string temp = FindParameters(parameters, x);
			if (!temp.empty()) { x = temp; }
		}

		if ('/' == y[0])
		{
			y = Find(&global, y);
		} 
		{
			string temp = FindParameters(parameters, y);
			if (!temp.empty()) { y = temp; }
		}

		if (x == y) {
			operandStack.push("TRUE");
		}
		else {
			operandStack.push("FALSE");
		}
	}
	else if ("$NOTEQ" == str)
	{
		string x, y;
		x = operandStack.pop();
		y = operandStack.pop();

		if ('/' == x[0])
		{
			x = Find(&global, x);
		}
		{
			string temp = FindParameters(parameters, x);
			if (!temp.empty()) { x = temp; }
		}

		if ('/' == y[0])
		{
			y = Find(&global, y);
		}
		{
			string temp = FindParameters(parameters, y);
			if (!temp.empty()) { y = temp; }
		}

		if (x != y) {
			operandStack.push("TRUE");
		}
		else {
			operandStack.push("FALSE");
		}
	}
	else if ("$COMP<" == str)
	{
		string x, y;
		x = operandStack.pop();
		y = operandStack.pop();

		if ('/' == x[0])
		{
			x = Find(&global, x);
		}
		{
			string temp = FindParameters(parameters, x);
			if (!temp.empty()) { x = temp; }
		}

		if ('/' == y[0])
		{
			y = Find(&global, y);
		}
		{
			string temp = FindParameters(parameters, y);
			if (!temp.empty()) { y = temp; }
		}

		if (Compare(x, y) == "< 0") {
			operandStack.push("TRUE");
		}
		else
		{
			operandStack.push("FALSE");
		}
	}
	else if ("$COMP>" == str)
	{
		string x, y;
		x = operandStack.pop();
		y = operandStack.pop();

		if ('/' == x[0])
		{
			x = Find(&global, x);
		}
		{
			string temp = FindParameters(parameters, x);
			if (!temp.empty()) { x = temp; }
		}

		if ('/' == y[0])
		{
			y = Find(&global, y);
		}
		{
			string temp = FindParameters(parameters, y);
			if (!temp.empty()) { y = temp; }
		}

		if (Compare(x, y) == "> 0") {
			operandStack.push("TRUE");
		}
		else
		{
			operandStack.push("FALSE");
		}
	}
	else if ("$COMP<EQ" == str)
	{
		string x, y;
		x = operandStack.pop();
		y = operandStack.pop();

		if ('/' == x[0])
		{
			x = Find(&global, x);
		}
		{
			string temp = FindParameters(parameters, x);
			if (!temp.empty()) { x = temp; }
		}

		if ('/' == y[0])
		{
			y = Find(&global, y);
		}
		{
			string temp = FindParameters(parameters, y);
			if (!temp.empty()) { y = temp; }
		}

		if (Compare(x, y) == "< 0" || Compare(x, y) == "== 0") {
			operandStack.push("TRUE");
		}
		else
		{
			operandStack.push("FALSE");
		}
	}
	else if ("$COMP>EQ" == str)
	{
		string x, y;
		x = operandStack.pop();
		y = operandStack.pop();

		if ('/' == x[0])
		{
			x = Find(&global, x);
		}
		{
			string temp = FindParameters(parameters, x);
			if (!temp.empty()) { x = temp; }
		}

		if ('/' == y[0])
		{
			y = Find(&global, y);
		}
		{
			string temp = FindParameters(parameters, y);
			if (!temp.empty()) { y = temp; }
		}

		if (Compare(x, y) == "> 0" || Compare(x, y) == "== 0") {
			operandStack.push("TRUE");
		}
		else
		{
			operandStack.push("FALSE");
		}
	}
	else if ("$add" == str)
	{
		string x, y;
		x = operandStack.pop();
		y = operandStack.pop();

		if ('/' == x[0])
		{
			x = Find(&global, x);
		}
		{
			string temp = FindParameters(parameters, x);
			if (!temp.empty()) { x = temp; }
		}

		if ('/' == y[0])
		{
			y = Find(&global, y);
		}
		{
			string temp = FindParameters(parameters, y);
			if (!temp.empty()) { y = temp; }
		}

		if (GetType(x) == GetType(y) && (GetType(y) == "INTEGER")) { /// only integer? -> BigInteger?
			operandStack.push(wiz::toStr(atoll(x.c_str()) + atoll(y.c_str())));
		}
		else
		{
			operandStack.push("ERROR");
		}
	}
	else if( "$multiple" == str)
	{
		string x, y;
		x = operandStack.pop();
		y = operandStack.pop();

		if ('/' == x[0])
		{
			x = Find(&global, x);
		}
		{
			string temp = FindParameters(parameters, x);
			if (!temp.empty()) { x = temp; }
		}

		if ('/' == y[0])
		{
			y = Find(&global, y);
		}
		{
			string temp = FindParameters(parameters, y);
			if (!temp.empty()) { y = temp; }
		}

		if (GetType(x) == GetType(y) && (GetType(y) == "INTEGER")) { /// only integer? -> BigInteger?
			operandStack.push(wiz::toStr(atoll(x.c_str()) * atoll(y.c_str())));
		}
		else
		{
			operandStack.push("ERROR");
		}
	}
	else if ("$divide" == str)
	{
		string x, y;
		x = operandStack.pop();
		y = operandStack.pop();

		if ('/' == x[0])
		{
			x = Find(&global, x);
		}
		{
			string temp = FindParameters(parameters, x);
			if (!temp.empty()) { x = temp; }
		}

		if ('/' == y[0])
		{
			y = Find(&global, y);
		}
		{
			string temp = FindParameters(parameters, y);
			if (!temp.empty()) { y = temp; }
		}

		if (GetType(x) == GetType(y) && (GetType(y) == "INTEGER")) { /// only integer? -> BigInteger?
			operandStack.push(wiz::toStr(atoll(x.c_str()) / atoll(y.c_str())));
		}
		else
		{
			operandStack.push("ERROR");
		}
	}
	else if ("$modular" == str)
	{
		string x, y;
		x = operandStack.pop();
		y = operandStack.pop();

		if ('/' == x[0])
		{
			x = Find(&global, x);
		}
		{
			string temp = FindParameters(parameters, x);
			if (!temp.empty()) { x = temp; }
		}

		if ('/' == y[0])
		{
			y = Find(&global, y);
		}
		{
			string temp = FindParameters(parameters, y);
			if (!temp.empty()) { y = temp; }
		}

		if (GetType(x) == GetType(y) && (GetType(y) == "INTEGER")) { /// only integer? -> BigInteger?
			operandStack.push(wiz::toStr(atoll(x.c_str()) % atoll(y.c_str())));
		}
		else
		{
			operandStack.push("ERROR");
		}
	}
}

// todo - rename!
string ToBool(wiz::load_data::UserType& global, const vector<pair<string,string>>& parameters, const string& temp)
{
	wiz::ArrayStack<string> operandStack; // 피연산자
	wiz::ArrayStack<string> operatorStack; // 연산자

	wiz::StringTokenizer tokenizer(temp, { " ", "\n", "\t", "\r", "{", "=", "}" });
	vector<string> tokenVec;

	while (tokenizer.hasMoreTokens()) {
		tokenVec.push_back(tokenizer.nextToken());
	}

	for (int i = tokenVec.size() - 1; i >= 0; --i)
	{
		if ('$' != tokenVec[i][0] || wiz::String::startsWith( tokenVec[i], "$parameter.")) {
			operandStack.push(tokenVec[i]);
		}
		else
		{
			operatorStack.push(tokenVec[i]);
			operation(global, parameters, tokenVec[i], operandStack);
		}
	}

	return operandStack[0];
}

// todo -  vector< pair<string, string> > ??

int main(void)
{
	// data, event load..
	stack<EventInfo> eventStack;
	map<string, int> convert;
	wiz::load_data::UserType global;
	wiz::load_data::LoadData::LoadDataFromFile("test.txt", global);
	auto events = global.GetUserTypeItem("Event");
	auto Main = GetUserType(&global, "Main");
	//

	// event table setting?
	for (int i = 0; i < events.size(); ++i)
	{
		auto x = events[i].Get(0)->GetItem("id");
		if (!x.empty()) {
			//cout <<	x[0].Get(0) << endl;
			convert.insert(pair<string, int>(x[0].Get(0), i));
		}
		else {
			// error?
		}
	}

	// start from Main.
	{
		EventInfo info;
		info.eventUT = Main;
		info.nowUT = NULL;
		info.item_idx = 0;
		info.userType_idx = 0;
		info.parameters.push_back(
			make_pair("id", info.eventUT->GetUserTypeItem("$call")[0].Get(0)->GetItem("id")[0].Get(0)) 
		);
		info.id = info.parameters[0].second;
		eventStack.push(info);
	}

	// main loop
	while (!eventStack.empty())
	{
		EventInfo info = eventStack.top();
		string str;
		
		for (int i = 0; i < info.parameters.size(); ++i) {
			if (info.parameters[i].first == "id") {
				str = info.parameters[i].second;
				break;
			}
		}

		const int no = convert[str];
		bool pass = false;
		
		if (info.userType_idx >= events[no].Get(0)->GetUserTypeListSize())
		{
			eventStack.pop();
			continue;
		}
		
		cout << "idx " << info.userType_idx << " stack depth " << eventStack.size() << " info.id " << info.id << endl;
		cout << "depth " << info.depth << endl;
		for (int i = eventStack.top().userType_idx; i < events[no].Get(0)->GetUserTypeListSize(); ++i) {
			wiz::load_data::UserType* val = events[no].Get(0)->GetUserTypeList(i).Get(0);
			int state = 0;
			while (true) {
				//cout << val->GetName() << endl;
				if ("$call" == val->GetName()) {
					cout << "$call " << val->GetItem("id")[0].Get(0) << endl;
					info.id = val->GetItem("id")[0].Get(0);
					info.eventUT = events[no].Get(0);
					info.nowUT = NULL;
					info.userType_idx = 0;
					if (info.id != eventStack.top().id) {
						info.parameters.clear();
					}
					info.conditionStack.clear();

					/// 순서가 다를 수 있다??
					if (info.id != eventStack.top().id) {
						for (int j = 0; j < val->GetItemListSize(); ++j) {
							info.parameters.push_back(make_pair(val->GetItemList(j).GetName(), val->GetItemList(j).Get(0)));
						}
					}
					else {
						if (val->GetUserTypeListSize() > 0) {
							string temp = ToBool(global, info.parameters, val->GetUserTypeList(0).Get(0)->ToString());
							for (int j = 0; j < info.parameters.size(); ++j)
							{
								if (info.parameters[j].first == val->GetUserTypeList(0).GetName())
								{
									info.parameters[j].second = temp;
								}
							}
							cout << temp << endl;
						}
						eventStack.pop();
					}
					eventStack.top().userType_idx++;
					eventStack.push(info);
					pass = true;
					break;            
				}
				else if ("$assign" == val->GetName())
				{
					// to do..
				}
				else if ("$insert" == val->GetName())
				{
					// to do..
				}
				else if ("$if" == val->GetName()) // ToDo!!
				{
					string temp = val->GetUserTypeList(0).Get(0)->ToString();
					temp = ToBool(global, eventStack.top().parameters, temp);
					//cout << temp << endl;
					eventStack.top().conditionStack.push(temp);

					if ("TRUE" == temp)
					{
						eventStack.top().nowUT = val->GetUserTypeList(1).Get(0);
						val = eventStack.top().nowUT->GetUserTypeList(0).Get(0); // + break!
						eventStack.top().num = 1;
						state = 1;
						eventStack.top().depth++;
					}
					else if ("FALSE" == temp)
					{
						eventStack.top().userType_idx++;
						break;
					}
					else
					{
						// debug..
						cout << "Error Debug : " << temp << endl;
						return -1;
					}
				}
				else if ("$else" == val->GetName())
				{
					// if가 바로 앞에 나와야한다. - later?
					//
					if (!eventStack.top().conditionStack.empty() && "FALSE" == eventStack.top().conditionStack.top())
					{
						eventStack.top().nowUT = val->GetUserTypeList(0).Get(0);
						val = eventStack.top().nowUT->GetUserTypeList(0).Get(0);
						eventStack.top().num = 1;
						state = 2;
						eventStack.top().depth--;
					}
					else
					{
						break;
					}
				}
				else {
					if (state == 1 || state == 2)
					{
						if (eventStack.top().num < eventStack.top().nowUT->GetUserTypeListSize())
						{
							val = eventStack.top().nowUT->GetUserTypeList(eventStack.top().num).Get(0);
							eventStack.top().num++;
						}
						else
						{
							eventStack.top().depth = std::max(0, eventStack.top().depth - 1);
							state = 0;
							break;
						}
					}
					else {
						eventStack.top().depth = std::max(0, eventStack.top().depth - 1);
						break;
					}
				}
			}
			if (pass) { break; }
		}

		if (false == pass)  {
			eventStack.top().userType_idx++;
		}
	}


	return 0;
}
