#pragma once
#include<iostream>
#include<stack>
#include<vector>
#include<string.h>
#include<stdlib.h>
#include<cmath>
#include<iomanip>
using namespace std;

class calc
{
	char *expression;
	
	enum token{OPAREN,ADD,SUB,MULTI,DIV,EXP,CPAREN,VALUE,EOL};

	void BinaryOp(token op, stack<double, vector<double>> &dataStack );
	token getOp(double &value,const double &x); //getOp函数返回token类型的值，告诉调用它的函数这是个什么符号，若是VALUE类型的返回到value的值里
	void display(double res)
	{

		if (res >= 1.5706 && res <= 1.5709) { cout << "π/2" << endl; return; }
		if (res >= 0.7853 && res <= 0.7855) {cout << "π/4" << endl; return;}
		if (res >= 3.1415 && res <= 3.1417) {cout << "π" << endl; return;}
		if (res >= 2.7182 && res <= 2.7184) { cout << "e" << endl; return; }
		if (res >= 1.3590 && res <= 1.3592) { cout << "e/2" << endl; return; }

		cout << fixed << setprecision(5) <<res << endl;
	}

public:
	calc(char *s)
	{
		expression = new char[strlen(s) + 1];
		strcpy_s(expression, strlen(s)+1,s);
	}
	calc(const char s[])
	{
		expression = new char[strlen(s) + 1];
		strcpy_s(expression, strlen(s)+1, s);
	}
	~calc() { delete expression; }

	double result(double x=0 ); //最主要的函数，给用户调用的共有函数，其返回运算结果。
	double integral();
};

double calc::result(double x)
{
	token lastOp, topOp; //lastOp表示取到的符号，topOp表示栈顶此时的符号
	double result_value, CurrentValue;//CurrentValue用来临时保存中间的运算结果，压入数值栈中
	stack<token, vector<token>> opStack;//运算符栈
	stack<double, vector<double>> dataStack; //运算数栈
	char* str = expression; //由于运算过程会改变expression的值，导致析构函数失效，因此先把expression保存在str中，最后再还原

	while ((lastOp = getOp(CurrentValue,x)) != EOL) //当 取出一个符号 此符号不是end of line，执行操作
	{
		switch (lastOp)
		{
			case VALUE: dataStack.push(CurrentValue); break;
			case CPAREN://遇到闭括号 ) 时，栈中开闭括号内所有的运算符都可以执行运算
				while (!opStack.empty() && (topOp = opStack.top()) != OPAREN) //☆记得！之后！把！左括号！pop出去！
				{
					opStack.pop();
					BinaryOp(topOp, dataStack);
				}
				if (opStack.top() == OPAREN) opStack.pop();
				else cerr << "缺少左括号！" << endl;
				break;
			case OPAREN: opStack.push(OPAREN); break;
			case EXP: opStack.push(EXP); break;
			case MULTI:case DIV:
			{
				while (!opStack.empty() && opStack.top() >= MULTI)
				{
					BinaryOp(opStack.top(), dataStack);
					opStack.pop();
				}
				opStack.push(lastOp);
				break;
			}
			case ADD:case SUB:
				while (!opStack.empty() && opStack.top() != OPAREN)
				{
					BinaryOp(opStack.top(), dataStack);
					opStack.pop();
				}
				opStack.push(lastOp);
		}
	}

	while (!opStack.empty()) {  BinaryOp(opStack.top(), dataStack); opStack.pop(); } //操作符栈中还有元素，取出来计算掉

	if (dataStack.empty())
	{
		cerr << "无结果！" << endl;
		return 0;
	}

	result_value = dataStack.top();
	dataStack.pop();

	if (!dataStack.empty())
	{
		cerr << "还有数没被计算！缺少操作符！" << endl;
		return 0;
	}

	expression = str;
	return result_value;
}

void calc::BinaryOp(token op,stack<double,vector<double>> &dataStack )
{
	double num1, num2;

	if (dataStack.empty())
	{
		cerr << "缺少右运算数！！！" << endl;
		exit(1);
	}
	else {num2 = dataStack.top(); dataStack.pop();}

	if (dataStack.empty())
	{
		cerr << "缺少左运算数！！！" << endl;
		exit(1);
	}
	else { num1 = dataStack.top(); dataStack.pop(); }

	switch (op)
	{
	case calc::ADD:dataStack.push(num1 + num2);
		break;
	case calc::SUB:dataStack.push(num1 - num2);
		break;
	case calc::MULTI:dataStack.push(num1*num2);
		break;
	case calc::DIV:dataStack.push(num1 / num2);
		break;
	case calc::EXP:dataStack.push(pow(num1, num2));
		break;
	}
}

calc::token calc::getOp(double &value,const double &x)
{
	while (*expression && *expression == ' ') //当表达式当前位置指向的字符不是NULL并且不是空格（用来跳过空格）
	{
		++expression;
	}
	if (*expression <= '9'&&*expression >= '0' || *expression=='.')  //处理数字的情况
	{
		value = 0;
		while (*expression >= '0' && *expression <= '9' || *expression=='.')
		{
			if (*expression >= '0' && *expression <= '9') value = value * 10 + *expression - '0';
			if (*expression == '.') //当遇到小数点后
			{
				++expression; //先跳过小数点
				int count = 1;
				while (*expression >= '0' && *expression <= '9')
				{
					value += double(*expression - '0')*pow(0.1, count);
					++count;
					++expression;
				}
				--expression;  //因为最后要做一个++expression，所以我们先减一下，然后刚好最后加一个就可以跳出前面那一层while了
							   //emmmmm，如果这里直接写break会不会更好一点呢？
			}
			++expression; 
		}   //最后一次循环结束expression应指向一个非数字的符号
		return calc::VALUE;
	}

	switch (*expression)  //处理运算符的情况
	{
	case'(':++expression; return calc::OPAREN; //因为是return了所以不用写break
		case')':++expression; return calc::CPAREN;
		case'+':++expression; return calc::ADD;
		case'-':++expression; return calc::SUB;
		case'*':++expression; return calc::MULTI;
		case'/':++expression; return calc::DIV;
		case'^':++expression; return calc::EXP;
	}

	if (*expression == 'π') //处理表达式中的π
	{
		value = 3.1415926536;
		++expression;
		return calc::VALUE;
	}

	if (*expression == 'e')
	{
		value = 2.718281828;
		++expression;
		return calc::VALUE;
	}

	if (*expression == 's')  //表达式中的sin，其规则是后面的东西用括号括起来
	{
		if (*(++expression) != 'i') { cerr << "sin符号不完整！！" << endl;  exit(1); }
		if(*(++expression)!='n') { cerr << "sin符号不完整！！" << endl;  exit(1); }
		++expression;  if (*expression != '(') { cerr << "sin后面的表达式要接括号！" << endl; exit(1); }
		char tmps[100]; int i = 0; //创建tmps字符串，把sin后面的表达式写进tmps
		while (*expression != ')')
		{
			tmps[i] = *expression;
			++i;
			++expression;
		}
		tmps[i] = ')';  tmps[i + 1] = '\0';  //给tmps加上\0

		++expression; //跳到 )后面那个

		double xofsin = 0;  calc exp1(tmps); xofsin = exp1.result(x);  //计算出sin后面表达式的值，保存在xofsin中
		value = sin(xofsin);  //用sin(xofsin)修改value的值

		return calc::VALUE;
	}

	if (*expression == 'x')  //处理积分时遇到表达式中有x的情况
	{
		value = x;
		++expression;
		return calc::VALUE;

	}

	return calc::EOL;
}

double calc::integral()
{
	double x, low, high, Res = 0;
	char s[1000];
	cout << "请输入积分表达式:";
	cin >> s;
	strcpy_s(expression,strlen(s)+1, s);
	cout << "请输入积分下限和上限" << endl<<"下限:";
	cin >> low;
	cout << "上限:";
	cin >> high;
	for (x = low; x <= high; x += 0.00001)
	{
		Res += result(x)*0.00001;
	}
	display(Res);
	return Res;
}
