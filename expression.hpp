/*
 * expression.hpp
 *
 *  Created on: Jul 6, 2015
 *      Author: dimitar
 */

#include "context.hpp"
#include <unordered_set>
#include <boost/lexical_cast.hpp>
#include <typeinfo>

#ifndef EXPRESSION_HPP_
#define EXPRESSION_HPP_

namespace kaluun {
struct dummy_expression {
	template<class Holder>
	static void parse(Holder, dummy_expression&) {
	}
	template<class Context>
	std::string operator()(Context&) {
		return "__dummy__value__";
	}

};

struct dummy_condition {
	template<class Holder>
	static void parse(Holder, dummy_condition&) {
	}
	template<class Context>
	bool operator()(Context&) {
		return false;
	}
};}
#endif /* EXPRESSION_HPP_ */
