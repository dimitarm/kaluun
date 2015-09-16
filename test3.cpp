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
#include <sstream>
#include <map>

template<class K, class V>
struct test_container {
	std::map<K, V> c_;

	V& operator[](const K& key) {
		return c_[key];
	}
};
//todo ugly! to be removed
template<class K, class V> struct testmap: public std::map<K, V> {
};

using namespace std;
TEST(Functional_range, parse1) {
	typedef boost::iterator_range<string::const_iterator> holder_t;
	typedef kaluun::template_tree<testmap, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, holder_t> template_type;

	template_type tpl;
	template_type::in_type template_text("{blahblah}{{x}}qwerty{}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	std::string key("x");
	ctx[key] = std::string("123");
	std::string keyy("y");
	ctx[keyy] = std::string("7");
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("{blahblah}123qwerty{}}", str_out.str().c_str());
	template_text[0] = '_';
	template_text[12] = 'y';  //variable name should be kept as reference

	template_type::out_type str_out2;
	tpl.generate(ctx, str_out2);
	EXPECT_STREQ("_blahblah}7qwerty{}}", str_out2.str().c_str());
}

TEST(Functional, out1) {
	typedef kaluun::template_tree<testmap, std::string, std::stringstream, kaluun::dummy_expression, kaluun::dummy_condition, std::string> template_type;

	template_type tpl;
	template_type::in_type template_text("{blahblah}{{x}}qwerty{}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx[std::string("x")] = std::string("123");
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("{blahblah}123qwerty{}}", str_out.str().c_str());
}

TEST(Functional_range, json1) {
	typedef kaluun::template_tree<testmap, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{{valuestr}},{{valueint}},{{valuebool}},{{valueobj.prop1}}:{% for l in valueobj.prop2 %}{{l}},{% endfor %}");
	template_type::out_type str_out;
	template_type::context_type ctx;

	char json[] = "{ \"valuestr\": \"15\","
			"		\"valueint\": 23,"
			"\"valuebool\": true,"
			"\"valueobj\": {"
			"\"prop1\":11,"
			"\"prop2\":[1, \"xexe\", false]"
			"} }";
	ctx.load_json(json);
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("15,23,1,11:1,xexe,0,", str_out.str().c_str());
}



