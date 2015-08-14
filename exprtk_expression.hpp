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

#ifndef EXPRTK_EXPRESSION_HPP_
#define EXPRTK_EXPRESSION_HPP_

namespace templater {

namespace exprtk{

class expression: public templater::expression {
public:
	typedef double 									mj_type;
	typedef ::exprtk::symbol_table<mj_type> 		symbol_table_t;
	typedef ::exprtk::expression<mj_type> 			expression_t;
	typedef ::exprtk::parser<mj_type> 				parser_t;

	struct variable_function: public ::exprtk::ifunction<mj_type> {
		const std::string				variable_;
		expression&						expression_;

		variable_function(expression& expression, const std::string& variable): ::exprtk::ifunction<mj_type>(0), variable_(variable), expression_(expression) { }

		inline mj_type operator()() {
			if(expression_.value_provider_ != NULL)
				return expression_.value_provider_->get_value(variable_);
			else
				throw std::logic_error("value_provider has not been set");
		}
	};

	struct unknown_symbol_resolver_helper: public parser_t::unknown_symbol_resolver {
		std::unordered_set<std::string>& operands_;

		unknown_symbol_resolver_helper(std::unordered_set<std::string>& operands):operands_(operands){}
		virtual bool process(const std::string& unknown_symbol, usr_symbol_type& st, mj_type & default_value, std::string& error_message) {
			st = e_usr_variable_type;
			default_value = 13;
			error_message = "";
			operands_.insert(unknown_symbol);
			return true;
		}
	};

	struct value_provider{
		virtual mj_type get_value(const std::string& var) = 0;
	};

	symbol_table_t 							symbol_table_;
	expression_t 							expression_;
	std::list<variable_function>			functors_;

	mutable value_provider*					value_provider_;

public:
	expression() {
		expression_.register_symbol_table(symbol_table_);
		symbol_table_.add_constants();
		//todo symbol_table_ add built in functions
		value_provider_ = NULL;
	}

	mj_type value(value_provider*	value_provider) const {
		value_provider_ = value_provider;
		mj_type result = expression_.value();
		value_provider_ = NULL;
		return result;
	}
	static void parse(std::string expression_string, expression& expr) {
		unknown_symbol_resolver_helper 		resolver(expr.variables_);
		expression::parser_t 				parser;
		expression_t 						expression1;
		symbol_table_t 						symbol_table1;

		expression1.register_symbol_table(symbol_table1);
		parser.enable_unknown_symbol_resolver(&resolver);

		parser.compile(expression_string, expression1); //first run
		for(auto it = expr.variables_.begin(); it != expr.variables_.end(); it++){
			//variable_function functor(expr, *it);
			expr.functors_.emplace_back(expr, *it);
			expr.symbol_table_.add_function(*it, expr.functors_.back());
		}
		parser.disable_unknown_symbol_resolver();
		parser.compile(expression_string, expr.expression_);
	}
};

template<class Context>
class evaluator : public expression::value_provider{
	Context& ctx_;
	evaluator(Context& ctx):ctx_(ctx){}
public:
	static std::string value(const expression& expr, Context& ctx){
		expression::mj_type res = expr.value(new evaluator(ctx));
		return std::to_string(res);//todo ???
	}

	expression::mj_type get_value(const std::string& var){
		if(ctx_.has(var))
			return std::stod(ctx_[var]);  //todo conversion
		else
			return expression::mj_type();
	}
	virtual ~evaluator() {};
};
} } //namespace exprtk::templater

#endif /* EXPRTK_EXPRESSION_HPP_ */
