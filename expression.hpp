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

namespace templater {
template<class Context, class Holder>
struct dummy_expression {
	static void parse(Holder, dummy_expression&) {
	}
	std::string operator()(Context&) {
		return "__dummy__value__";
	}

};

template<class Context, class Holder>
struct dummy_condition {
	static void parse(Holder, dummy_condition&) {
	}
	bool operator()(Context&) {
		return false;
	}
};
}  //namespace templater
#endif /* EXPRESSION_HPP_ */
