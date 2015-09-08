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

using namespace std;
//TEST(Functional, exprtk_condition) {
//	typedef templater::template_tree<templater::context<string>, string, stringstream, templater::exprtk::expression<templater::context<string> >,
//			templater::exprtk::condition<templater::context<string> >, string> template_type;
//
//	template_type tpl;
//	template_type::in_type template_text("{% set x = 5 %}{% if x > 5 %}bigger than five{% else %}less than five{% endif %}");
//	template_type::out_type str_out;
//	template_type::context_type ctx;
//	templater::parser<template_type> parser(template_text, tpl);
//	parser.parse_template();
//	tpl.generate(ctx, str_out);
//	EXPECT_STREQ("less than five", str_out.str().c_str());
//}
//
//TEST(Functional, exprtk_expression) {
//	typedef templater::template_tree<templater::context<string>, string, stringstream, templater::exprtk::expression<templater::context<string> >,
//			templater::dummy_condition<templater::context<string>, string>, string> template_type;
//
//	template_type tpl;
//	template_type::in_type template_text("{{b}} {% set x = 5 %}{% set y = 10 %}{% set z = x + y %}{{z}}{% set z = a + 1%} {{z}}{% set z = b%} {{z}} {{'sss'}} {{ x<y }}");
//	template_type::out_type str_out;
//	template_type::context_type ctx;
//	ctx["a"] = 26;
//	ctx["b"] = string("26.62");
//	templater::parser<template_type> parser(template_text, tpl);
//	parser.parse_template();
//	tpl.generate(ctx, str_out);
//	EXPECT_STREQ("26.62 15 27 26.620000000000001 sss 1", str_out.str().c_str());
//}
//
//TEST(Functional, comment) {
//	typedef templater::template_tree<templater::context<string>, string, stringstream, templater::exprtk::expression<templater::context<string> >,
//			templater::exprtk::condition<templater::context<string> >, string> template_type;
//
//	template_type tpl;
//	template_type::in_type template_text("{#####{{}}#####}{% set x = 5%}{#######}x={{x}}{#####}");
//	template_type::out_type str_out;
//	templater::parser<template_type> parser(template_text, tpl);
//	parser.parse_template();
//	template_type::context_type ctx;
//	tpl.generate(ctx, str_out);
//
//	EXPECT_STREQ("x=5", str_out.str().c_str());
//}

//TEST(Performance, parse_generate1) {
//	typedef templater::template_tree<templater::context<string>, string, stringstream, templater::exprtk::expression<templater::context<string> >,
//			templater::dummy_condition<templater::context<string>, string>, string> template_type;
//
//	string template_text;
//
//	template_text.append(1000, 'a');
//	template_text.append("{% set x = 1 %}");
//	template_text.append(1000, 'b');
//	for (int i = 0; i < 100; i++) {
//		template_text.append("{% for row in rows %}");
//		template_text.append(10, 'c');
//		template_text.append("{{x}}");
//		template_text.append("{% set x = x + 0.5 %}");
//		template_text.append(10, 'd');
//		template_text.append("{% if row / 5 < x %}");
//		template_text.append(10, 'e');
//		template_text.append("{{x}}");
//		template_text.append("{% elif row - 50 > x %}");
//		template_text.append(10, 'f');
//		template_text.append("{{x}}");
//		template_text.append("{% else %}");
//		template_text.append(10, 'g');
//		template_text.append("{{x}}");
//		template_text.append("{% endif %}");
//		template_text.append(10, 'h');
//		template_text.append(1, '\n');
//		template_text.append("{% endfor %}");
//	}
//
//	template_text.append("{#");
//	template_text.append(100, '#');
//	template_text.append("#}");
//	template_text.append(1000, 'i');
//
//	template_type::context_type ctx;
//	list<int> rows;
//	for (int i = 0; i < 100; i++)
//		rows.push_back(i);
//	ctx["rows"] = rows;
//
//	//parse test
//	boost::posix_time::ptime begin = boost::posix_time::microsec_clock::local_time();
//	for (int i = 0; i < 100; i++) {
//		template_type tpl;
//		templater::parser<template_type> parser(template_text, tpl);
//		parser.parse_template();
//	}
//	//generate test
//	template_type tpl;
//	templater::parser<template_type> parser(template_text, tpl);
//	parser.parse_template();
//	auto parse_time = boost::posix_time::microsec_clock::local_time() - begin;
//	begin = boost::posix_time::microsec_clock::local_time();
//	{
//		for (int i = 0; i < 100; i++) {
//			template_type::out_type str_out;
//			tpl.generate(ctx, str_out);
//		}
//	}
//	auto generate_time = boost::posix_time::microsec_clock::local_time() - begin;
//	stringstream buf1;
//	buf1 << parse_time;
//	::testing::Test::RecordProperty("ParseTime", buf1.str());
//	cerr << "ParseTime: " << buf1.str() << endl;
//	stringstream buf2;
//	buf2 << generate_time;
//	::testing::Test::RecordProperty("GenerateTime", buf2.str());
//	cerr << "GenerateTime: " << buf2.str() << endl;
//}

TEST(Performance, holders_compare1) {
	typedef templater::template_tree<templater::context<string>, string, stringstream, templater::dummy_expression<templater::context<string>, string>,
			templater::dummy_condition<templater::context<string>, string>, string> template_type1;  //string holder
	typedef boost::iterator_range<string::const_iterator> holder_t;
	typedef templater::template_tree<templater::context<holder_t>, string, stringstream, templater::dummy_expression<templater::context<holder_t>, holder_t>,
			templater::dummy_condition<templater::context<holder_t>, holder_t>, holder_t> template_type2;

	string template_text;

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

	template_type1::context_type ctx1;
	template_type2::context_type ctx2;
	string rows_key = string("rows");
	list<int> rows;
	for (int i = 0; i < 100; i++)
		rows.push_back(i);
	ctx1["rows"] = rows;
	ctx2[rows_key] = rows;

	//parse test
	boost::posix_time::ptime begin = boost::posix_time::microsec_clock::local_time();
	for (int i = 0; i < 100; i++) {	//?????????????????????????????????????????????????????????????????????????????????
		template_type1 tpl1;
		templater::parser<template_type1> parser(template_text, tpl1);
		parser.parse_template();
	}
	auto tpl1_parse_time = boost::posix_time::microsec_clock::local_time() - begin;

	begin = boost::posix_time::microsec_clock::local_time();
	for (int i = 0; i < 100; i++) {     //?????????????????????????????????????????????????????????????????????????????????
		template_type2 tpl2;
		templater::parser<template_type2> parser(template_text, tpl2);
		parser.parse_template();
	}
	auto tpl2_parse_time = boost::posix_time::microsec_clock::local_time() - begin;
	if (tpl1_parse_time <= tpl2_parse_time){
		cerr << "tpl1_parse_time: " << tpl1_parse_time << endl;
		cerr << "tpl2_parse_time: " << tpl2_parse_time << endl;
	}
	ASSERT_TRUE(tpl1_parse_time > tpl2_parse_time);
	//assert   both templates generate same result
	template_type1 tpl1;
	template_type2 tpl2;
	{
		templater::parser<template_type1> parser1(template_text, tpl1);
		parser1.parse_template();
		templater::parser<template_type2> parser2(template_text, tpl2);
		parser2.parse_template();

		template_type1::out_type str_out1;
		tpl1.generate(ctx1, str_out1);
		template_type2::out_type str_out2;
		tpl2.generate(ctx2, str_out2);
		EXPECT_STREQ(str_out1.str().c_str(), str_out2.str().c_str());
	}

	begin = boost::posix_time::microsec_clock::local_time();
	{
		for (int i = 0; i < 100; i++) {
			template_type1::out_type str_out;
			tpl1.generate(ctx1, str_out);
		}
	}
	auto tpl1_generate_time = boost::posix_time::microsec_clock::local_time() - begin;

	begin = boost::posix_time::microsec_clock::local_time();
	{
		for (int i = 0; i < 100; i++) {
			template_type2::out_type str_out;
			tpl2.generate(ctx2, str_out);
		}
	}
	auto tpl2_generate_time = boost::posix_time::microsec_clock::local_time() - begin;
//	if(tpl1_generate_time <= tpl2_generate_time){
//		cerr << "tpl1_generate_time: " << tpl1_generate_time << endl;
//		cerr << "tpl2_generate_time: " << tpl2_generate_time << endl;
//	}
	ASSERT_TRUE(tpl1_generate_time > tpl2_generate_time);
}

//TEST(Performance, generate1) {
//	typedef templater::template_tree<templater::context<string>, string, stringstream, templater::dummy_expression<templater::context<string>, string>,
//			templater::dummy_condition<templater::context<string>, string>, string> template_type;
//
//	string template_text;
//
//	template_text.append("{% set x = 1 %}");
//	template_text.append("{% for row in rows %}");
//	template_text.append("{{x}}");
//	template_text.append("{% set x = x + 0.5 %}");
//	template_text.append("{% if row / 5 < x %}");
//	template_text.append("{{x}}");
//	template_text.append("{% elif row - 50 > x %}");
//	template_text.append("{{x}}");
//	template_text.append("{% else %}");
//	template_text.append("{{x}}");
//	template_text.append("{% endif %}");
//	template_text.append("{% endfor %}");
//
//	template_type::context_type ctx;
//	list<int> rows;
//	for (int i = 0; i < 100000; i++)
//		rows.push_back(i);
//	ctx["rows"] = rows;
//
//	//parse test
//	//generate test
//	template_type tpl;
//	templater::parser<template_type> parser(template_text, tpl);
//	parser.parse_template();
//	boost::posix_time::ptime begin = boost::posix_time::microsec_clock::local_time();
//	{
//		for (int i = 0; i < 100; i++) {
//			template_type::out_type str_out;
//			tpl.generate(ctx, str_out);
//		}
//	}
//	auto generate_time = boost::posix_time::microsec_clock::local_time() - begin;
//	stringstream buf2;
//	buf2 << generate_time;
//	::testing::Test::RecordProperty("GenerateTime", buf2.str());
//	cerr << "GenerateTime: " << buf2.str() << endl;
//}
