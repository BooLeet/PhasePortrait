#pragma once
#include <string>
#include <map>
#include <functional>
#include <stack>
#include <vector>

using function = std::function<double(std::stack<double>&)>; // Функция калькулятора, сама вытаскивает из стека необходимые параметры

// Извлекает и возвращает верхушку стека
double StackPopReturn(std::stack<double>& stack);

class Expression
{
	using expression = std::pair<std::string, std::map<std::string, double>>; // Символьное выражение {выражение(строка),переменные(название,значение)}
	
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

	// Вывод термов на экран
	void PrintTerms(const std::vector<Term>& terms);

	// Определение типа по символу
	static TermType GetType(char c);

	// Конвертация строки выражения в вектор термов
	static std::vector<Term> ConvertToTerms(const std::string& expression);

	// Определение приоритета терма
	static int TermPriority(const Term& type);

	// Конвертация вектора терма в постфиксный вид
	static std::vector<Term> ConvertToPostfix(const std::vector<Term>& terms);

	// Чтение выбражения из потока
	static expression ReadExpressionFromStream(std::istream& stream);

public:
	// Чтение выбражения из потока
	static Expression ReadExpression(std::istream& stream);

	// Вычисление выражения по заданному выражению и ассоциативному массиву функций
	double EvaluateExpression(std::map<std::string, function >& functions);

	// Вычисление выражения по заданному выражению и ассоциативному массиву функций и запись первой попавшейся ошибки
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
//using expression = std::pair<std::string, std::map<std::string, double>>; // Символьное выражение {выражение(строка),переменные(название,значение)}
//using function = std::function<double(std::stack<double>&)>; // Функция калькулятора, сама вытаскивает из стека необходимые параметры
//
//// Извлекает и возвращает верхушку стека
//double StackPopReturn(std::stack<double>& stack);
//
//// Вывод термов на экран
//void PrintTerms(const std::vector<Term>& terms);
//
//// Определение типа по символу
//TermType GetType(char c);
//
//// Конвертация строки выражения в вектор термов
//std::vector<Term> ConvertToTerms(const std::string& expression);
//
//// Определение приоритета терма
//int TermPriority(const Term& type);
//
//// Конвертация вектора терма в постфиксный вид
//std::vector<Term> ConvertToPostfix(const std::vector<Term>& terms);
//
//// Чтение выбражения из потока
//expression ReadExpressionFromStream(std::istream& stream);
//
//// Вычисление выражения по заданному выражению и ассоциативному массиву функций
//double EvaluateExpression(const expression& expr, std::map<std::string, function >& functions);