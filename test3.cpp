/*
 * test3.cpp
 *
 *  Created on: Sep 2, 2015
 *      Author: dimitar
 */

#include "gtest/gtest.h"
#include "context.hpp"
#include "parser.hpp"
#include "template.hpp"
#include "expression.hpp"
#include <boost/range.hpp>

using namespace std;
TEST(Functional_range, parse1) {
	typedef boost::iterator_range<string::const_iterator> holder_t;
	typedef templater::template_tree<templater::context<holder_t>, string, stringstream, templater::dummy_expression<templater::context<holder_t>, holder_t>,
			templater::dummy_condition<templater::context<holder_t>, holder_t>, holder_t> template_type;

	template_type tpl;
	template_type::in_type template_text("{blahblah}{{x}}qwerty{}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	std::string key("x");
	ctx[key] = std::string("123");
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("{blahblah}123qwerty{}}", str_out.str().c_str());
	template_text[0] = '_';
	template_type::out_type str_out2;
	tpl.generate(ctx, str_out2);
	EXPECT_STREQ("_blahblah}123qwerty{}}", str_out2.str().c_str());
}




