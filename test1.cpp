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
	template_type::in_type  template_text("{% for v in values %}{{v}}{%endfor%}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx["values"] = std::string("123");
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("123", str_out.str().c_str());
}

TEST(Functional, context1){
	typedef templater::template_tree<templater::context, std::string, std::stringstream, templater::dummy_expression<std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type  template_text("{% for v in values %}{{v}}{%endfor%}_{% for v in value2 %}{{v}}{%endfor%}_{{value2}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	std::list<std::string>	values;
	values.push_back(std::string("1"));values.push_back(std::string("2"));values.push_back(std::string("3"));
	ctx["values"] = values;
	ctx["value2"] = 5;
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("123_5_5", str_out.str().c_str());
}

//TEST(Functional, context){
//	typedef templater::template_tree<templater::context, std::string, std::stringstream, templater::dummy_expression<std::string>, std::string> template_type;
//	template_type::context_type::value values;
//	{
//		template_type::context_type ctx;
//		ctx["values"] = std::string("123");
//		values = ctx["values"];
//	}
//	values.to_string();
//
//	SUCCEED();
//}
//
TEST(Functional, elif_syntax1){
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
//
//TEST(Functional, exprtk_expression){
//	typedef templater::template_tree<templater::context, std::string, std::stringstream, templater::exprtk::expression<std::string>, std::string> template_type;
//
//	template_type tpl;
//	template_type::in_type  template_text("{% if x > 5 %}abc{% endif %}");
//	template_type::out_type str_out;
//	template_type::context_type ctx;
//	ctx["x"] = std::string("5");
//	templater::parser<template_type> parser(template_text, tpl);
//	parser.parse_template();
//	tpl.generate(ctx, str_out);
//	EXPECT_STREQ("", str_out.str().c_str());
//
//	ctx["x"] = std::string("6");
//	tpl.generate(ctx, str_out);
//	EXPECT_STREQ("abc", str_out.str().c_str());
//}

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

