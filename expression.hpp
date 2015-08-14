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

namespace templater{
	template <class Holder>
	struct dummy_expression{
		static void parse(Holder , dummy_expression& ){

		}
		template<typename T, typename Context>
		bool value(Context& ){
			return false;
		}

		std::type_info get_result_type(){
			return typeid(bool);
		}

		virtual ~dummy_expression() {};
	};
}  //namespace templater
#endif /* EXPRESSION_HPP_ */
