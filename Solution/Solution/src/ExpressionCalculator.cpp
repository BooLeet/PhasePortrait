#include "ExpressionCalculator.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <functional>
#include <algorithm>
#include <fstream>
#include <assert.h>

Expression::Term::Term(std::string aName, TermType aType)
{
	name = aName;
	type = aType;
}

static std::vector<char> openBrackets = { '(' };
static std::vector<char> closeBrackets = { ')' };
static std::vector<char> numbers = { '0','1','2','3','4','5','6','7','8','9','.' };
static std::vector<char> operators = { '+','-','*','/','^' };//,'=','>','<','!'
static std::map<std::string, int> priorities{ { "+", 10 }, { "-", 10 }, { "*", 20 }, { "/", 20 }, { "^", 30 } };

// Извлекает и возвращает верхушку стека
double StackPopReturn(std::stack<double>& stack)
{
	double value = stack.top();
	stack.pop();
	return value;
}

// Вывод термов на экран
void Expression::PrintTerms(const std::vector<Term>& terms)
{
	std::for_each(begin(terms), end(terms), [](Term n) {std::cout << n.name << ' '; });
	std::cout << std::endl;
}

// Определение типа по символу
Expression::TermType Expression::GetType(char c)
{
	if (c == ',')
		return TermType::Comma;

	if (std::find(begin(openBrackets), end(openBrackets), c) != end(openBrackets))
		return TermType::OpenBracket;

	if (std::find(begin(closeBrackets), end(closeBrackets), c) != end(closeBrackets))
		return TermType::CloseBracket;

	if (std::find(begin(numbers), end(numbers), c) != end(numbers))
		return TermType::Constant;

	if (std::find(begin(operators), end(operators), c) != end(operators))
		return TermType::Operator;

	return TermType::Variable;
}

// Конвертация строки выражения в вектор термов
std::vector<Expression::Term> Expression::ConvertToTerms(const std::string& expression)
{
	std::vector<Term> result;

	std::string currentTerm;
	TermType currentTermType = TermType::Constant;
	size_t expressionSize = expression.size();

	for (size_t i = 0; i < expressionSize; ++i)
	{
		char currentChar = expression[i];
		if (currentChar == ' ')
			continue;

		if (currentTerm.size() == 0)
		{
			currentTermType = GetType(currentChar);
			if (currentTermType == TermType::OpenBracket || currentTermType == TermType::CloseBracket)
			{
				std::string termName;
				termName.push_back(currentChar);
				result.push_back(Term(termName, currentTermType));

				continue;
			}
		}

		TermType tempType = GetType(currentChar);
		if (tempType != currentTermType && !(tempType == TermType::Constant && currentTermType == TermType::Variable))
		{
			if (tempType == TermType::OpenBracket && currentTermType == TermType::Variable)
			{
				currentTermType = TermType::Function;
			}
			result.push_back(Term(currentTerm, currentTermType));
			currentTerm.clear();

			i--;
			continue;
		}

		currentTerm.push_back(expression[i]);

		if (i == expressionSize - 1)
			result.push_back(Term(currentTerm, currentTermType));
	}

	return result;
}

// Определение приоритета терма
int Expression::TermPriority(const Term& type)
{
	return priorities[type.name];
}

// Конвертация вектора терма в постфиксный вид
std::vector<Expression::Term> Expression::ConvertToPostfix(const std::vector<Term>& terms)
{
	std::stack<Term> termStack;
	std::vector<Term> result;

	for (auto&& t : terms)
	{
		switch (t.type)
		{
		case TermType::Variable: case TermType::Constant: result.push_back(t); break;
		case TermType::OpenBracket: termStack.push(t); break;
		case TermType::Function: termStack.push(t); break;
		case TermType::CloseBracket:
			while (termStack.top().type != TermType::OpenBracket)
			{
				result.push_back(termStack.top());
				termStack.pop();
			}
			termStack.pop();
			break;

		case TermType::Operator:
			while (termStack.size() > 0 && (termStack.top().type == TermType::Function ||
				(termStack.top().type != TermType::OpenBracket && TermPriority(termStack.top()) >= TermPriority(t))))
			{
				result.push_back(termStack.top());
				termStack.pop();
			}
			termStack.push(t);
			break;
		}
	}
	while (termStack.size())
	{
		result.push_back(termStack.top());
		termStack.pop();
	}
	return result;
}

// Чтение выбражения из потока
Expression::expression Expression::ReadExpressionFromStream(std::istream& stream)
{
	expression result;
	std::map<std::string, double> variables;
	std::string expression;
	std::getline(stream, expression);

	result.first = expression;
	std::string line;
	while (std::getline(stream, line))
	{
		std::string variableName = "";
		std::string variableValue = "";
		bool variableNameRead = false;
		for (size_t i = 0; i < line.size(); ++i)
		{
			char currentChar = line[i];
			if (currentChar == ' ')
				continue;
			if (currentChar == '=')
			{
				variableNameRead = true;
				continue;
			}
			if (variableNameRead)
				variableValue.push_back(currentChar);
			else
				variableName.push_back(currentChar);
		}
		variables.insert(std::pair<std::string, double>(variableName, std::stod(variableValue)));
	}
	result.second = variables;
	return result;
}

//// Вычисление выражения по заданному выражению и ассоциативному массиву функций
//double EvaluateExpression(const expression& expr,std::map<std::string, function >& functions)
//{
//	std::vector<Term> postfixTerms = ConvertToPostfix(ConvertToTerms(expr.first));
//	std::stack<double> stack;
//
//	for (size_t i = 0; i < postfixTerms.size(); ++i)
//	{
//		switch (postfixTerms[i].type)
//		{
//		case TermType::Variable: stack.push(expr.second.at(postfixTerms[i].name)); break;
//		case TermType::Constant: stack.push(std::stod(postfixTerms[i].name)); break;
//		case TermType::Operator: case TermType::Function:
//			stack.push(functions[postfixTerms[i].name](stack));
//		}
//	}
//	return stack.top();
//}

Expression Expression::ReadExpression(std::istream& stream)
{
	Expression result;
	result.expr = ReadExpressionFromStream(stream);
	result.postfixTerms = ConvertToPostfix(ConvertToTerms(result.expr.first));

	return result;
}

double Expression::EvaluateExpression(std::map<std::string, calcFunction>& functions)
{
	std::stack<double> stack;

	for (size_t i = 0; i < postfixTerms.size(); ++i)
	{
		switch (postfixTerms[i].type)
		{
		case TermType::Variable: stack.push(expr.second.at(postfixTerms[i].name)); break;
		case TermType::Constant: stack.push(std::stod(postfixTerms[i].name)); break;
		case TermType::Operator: case TermType::Function:
			stack.push(functions[postfixTerms[i].name](stack));
		}
	}
	return stack.top();
}

std::pair<double, std::string> Expression::EvaluateExpressionAndGetError(std::map<std::string, calcFunction>& functions)
{
	std::stack<double> stack;

	for (size_t i = 0; i < postfixTerms.size(); ++i)
	{
		switch (postfixTerms[i].type)
		{
		case TermType::Variable: 
			if (expr.second.find(postfixTerms[i].name) == expr.second.end())
				return { 0, "variable " + postfixTerms[i].name + " is not defined" };
			stack.push(expr.second.at(postfixTerms[i].name)); break;
		case TermType::Constant: stack.push(std::stod(postfixTerms[i].name)); break;
		case TermType::Operator: case TermType::Function:
			stack.push(functions[postfixTerms[i].name](stack));
		}
	}
	return { stack.top(),"" };
}

std::string Expression::GetString() const
{
	return expr.first;
}

void Expression::SetVariable(std::string name, double val)
{
	auto iterator = expr.second.find(name);
	if (iterator == expr.second.end())
		expr.second.insert(std::pair<std::string, double>(name, val));
	else
		(*iterator).second = val;
}
