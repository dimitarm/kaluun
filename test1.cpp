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
#include <list>

TEST(Functional, parse1) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type template_text("{blahblah}{{x}}qwerty{}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx[std::string("x")] = std::string("123");
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("{blahblah}123qwerty{}}", str_out.str().c_str());
}

TEST(Functional, loop1) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type template_text("{% for v in values %}{{v}}{%endfor%}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx["values"] = std::string("123");
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("123", str_out.str().c_str());
}

TEST(Functional, context1) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	template_type tpl;
	template_type tpl1;
	template_type tpl2;
	template_type::in_type template_text("{% for v in values %}{{v}}{%endfor%}_{% for v in value2 %}{{v}}{% endfor %}_{{value2}}");
	template_type::in_type template_text1("{{v}}");
	template_type::in_type template_text2("{% for v in vv %}{{v}}{% endfor %}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	std::list<std::string> values;
	values.push_back(std::string("1"));
	values.push_back(std::string("2"));
	values.push_back(std::string("3"));
	ctx["values"] = values;
	ctx["value2"] = 5;
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	templater::parser<template_type> parser1(template_text, tpl1);
	parser1.parse_template();
	templater::parser<template_type> parser2(template_text, tpl2);
	parser2.parse_template();
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("123_5_5", str_out.str().c_str());

	try { tpl1.generate(ctx, str_out); }
	catch(std::exception& ex) {	EXPECT_TRUE(std::string(ex.what()).find("no value for: v") != std::string::npos); }

	try { tpl2.generate(ctx, str_out); }
	catch(std::exception& ex) {	EXPECT_TRUE(std::string(ex.what()).find("no value for: vv") != std::string::npos); }
}

TEST(Functional, context2) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type template_text("{% for v in values %}{{v}}{%endfor%}_{% for v in value2 %}{{v}}{%endfor%}_{{value2}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	std::list<int> values;
	values.push_back(1);
	values.push_back(2);
	values.push_back(3);
	ctx["values"] = values;
	ctx["value2"] = 5;
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("123_5_5", str_out.str().c_str());
}

//
//TEST(Functional, context3) {
//	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>,
//			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;
//
//	template_type tpl;
//	template_type::in_type template_text("{% for v in values %}{% set values = 5 %}{%endfor%}");
//	template_type::out_type str_out;
//	template_type::context_type ctx;
//	std::list<std::string> values;
//	values.push_back(std::string("1"));
//	values.push_back(std::string("2"));
//	values.push_back(std::string("3"));
//	ctx["values"] = values;
//	templater::parser<template_type> parser(template_text, tpl);
//	parser.parse_template();
//	//ctx["value1"] = 5;
//	tpl.generate(ctx, str_out);
//	EXPECT_STREQ("123_5_5", str_out.str().c_str());
//}

//TEST(Functional, context){
//	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>, templater::dummy_condition<templater::context, std::string>, std::string> template_type;
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

TEST(Functional, context4) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	template_type::context_type ctx;
	std::list<std::string> ints; ints.push_back("1");ints.push_back("2");ints.push_back("3");
	ctx["ints"] = ints;
	auto it = ctx["ints"].begin();

	EXPECT_STREQ("1", (*it).to_string().c_str());
	EXPECT_STREQ("2", (*(++it)).to_string().c_str());
	auto it2 = it;
	EXPECT_STREQ("2", (*it2).to_string().c_str());
	it2 = ++it;
	EXPECT_STREQ("3", (*it2).to_string().c_str());
	EXPECT_STREQ("3", (*it).to_string().c_str());
	++it2;
	it = it2;
	EXPECT_TRUE(it == ctx["ints"].end());
	EXPECT_TRUE(it2 == ctx["ints"].end());
	EXPECT_TRUE(ctx["ints"].end() == it--);
	EXPECT_STREQ("3", (*it).to_string().c_str());
	it2 = it--;
	EXPECT_STREQ("3", (*it2).to_string().c_str());
	EXPECT_STREQ("2", (*it).to_string().c_str());
	it2++;
	it++; it++;
	EXPECT_TRUE(it == ctx["ints"].end());
	EXPECT_TRUE(it2 == ctx["ints"].end());
	ctx["floats"] = std::list<float>();
	EXPECT_TRUE(ctx["floats"].end() != ctx["ints"].end());
	EXPECT_TRUE(ctx["floats"].end() == ctx["floats"].end());
}

TEST(Functional, elif_syntax1) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type template_text("abc{% if x > 5 %}biggerthanfive{% elif x > 3 %}biggerthanthree {%for x in xx%} {% else %}unknown{% endif %}efg");
	template_type::out_type str_out;
	templater::parser<template_type> parser(template_text, tpl);

	try {
		parser.parse_template();
		FAIL();
	} catch (std::exception& ex) {
		EXPECT_TRUE(std::string(ex.what()).find("template error: expected node was if or elif, got: ") != std::string::npos);
	}
}

TEST(Functional, elif_syntax2) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type template_text("{% if x > 5 %} template {% elif x < 2 %} template2 {% endif %}");
	template_type::out_type str_out;
	templater::parser<template_type> parser(template_text, tpl);

	parser.parse_template();
	SUCCEED();
}
//TEST(Functional, dummy_expression){
//	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>, templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;
//
//	template_type tpl;
//	template_type::in_type  template_text("{% if x > 5 %}biggerthanfive{% elif x > 3 %}biggerthanthree{% else %}unknown{% endif %} {{ x + y }}");
//	template_type::out_type str_out;
//	templater::parser<template_type> parser(template_text, tpl);
//	parser.parse_template();
//	template_type::context_type ctx;
//	ctx["x"] = std::string("123");
//	tpl.generate(ctx, str_out);
//
//	EXPECT_STREQ("unknown __dummy__value__", str_out.str().c_str());
//}

TEST(Functional, set) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type template_text("{% set x = y%}x={{x}}");
	template_type::out_type str_out;
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	template_type::context_type ctx;
	ctx["y"] = std::string("5");
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("x=__dummy__value__", str_out.str().c_str());
}

TEST(Functional, var_node) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type template_text("{{x}} {{'sss''d'\"'\" '\"'}} {{x>4}}");
	template_type::out_type str_out;
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	template_type::context_type ctx;
	ctx["x"] = std::string("5");
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("5 sssd' \" __dummy__value__", str_out.str().c_str());
}

int main(int argc, char **argv) {
	std::time_t now = std::time(nullptr);
	::testing::GTEST_FLAG(output) = std::string("xml:./test_output/").append(std::to_string(now)).append(".xml");
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

//todo improve context structure
//todo bring context while parsing

