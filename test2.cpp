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
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <list>
#include <unordered_map>
#include <boost/functional/hash.hpp>

namespace std{
	template<>
	struct hash<boost::iterator_range<__gnu_cxx::__normal_iterator<const char*, basic_string<char> > > >{
		size_t operator()(const boost::iterator_range<__gnu_cxx::__normal_iterator<const char*, basic_string<char> > >& key) const {
			return boost::hash_range(begin(key), end(key));
		}
		size_t operator()(const boost::iterator_range<__gnu_cxx::__normal_iterator<const char*, basic_string<char> > >& key) {
			return boost::hash_range(begin(key), end(key));
		}
	};
}

using namespace std;

TEST(Functional, comment) {
	typedef kaluun::template_tree<unordered_map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	template_type tpl;
	template_type::in_type template_text("{#####{{}}#####}{% set x = 5%}{#######}x={{x}}{#####}");
	template_type::out_type str_out;
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	template_type::context_type ctx;
	tpl.generate(ctx, str_out);

	EXPECT_STREQ("x=__dummy__value__", str_out.str().c_str());
}

TEST(Performance, parse_generate1) {
	typedef kaluun::template_tree<map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	string template_text;

	template_text.append(1000, 'a');
	template_text.append("{% set x = 1 %}");
	template_text.append(1000, 'b');
	for (int i = 0; i < 1000; i++) {
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
	list<int> rows;
	for (int i = 0; i < 100; i++)
		rows.push_back(i);
	ctx["rows"] = rows;

	//parse test
	boost::posix_time::ptime begin = boost::posix_time::microsec_clock::local_time();
	for (int i = 0; i < 100; i++) {
		template_type tpl;
		kaluun::parser<template_type>::parse_template(template_text, tpl);
	}
	//generate test
	template_type tpl;
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	auto parse_time = boost::posix_time::microsec_clock::local_time() - begin;
	begin = boost::posix_time::microsec_clock::local_time();
	{
		for (int i = 0; i < 100; i++) {
			template_type::out_type str_out;
			tpl.generate(ctx, str_out);
		}
	}
	auto generate_time = boost::posix_time::microsec_clock::local_time() - begin;
	stringstream buf1;
	buf1 << parse_time;
	::testing::Test::RecordProperty("ParseTime", buf1.str());
	stringstream buf2;
	buf2 << generate_time;
	::testing::Test::RecordProperty("GenerateTime", buf2.str());
}

TEST(Performance, parse1) {
	typedef kaluun::template_tree<map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	string template_text;

	template_text.append(1000, 'a');
	template_text.append("{% set x = 1 %}");
	template_text.append(1000, 'b');
	for (int i = 0; i < 10000; i++) {
		template_text.append("{% for row in rows %}");
		template_text.append(100, 'c');
		template_text.append("{{x}}");
		template_text.append("{% set x = x + 0.5 %}");
		template_text.append(100, 'd');
		template_text.append("{% if row / 5 < x %}");
		template_text.append(100, 'e');
		template_text.append("{{x}}");
		template_text.append("{% elif row - 50 > x %}");
		template_text.append(100, 'f');
		template_text.append("{{x}}");
		template_text.append("{% else %}");
		template_text.append(100, 'g');
		template_text.append("{{x}}");
		template_text.append("{% endif %}");
		template_text.append(100, 'h');
		template_text.append(1, '\n');
		template_text.append("{% endfor %}");
	}

	template_text.append("{#");
	template_text.append(100, '#');
	template_text.append("#}");
	template_text.append(1000, 'i');

	template_type::context_type ctx;
	list<int> rows;
	ctx["rows"] = rows;

	//parse test
	boost::posix_time::ptime begin = boost::posix_time::microsec_clock::local_time();
	for (int i = 0; i < 100; i++) {
		template_type tpl;
		kaluun::parser<template_type>::parse_template(template_text, tpl);
	}
	auto parse_time = boost::posix_time::microsec_clock::local_time() - begin;
	stringstream buf1;
	buf1 << parse_time;
	::testing::Test::RecordProperty("ParseTime", buf1.str());
}

TEST(Performance, holders_compare1) {
	typedef kaluun::template_tree<map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type1;  //string holder
	typedef boost::iterator_range<string::const_iterator> holder_t;
	typedef kaluun::template_tree<map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, holder_t> template_type2;

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
		kaluun::parser<template_type1>::parse_template(template_text, tpl1);
	}
	auto tpl1_parse_time = boost::posix_time::microsec_clock::local_time() - begin;

	begin = boost::posix_time::microsec_clock::local_time();
	for (int i = 0; i < 100; i++) {     //?????????????????????????????????????????????????????????????????????????????????
		template_type2 tpl2;
		kaluun::parser<template_type2>::parse_template(template_text, tpl2);
	}
	auto tpl2_parse_time = boost::posix_time::microsec_clock::local_time() - begin;
	if (tpl1_parse_time <= tpl2_parse_time) {
		cerr << "tpl1_parse_time: " << tpl1_parse_time << endl;
		cerr << "tpl2_parse_time: " << tpl2_parse_time << endl;
	}
	ASSERT_TRUE(tpl1_parse_time > tpl2_parse_time);
	//assert   both templates generate same result
	template_type1 tpl1;
	template_type2 tpl2;
	{
		kaluun::parser<template_type1>::parse_template(template_text, tpl1);
		kaluun::parser<template_type2>::parse_template(template_text, tpl2);

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
	if (tpl1_generate_time <= tpl2_generate_time) {
		cerr << "tpl1_generate_time: " << tpl1_generate_time << endl;
		cerr << "tpl2_generate_time: " << tpl2_generate_time << endl;
	}
	ASSERT_TRUE(tpl1_generate_time > tpl2_generate_time);
}

TEST(Performance, generate1) {
	typedef kaluun::template_tree<map, string, stringstream, kaluun::dummy_expression, kaluun::dummy_condition, string> template_type;

	string template_text;

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
	list<int> rows;
	for (int i = 0; i < 1000000; i++)
		rows.push_back(i);
	ctx["rows"] = rows;

	//parse test
	//generate test
	template_type tpl;
	kaluun::parser<template_type>::parse_template(template_text, tpl);
	boost::posix_time::ptime begin = boost::posix_time::microsec_clock::local_time();
	{
		for (int i = 0; i < 100; i++) {
			template_type::out_type str_out;
			tpl.generate(ctx, str_out);
		}
	}
	auto generate_time = boost::posix_time::microsec_clock::local_time() - begin;
	stringstream buf2;
	buf2 << generate_time;
	::testing::Test::RecordProperty("GenerateTime", buf2.str());
}
