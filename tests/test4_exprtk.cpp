/*
 * test4_exprtk.cpp
 *
 *  Created on: Sep 8, 2015
 *      Author: dimitar
 */
#include "gtest/gtest.h"
#include <kaluun/exprtk_expression.hpp>
#include <kaluun/context.hpp>
#include <kaluun/parser.hpp>
#include <kaluun/template.hpp>
#include <kaluun/expression.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <unordered_map>

using namespace std;

TEST(Functional, exprtk_condition) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::exprtk::expression<kaluun::context<string, unordered_map<string, kaluun::value>>>, kaluun::exprtk::condition<kaluun::context<string, unordered_map<string, kaluun::value>>>, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{% set x = 5 %}{% if x > 5 %}bigger than five{% else %}less than five{% endif %}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("less than five", str_out.str().c_str());
}

TEST(Functional, exprtk_expression) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::exprtk::expression<kaluun::context<string, unordered_map<string, kaluun::value>>>, kaluun::exprtk::condition<kaluun::context<string, unordered_map<string, kaluun::value>>>, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{{b}} {% set x = 5 %}{% set y = 10 %}{% set z = x + y %}{{z}}{% set z = a + 1%} {{z}}{% set z = b%} {{z}} {{'sss'}} {{ x<y }}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx["a"] = 26;
	ctx["b"] = string("26.62");
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("26.62 15 27 26.620000000000001 sss 1", str_out.str().c_str());
}

