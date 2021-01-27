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

using expression = std::pair<std::string, std::map<std::string, double>>; // Символьное выражение {выражение(строка),переменные(название,значение)}
using function = std::function<double(std::stack<double>&)>; // Функция калькулятора, сама вытаскивает из стека необходимые параметры

// Извлекает и возвращает верхушку стека
double StackPopReturn(std::stack<double>& stack);

// Вывод термов на экран
void PrintTerms(const std::vector<Term>& terms);

// Определение типа по символу
TermType GetType(char c);

// Конвертация строки выражения в вектор термов
std::vector<Term> ConvertToTerms(const std::string& expression);

// Определение приоритета терма
int TermPriority(const Term& type);

// Конвертация вектора терма в постфиксный вид
std::vector<Term> ConvertToPostfix(const std::vector<Term>& terms);

// Чтение выбражения из потока
expression ReadExpression(std::istream& stream);

// Вычисление выражения по заданному выражению и ассоциативному массиву функций
double EvaluateExpression(const expression& expr, std::map<std::string, function >& functions);