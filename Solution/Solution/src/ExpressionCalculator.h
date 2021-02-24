#pragma once
#include <string>
#include <map>
#include <functional>
#include <stack>
#include <vector>

using function = std::function<double(std::stack<double>&)>; // ������� ������������, ���� ����������� �� ����� ����������� ���������

// ��������� � ���������� �������� �����
double StackPopReturn(std::stack<double>& stack);

class Expression
{
	using expression = std::pair<std::string, std::map<std::string, double>>; // ���������� ��������� {���������(������),����������(��������,��������)}
	
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

	std::vector<Term> postfixTerms;
	expression expr;

	// ����� ������ �� �����
	void PrintTerms(const std::vector<Term>& terms);

	// ����������� ���� �� �������
	static TermType GetType(char c);

	// ����������� ������ ��������� � ������ ������
	static std::vector<Term> ConvertToTerms(const std::string& expression);

	// ����������� ���������� �����
	static int TermPriority(const Term& type);

	// ����������� ������� ����� � ����������� ���
	static std::vector<Term> ConvertToPostfix(const std::vector<Term>& terms);

	// ������ ���������� �� ������
	static expression ReadExpressionFromStream(std::istream& stream);

public:
	// ������ ���������� �� ������
	static Expression ReadExpression(std::istream& stream);

	// ���������� ��������� �� ��������� ��������� � �������������� ������� �������
	double EvaluateExpression(std::map<std::string, function >& functions);

	// ���������� ��������� �� ��������� ��������� � �������������� ������� ������� � ������ ������ ���������� ������
	std::pair<double,std::string> EvaluateExpressionAndGetError(std::map<std::string, function >& functions);

	std::string GetString() const;

	void SetVariable(std::string name, double val);
};

//enum class TermType
//{
//	Variable, Constant, OpenBracket, CloseBracket, Operator, Function, Comma
//};
//
//struct Term
//{
//	std::string name;
//	TermType type;
//
//	Term(std::string aName, TermType aType);
//};
//
//using expression = std::pair<std::string, std::map<std::string, double>>; // ���������� ��������� {���������(������),����������(��������,��������)}
//using function = std::function<double(std::stack<double>&)>; // ������� ������������, ���� ����������� �� ����� ����������� ���������
//
//// ��������� � ���������� �������� �����
//double StackPopReturn(std::stack<double>& stack);
//
//// ����� ������ �� �����
//void PrintTerms(const std::vector<Term>& terms);
//
//// ����������� ���� �� �������
//TermType GetType(char c);
//
//// ����������� ������ ��������� � ������ ������
//std::vector<Term> ConvertToTerms(const std::string& expression);
//
//// ����������� ���������� �����
//int TermPriority(const Term& type);
//
//// ����������� ������� ����� � ����������� ���
//std::vector<Term> ConvertToPostfix(const std::vector<Term>& terms);
//
//// ������ ���������� �� ������
//expression ReadExpressionFromStream(std::istream& stream);
//
//// ���������� ��������� �� ��������� ��������� � �������������� ������� �������
//double EvaluateExpression(const expression& expr, std::map<std::string, function >& functions);