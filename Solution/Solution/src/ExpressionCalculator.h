#pragma once
#include <string>
#include <map>
#include <functional>
#include <stack>
#include <vector>

enum class TermType
{
	Variable, Constant, OpenBracket, CloseBracket, Operator, Function, Comma
};

struct Term
{
	std::string name;
	TermType type;

	Term(std::string aName, TermType aType);
};

using expression = std::pair<std::string, std::map<std::string, double>>; // ���������� ��������� {���������(������),����������(��������,��������)}
using function = std::function<double(std::stack<double>&)>; // ������� ������������, ���� ����������� �� ����� ����������� ���������

// ��������� � ���������� �������� �����
double StackPopReturn(std::stack<double>& stack);

// ����� ������ �� �����
void PrintTerms(const std::vector<Term>& terms);

// ����������� ���� �� �������
TermType GetType(char c);

// ����������� ������ ��������� � ������ ������
std::vector<Term> ConvertToTerms(const std::string& expression);

// ����������� ���������� �����
int TermPriority(const Term& type);

// ����������� ������� ����� � ����������� ���
std::vector<Term> ConvertToPostfix(const std::vector<Term>& terms);

// ������ ���������� �� ������
expression ReadExpression(std::istream& stream);

// ���������� ��������� �� ��������� ��������� � �������������� ������� �������
double EvaluateExpression(const expression& expr, std::map<std::string, function >& functions);