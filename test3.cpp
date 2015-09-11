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
#include <strstream>
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
	typedef templater::template_tree<testmap, string, stringstream, templater::dummy_expression, templater::dummy_condition, holder_t> template_type;

	template_type tpl;
	template_type::in_type template_text("{blahblah}{{x}}qwerty{}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	std::string key("x");
	ctx[key] = std::string("123");
	std::string keyy("y");
	ctx[keyy] = std::string("7");
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("{blahblah}123qwerty{}}", str_out.str().c_str());
	template_text[0] = '_';
	template_text[12] = 'y';  //variable name should be kept as reference

	template_type::out_type str_out2;
	tpl.generate(ctx, str_out2);
	EXPECT_STREQ("_blahblah}7qwerty{}}", str_out2.str().c_str());
}

TEST(Functional, out1) {
	typedef templater::template_tree<testmap, std::string, std::ostrstream, templater::dummy_expression, templater::dummy_condition, std::string> template_type;

	template_type tpl;
	template_type::in_type template_text("{blahblah}{{x}}qwerty{}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx[std::string("x")] = std::string("123");
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("{blahblah}123qwerty{}}", str_out.rdbuf()->str());
}

