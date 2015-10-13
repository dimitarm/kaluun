/*
 * exprtk_expression.hpp
 *
 *  Created on: Jul 6, 2015
 *      Author: dimitar
 */

#include "expression.hpp"
#include <exprtk.hpp>
#include "context.hpp"
#include <list>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>
#include <cmath>

#ifndef EXPRTK_EXPRESSION_HPP_
#define EXPRTK_EXPRESSION_HPP_

namespace kaluun {

namespace exprtk{

template<class Context>
struct expression : public boost::noncopyable{
	typedef double 										result_type;
	typedef ::exprtk::symbol_table<result_type> 		symbol_table_t;
	typedef ::exprtk::expression<result_type> 			expression_t;
	typedef ::exprtk::parser<result_type> 				parser_t;

	struct variable_function: public ::exprtk::ifunction<result_type> , boost::noncopyable {
		const std::string&				variable_;
		Context*						context_;

		variable_function(const std::string& variable): ::exprtk::ifunction<result_type>(0), variable_(variable), context_(nullptr) { }
		inline void set_context(Context* target){
			context_ = target;
		}
		inline result_type operator()() {
			if(context_ != NULL)
				return boost::lexical_cast<result_type>(context_->operator[](variable_).to_string());
			else
				throw std::logic_error("context has not been set");
		}
	};

	struct unknown_symbol_resolver_helper: public parser_t::unknown_symbol_resolver , boost::noncopyable {
		std::unordered_set<std::string>& operands_;

		unknown_symbol_resolver_helper(std::unordered_set<std::string>& operands):operands_(operands){}
		virtual bool process(const std::string& unknown_symbol, usr_symbol_type& st, result_type & default_value, std::string& error_message) {
			st = e_usr_variable_type;
			default_value = 13;
			error_message = "";
			operands_.insert(unknown_symbol);
			return true;
		}
	};

	symbol_table_t 							symbol_table_;
	expression_t 							expression_;
	std::list<variable_function>			functors_;
	std::unordered_set<std::string> 		variables_;

	expression() {
		expression_.register_symbol_table(symbol_table_);
	}

	std::string operator()(Context& ctx) {
		for(auto itFunc = functors_.begin(); itFunc != functors_.end(); itFunc++)
			itFunc->set_context(&ctx);
		expression::result_type res = expression_.value();
		return boost::lexical_cast<std::string>(res);
	}

	template<class Holder>
	static void parse(Holder expression_string, expression& expr) {
		unknown_symbol_resolver_helper 		resolver(expr.variables_);
		expression::parser_t 					parser;
		expression_t 						expression1;
		symbol_table_t 						symbol_table1;

		expression1.register_symbol_table(symbol_table1);
		parser.enable_unknown_symbol_resolver(&resolver);

		parser.compile(std::string(expression_string), expression1); //first run
		for(auto it = expr.variables_.begin(); it != expr.variables_.end(); it++){
			expr.functors_.emplace_back(*it);
			expr.symbol_table_.add_function(*it, expr.functors_.back());
		}
		parser.disable_unknown_symbol_resolver();
		parser.compile(expression_string, expr.expression_);
	}

	static void parse(const std::string& expression_string, expression& expr) {
		unknown_symbol_resolver_helper 		resolver(expr.variables_);
		expression::parser_t 					parser;
		expression_t 						expression1;
		symbol_table_t 						symbol_table1;

		expression1.register_symbol_table(symbol_table1);
		parser.enable_unknown_symbol_resolver(&resolver);

		parser.compile(expression_string, expression1); //first run
		for(auto it = expr.variables_.begin(); it != expr.variables_.end(); it++){
			expr.functors_.emplace_back(*it);
			expr.symbol_table_.add_function(*it, expr.functors_.back());
		}
		parser.disable_unknown_symbol_resolver();
		parser.compile(expression_string, expr.expression_);
	}

};

template<class Context>
struct condition : public boost::noncopyable{
	typedef double 										result_type;
	typedef ::exprtk::symbol_table<result_type> 		symbol_table_t;
	typedef ::exprtk::expression<result_type> 			expression_t;
	typedef ::exprtk::parser<result_type> 				parser_t;

	struct variable_function: public ::exprtk::ifunction<result_type> , boost::noncopyable{
		const std::string&				variable_;
		Context*						context_;

		variable_function(const std::string& variable): ::exprtk::ifunction<result_type>(0), variable_(variable), context_(nullptr) { }

		inline void set_context(Context* target){
			context_ = target;
		}
		inline result_type operator()() {
			if(context_ != NULL)
				return std::stod(context_->operator[](variable_).to_string());
			else
				throw std::logic_error("context has not been set");
		}
	};

	struct unknown_symbol_resolver_helper: public parser_t::unknown_symbol_resolver , boost::noncopyable {
		std::unordered_set<std::string>& operands_;

		unknown_symbol_resolver_helper(std::unordered_set<std::string>& operands):operands_(operands){}
		virtual bool process(const std::string& unknown_symbol, usr_symbol_type& st, result_type & default_value, std::string& error_message) {
			st = e_usr_variable_type;
			default_value = 13;
			error_message = "";
			operands_.insert(unknown_symbol);
			return true;
		}
	};

	symbol_table_t 							symbol_table_;
	expression_t 							expression_;
	std::list<variable_function>			functors_;
	std::unordered_set<std::string> 		variables_;

	condition() {
		expression_.register_symbol_table(symbol_table_);
	}

	bool operator()(Context& ctx) {
		for(auto itFunc = functors_.begin(); itFunc != functors_.end(); itFunc++)
			itFunc->set_context(&ctx);
		return expression_.operator bool();
	}

	template<class Holder>
	static void parse(Holder expression_string, condition& expr) {
		unknown_symbol_resolver_helper 		resolver(expr.variables_);
		condition::parser_t 				parser;
		expression_t 						expression1;
		symbol_table_t 						symbol_table1;

		expression1.register_symbol_table(symbol_table1);
		parser.enable_unknown_symbol_resolver(&resolver);

		parser.compile(std::string(expression_string), expression1); //first run
		for(auto it = expr.variables_.begin(); it != expr.variables_.end(); it++){
			//variable_function functor(expr, *it);
			expr.functors_.emplace_back(*it);
			expr.symbol_table_.add_function(*it, expr.functors_.back());
		}
		parser.disable_unknown_symbol_resolver();
		parser.compile(expression_string, expr.expression_);
	}

	static void parse(const std::string& expression_string, condition& expr) {
		unknown_symbol_resolver_helper 		resolver(expr.variables_);
		condition::parser_t 				parser;
		expression_t 						expression1;
		symbol_table_t 						symbol_table1;

		expression1.register_symbol_table(symbol_table1);
		parser.enable_unknown_symbol_resolver(&resolver);

		parser.compile(expression_string, expression1); //first run
		for(auto it = expr.variables_.begin(); it != expr.variables_.end(); it++){
			//variable_function functor(expr, *it);
			expr.functors_.emplace_back(*it);
			expr.symbol_table_.add_function(*it, expr.functors_.back());
		}
		parser.disable_unknown_symbol_resolver();
		parser.compile(expression_string, expr.expression_);
	}

}; } }

#endif /* EXPRTK_EXPRESSION_HPP_ */
