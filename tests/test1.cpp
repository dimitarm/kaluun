/*
 * test1.cpp
 *
 *  Created on: Jul 20, 2015
 *      Author: dimitar
 */

#include "gtest/gtest.h"
#include <kaluun/context.hpp>
#include <kaluun/parser.hpp>
#include <kaluun/template.hpp>
#include <kaluun/expression.hpp>
#include <list>
#include <unordered_map>
#include <cassert>

using namespace std;

TEST(Functional, parse1) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{blahblah}{{x}}qwerty{}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx[string("x")] = string("123");
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("{blahblah}123qwerty{}}", str_out.str().c_str());
}

TEST(Functional, loop1) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{% for v in values %}{{v}}{%endfor%}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx["values"] = string("123");
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("123", str_out.str().c_str());
}

TEST(Functional, context1) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type tpl;
	template_type tpl1;
	template_type tpl2;
	template_type::in_type template_text("{% for v in values %}{{v}}{%endfor%}_{% for v in value2 %}{{v}}{% endfor %}_{{value2}}");
	template_type::in_type template_text1("{{v}}");
	template_type::in_type template_text2("{% for v in vv %}{{v}}{% endfor %}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	list<string> values;
	values.push_back(string("1"));
	values.push_back(string("2"));
	values.push_back(string("3"));
	ctx["values"] = values;
	ctx["value2"] = 5;
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	kaluun::parser<template_type>::parse_template(template_text, tpl1);
	kaluun::parser<template_type>::parse_template(template_text, tpl2);
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("123_5_5", str_out.str().c_str());

	try {
		tpl1.generate(ctx, str_out);
	} catch (exception& ex) {
		EXPECT_TRUE(string(ex.what()).find("no value for: v") != string::npos);
	}

	try {
		tpl2.generate(ctx, str_out);
	} catch (exception& ex) {
		EXPECT_TRUE(string(ex.what()).find("no value for: vv") != string::npos);
	}
}

TEST(Functional, context2) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{% for v in values %}{{v}}{%endfor%}_{% for v in value2 %}{{v}}{%endfor%}_{{value2}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	list<int> values;
	values.push_back(1);
	values.push_back(2);
	values.push_back(3);
	ctx["values"] = values;
	ctx["value2"] = 5;
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("123_5_5", str_out.str().c_str());
}

//
//TEST(Functional, context3) {
//	typedef kaluun::template_tree<kaluun::context<string>, string, stringstream, kaluun::dummy_expression<kaluun::context<string>, string>,
//			kaluun::dummy_condition<kaluun::context<string>, string>, string> template_type;
//
//	template_type tpl;
//	template_type::in_type template_text("{% for v in values %}{% set values = 5 %}{%endfor%}");
//	template_type::out_type str_out;
//	template_type::context_type ctx;
//	list<string> values;
//	values.push_back(string("1"));
//	values.push_back(string("2"));
//	values.push_back(string("3"));
//	ctx["values"] = values;
//kaluun::parser<template_type>::parse_template(template_text, tpl);
//	//ctx["value1"] = 5;
//	tpl.generate(ctx, str_out);
//	EXPECT_STREQ("123_5_5", str_out.str().c_str());
//}

//TEST(Functional, context_values_outside_context){
//	typedef kaluun::template_tree<kaluun::context<string>, string, stringstream, kaluun::dummy_expression<kaluun::context<string>, string>, kaluun::dummy_condition<kaluun::context, string>, string> template_type;
//	template_type::context_type::value values;
//	{
//		template_type::context_type ctx;
//		ctx["values"] = string("123");
//		values = ctx["values"];
//	}
//	values.to_string();
//
//	SUCCEED();
//}
//

TEST(Functional, context4) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type::context_type ctx;
	list<string> ints;
	ints.push_back("1");
	ints.push_back("2");
	ints.push_back("3");
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
	it++;
	it++;
	EXPECT_TRUE(it == ctx["ints"].end());
	EXPECT_TRUE(it2 == ctx["ints"].end());
	ctx["floats"] = list<float>();
	EXPECT_TRUE(ctx["floats"].end() != ctx["ints"].end());
	EXPECT_TRUE(ctx["floats"].end() == ctx["floats"].end());
}

TEST(Functional, elif_syntax1) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type tpl;
	template_type::in_type template_text("abc{% if x > 5 %}biggerthanfive{% elif x > 3 %}biggerthanthree {%for x in xx%} {% else %}unknown{% endif %}efg");
	template_type::out_type str_out;

	try {
		kaluun::parser<template_type>::parse_template(template_text, tpl);
		FAIL();
	} catch (exception& ex) {
		EXPECT_TRUE(string(ex.what()).find("template error: expected node was if or elif, got: ") != string::npos);
	}
}

TEST(Functional, elif_syntax2) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{% if x > 5 %} template {% elif x < 2 %} template2 {% endif %}");
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	SUCCEED();

	template_type tpl1;
	template_type::in_type template_text1("{% if dummy %} template {% elif x < 2 %} template2 {% else %} template3 {% endif %}");
	template_type::out_type str_out1;
	kaluun::parser<template_type>::parse_template(template_text1, tpl1);
	template_type::context_type ctx;
	tpl1.generate(ctx, str_out1);
	EXPECT_STREQ(" template3 ", str_out1.str().c_str());
}
//TEST(Functional, dummy_expression){
//	typedef kaluun::template_tree<kaluun::context<string>, string, stringstream, kaluun::dummy_expression<kaluun::context<string>, string>, kaluun::dummy_condition<kaluun::context<string>, string>, string> template_type;
//
//	template_type tpl;
//	template_type::in_type  template_text("{% if x > 5 %}biggerthanfive{% elif x > 3 %}biggerthanthree{% else %}unknown{% endif %} {{ x + y }}");
//	template_type::out_type str_out;
//kaluun::parser<template_type>::parse_template(template_text, tpl);
//	template_type::context_type ctx;
//	ctx["x"] = string("123");
//	tpl.generate(ctx, str_out);
//
//	EXPECT_STREQ("unknown __dummy__value__", str_out.str().c_str());
//}

TEST(Functional, set) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{% set x = y%}x={{x}}");
	template_type::out_type str_out;
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	template_type::context_type ctx;
	ctx["y"] = string("5");
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("x=__dummy__value__", str_out.str().c_str());
}

TEST(Functional, var_node) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{{x}} {{'sss''d'\"'\" '\"'}} {{x>4}}");
	template_type::out_type str_out;
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	template_type::context_type ctx;
	ctx["x"] = string("5");
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("5 sssd' \" __dummy__value__", str_out.str().c_str());
}

TEST(Functional, container1) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{blahblah}{{x}}qwerty{}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx[string("x")] = string("123");
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("{blahblah}123qwerty{}}", str_out.str().c_str());
}


int main(int argc, char **argv) {
#ifdef NDEBUG
	time_t now = time(nullptr);
	string nows = to_string(now);
	::testing::GTEST_FLAG(output) = string("xml:./test_output/").append(nows.substr(nows.size() - 6)).append(".xml");
#endif
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

//todo support for unicode wchar
