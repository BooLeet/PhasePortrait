#pragma once

#include "ExpressionCalculator.h"
class DifferentialEquation
{
private:
	size_t order;
public:
	Expression rightSideExpression;

	DifferentialEquation(size_t order, Expression rightSideExpression) : order(order), rightSideExpression(rightSideExpression) {}

	size_t GetOrder() const;

};