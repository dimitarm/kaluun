/*
 * test4_exprtk.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: dimitar
 */
#include "exprtk_expression.hpp"
#include "gtest/gtest.h"
#include "context.hpp"
#include "parser.hpp"
#include "template.hpp"
#include "expression.hpp"
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
TEST(Functional, exprtk_condition) {
	typedef templater::template_tree<templater::context<string>, string, stringstream, templater::exprtk::expression<templater::context<string> >,
			templater::exprtk::condition<templater::context<string> >, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{% set x = 5 %}{% if x > 5 %}bigger than five{% else %}less than five{% endif %}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("less than five", str_out.str().c_str());
}

TEST(Functional, exprtk_expression) {
	typedef templater::template_tree<templater::context<string>, string, stringstream, templater::exprtk::expression<templater::context<string> >,
			templater::dummy_condition<templater::context<string>, string>, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{{b}} {% set x = 5 %}{% set y = 10 %}{% set z = x + y %}{{z}}{% set z = a + 1%} {{z}}{% set z = b%} {{z}} {{'sss'}} {{ x<y }}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx["a"] = 26;
	ctx["b"] = string("26.62");
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("26.62 15 27 26.620000000000001 sss 1", str_out.str().c_str());
}





