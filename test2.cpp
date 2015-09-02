/*
 * test2.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: dimitar
 */
#include "gtest/gtest.h"
#include "context.hpp"
#include "parser.hpp"
#include "template.hpp"
#include "expression.hpp"
#include "exprtk_expression.hpp"
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

TEST(Functional, exprtk_condition) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::exprtk::expression<templater::context<std::string>, std::string>,
			templater::exprtk::condition<templater::context<std::string>, std::string>, std::string> template_type;

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
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::exprtk::expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type template_text("{% set x = 5 %}{% set y = 10 %}{% set z = x + y %}{{z}}{% set z = a + 1%} {{z}}{% set z = b%} {{z}}");
	template_type::out_type str_out;
	template_type::context_type ctx;
	ctx["a"] = 26;
	ctx["b"] = std::string("26.62");
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	tpl.generate(ctx, str_out);
	EXPECT_STREQ("15.000000 27.000000 26.620000", str_out.str().c_str());
}

TEST(Functional, comment) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::exprtk::expression<templater::context<std::string>, std::string>,
			templater::exprtk::condition<templater::context<std::string>, std::string>, std::string> template_type;

	template_type tpl;
	template_type::in_type template_text("{#####{{}}#####}{% set x = 5%}{#######}x={{x}}{#####}");
	template_type::out_type str_out;
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	template_type::context_type ctx;
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("x=5.000000", str_out.str().c_str());
}

TEST(Performance, parse_generate1) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::exprtk::expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	std::string template_text;

	template_text.append(1000, 'a');
	template_text.append("{% set x = 1 %}");
	template_text.append(1000, 'b');
	for (int i = 0; i < 100; i++) {
		template_text.append("{% for row in rows %}");
		template_text.append(10, 'c');
		template_text.append("{{x}}");
		template_text.append("{% set x = x + 0.5 %}");
		template_text.append(10, 'd');
		template_text.append("{% if row / 5 < x %}");
		template_text.append(10, 'e');
		template_text.append("{{x}}");
		template_text.append("{% elif row - 50 > x %}");
		template_text.append(10, 'f');
		template_text.append("{{x}}");
		template_text.append("{% else %}");
		template_text.append(10, 'g');
		template_text.append("{{x}}");
		template_text.append("{% endif %}");
		template_text.append(10, 'h');
		template_text.append(1, '\n');
		template_text.append("{% endfor %}");
	}

	template_text.append("{#");
	template_text.append(100, '#');
	template_text.append("#}");
	template_text.append(1000, 'i');

	template_type::context_type ctx;
	std::list<int> rows;
	for (int i = 0; i < 100; i++)
		rows.push_back(i);
	ctx["rows"] = rows;

	//parse test
	boost::posix_time::ptime begin = boost::posix_time::microsec_clock::local_time();
	for (int i = 0; i < 100; i++) {
		template_type tpl;
		templater::parser<template_type> parser(template_text, tpl);
		parser.parse_template();
	}
	//generate test
	template_type tpl;
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	auto parse_time = boost::posix_time::microsec_clock::local_time() - begin;
	begin = boost::posix_time::microsec_clock::local_time();
	{
		for (int i = 0; i < 100; i++) {
			template_type::out_type str_out;
			tpl.generate(ctx, str_out);
		}
	}
	auto generate_time = boost::posix_time::microsec_clock::local_time() - begin;
	std::stringstream buf1;
	buf1 << parse_time;
	::testing::Test::RecordProperty("ParseTime", buf1.str());
	std::cerr << "ParseTime: " << buf1.str() << std::endl;
	std::stringstream buf2;
	buf2 << generate_time;
	::testing::Test::RecordProperty("GenerateTime", buf2.str());
	std::cerr << "GenerateTime: " << buf2.str() << std::endl;
}

TEST(Performance, generate1) {
	typedef templater::template_tree<templater::context<std::string>, std::string, std::stringstream, templater::dummy_expression<templater::context<std::string>, std::string>,
			templater::dummy_condition<templater::context<std::string>, std::string>, std::string> template_type;

	std::string template_text;

	template_text.append("{% set x = 1 %}");
	template_text.append("{% for row in rows %}");
	template_text.append("{{x}}");
	template_text.append("{% set x = x + 0.5 %}");
	template_text.append("{% if row / 5 < x %}");
	template_text.append("{{x}}");
	template_text.append("{% elif row - 50 > x %}");
	template_text.append("{{x}}");
	template_text.append("{% else %}");
	template_text.append("{{x}}");
	template_text.append("{% endif %}");
	template_text.append("{% endfor %}");

	template_type::context_type ctx;
	std::list<int> rows;
	for (int i = 0; i < 100000; i++)
		rows.push_back(i);
	ctx["rows"] = rows;

	//parse test
	//generate test
	template_type tpl;
	templater::parser<template_type> parser(template_text, tpl);
	parser.parse_template();
	boost::posix_time::ptime begin = boost::posix_time::microsec_clock::local_time();
	{
		for (int i = 0; i < 100; i++) {
			template_type::out_type str_out;
			tpl.generate(ctx, str_out);
		}
	}
	auto generate_time = boost::posix_time::microsec_clock::local_time() - begin;
	std::stringstream buf2;
	buf2 << generate_time;
	::testing::Test::RecordProperty("GenerateTime", buf2.str());
	std::cerr << "GenerateTime: " << buf2.str() << std::endl;
}
