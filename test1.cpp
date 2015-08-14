/*
 * test1.cpp
 *
 *  Created on: Jul 20, 2015
 *      Author: dimitar
 */

#include "gtest/gtest.h"
#include "context.hpp"
#include "parser.hpp"
#include "template.hpp"
#include "expression.hpp"
//#include "exprtk_expression.hpp"

TEST(Functional, parse1){
	typedef templater::template_tree<templater::context, std::string, std::stringstream, templater::dummy_expression<std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type  template_text("{blahblah}{{x}}qwerty{}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx["x"] = std::string("123");
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("{blahblah}123qwerty{}}", str_out.str().c_str());
}

TEST(Functional, loop1){
	typedef templater::template_tree<templater::context, std::string, std::stringstream, templater::dummy_expression<std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type  template_text("abc{% for v in values %} {{v}}{%endfor%}efg");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx["values"] = std::string("123");
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("abc 1 2 3efg", str_out.str().c_str());
}

TEST(Functional, elif_syntax){
	typedef templater::template_tree<templater::context, std::string, std::stringstream, templater::dummy_expression<std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type  template_text("abc{% if x > 5 %}biggerthanfive{% elif x > 3 %}biggerthanthree {%for x in xx%} {% else %}unknown{% endif %}efg");
	template_type::out_type str_out;
	templater::parser<template_type> parser(template_text, tpl);

	try{
		parser.parse_template();
		FAIL();
	}
	catch(std::exception& ex){
		EXPECT_TRUE(std::string(ex.what()).find("template error: expected node was if or elif, got: ") != std::string::npos);
	}
}

TEST(Functional, dummy_expression){
	typedef templater::template_tree<templater::context, std::string, std::stringstream, templater::dummy_expression<std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type  template_text("{% if x > 5 %}biggerthanfive{% elif x > 3 %}biggerthanthree{% else %}unknown{% endif %}");
	template_type::out_type str_out;
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	template_type::context_type ctx;
	ctx["x"] = std::string("123");
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("unknown", str_out.str().c_str());
}

//TEST(Functional, set){
//	typedef templater::template_tree<templater::context, std::string, std::stringstream, templater::dummy_expression<std::string>, std::string> template_type;
//
//	template_type tpl;
//	template_type::in_type  template_text("{% set x = y%}x={{x}}");
//	template_type::out_type str_out;
//	templater::parser<template_type> parser(template_text, tpl);
//	parser.parse_template();
//	template_type::context_type ctx;
//	ctx["y"] = std::string("5");
//	tpl.generate(ctx, str_out);
//
//	EXPECT_STREQ("x=5", str_out.str().c_str());
//}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

