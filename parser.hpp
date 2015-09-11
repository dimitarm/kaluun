/*
 * parser.hpp
 *
 *  Created on: Jun 23, 2015
 *      Author: dimitar
 */

#include "template.hpp"
#include <ostream>
#include <sstream>
#include <utility>
#include <tuple>
#include <typeinfo>
#include <vector>
#include <algorithm>
#include <boost/range.hpp>
#include <boost/range/algorithm/find.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <cctype>

#ifndef PARSER_HPP_
#define PARSER_HPP_

namespace kaluun {

template<class Template>
struct parser {

	typedef Template templ_type;
	typedef typename Template::context_type context_type;
	typedef typename Template::out_type out_type;
	typedef typename Template::in_type in_type;
	typedef typename Template::holder_type holder_type;
	typedef typename templ_type::in_type::const_iterator const_iterator_type;
	typedef typename Template::expression_type expression_type;
	typedef typename Template::condition_type condition_type;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct exception: public std::exception {
		std::string msg_;
		exception(const in_type& in, const_iterator_type pos, std::string msg) {
			std::stringstream buf;
			auto begin = in.begin();
			for (int count = 10; pos != begin && count; pos--, count--) {
			}

			buf << msg << std::string(": ");

			for (int count = 20; pos != in.end() && count; pos++, count--)
				buf << *pos;
			buf << std::endl;
			msg_ = buf.str();
		}
		const char* what() const noexcept (true) {
			return msg_.c_str();
		}
	};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	const char OPEN_BRACKET = '{';
	const char CLOSE_BRACKET = '}';
	const char PERCENT = '%';
	const char DIEZ = '#';
	const char QUOTE1 = '\x27';
	const char QUOTE2 = '\"';
	const char WHITE_CHAR = ' ';

//tokens
	const char TOKEN_FOR[3] = { 'f' , 'o', 'r'};
	const char TOKEN_IN[2] = {'i','n'};
	const char TOKEN_ENDFOR[6] = {'e','n','d','f','o','r'};
	const char TOKEN_IF[2] = {'i','f'};
	const char TOKEN_ELIF[4] = {'e','l','i','f'};
	const char TOKEN_ELSE[4] = {'e','l','s','e'};
	const char TOKEN_ENDIF[5] = {'e','n','d','i','f'};
	const char TOKEN_SET[3] = {'s','e','t'};

	in_type& in_;
	templ_type& tree_;

	parser(in_type& in, templ_type& template_tree) :
			in_(in), tree_(template_tree) {
	}

	const_iterator_type create_text_node(const_iterator_type pos, node_with_children<context_type, out_type>& cur_level_node) {
		std::shared_ptr<text_node<context_type, out_type, holder_type> > tx_node(new text_node<context_type, out_type, holder_type>);
		const_iterator_type begin = pos;

		bool open_bracket = false;
		while (pos != in_.end()) {
			if ((*pos == OPEN_BRACKET || *pos == PERCENT || *pos == DIEZ) && open_bracket) {
				break;
			} else if (*pos == OPEN_BRACKET)
				open_bracket = true;
			else
				open_bracket = false;
			pos++;
		}
		if (pos == in_.end()) {
			holder_type pure_text(begin, pos);
			tx_node->text_ = std::move(pure_text);
		} else {
			holder_type pure_text(begin, pos - 1);
			tx_node->text_ = std::move(pure_text);
		}
		if (tx_node->text_.size() > 0)
			cur_level_node.add_child(tx_node);
		return pos;
	}

	/**
	 * Parse an expression. Returns true if expression has quotation and false otherwise.
	 * If the expression has quotation quoted_str holds the unquoted string
	 * If not expr_str holds the expression
	 */
	template<class Holder>
	bool parse_expression(const_iterator_type& pos, Holder& expr_str, std::string& quoted_str) {
		bool res = false;
		while (*pos == WHITE_CHAR and pos != in_.end())
			pos++; //skip all white chars
		if (pos == in_.end())
			throw exception(in_, pos, "unexpected end of template");
		auto begin = pos; //start of expression
		auto end = pos; //end of expression
		//extract variable name
		int quote = 0;
		int num_white_chars = 0;
		for (; pos != in_.end() && (*pos != CLOSE_BRACKET || quote != 0); pos++) {
			if (quote != 0) { //inside quote
				if (*pos == quote)
					quote = 0;
				else
					quoted_str += *pos;
				num_white_chars = 0; //white characters in the tail
			} else {
				if (*pos == QUOTE1 || *pos == QUOTE2) {
					res = true;
					quoted_str.append(begin, end); //append all unquoted string so far
					end = begin; //make sure nothing will be added anymore
					quote = *pos;   //will check for corresponding same type of quote
				} else {
					if (res)
						quoted_str += *pos;
					else
						end++;

					if (*pos == WHITE_CHAR)
						num_white_chars++;
					else
						num_white_chars = 0;
				}
			}
		}
		if (pos == in_.end())
			throw exception(in_, pos, "unexpected end of template");
		pos++;
		if (*pos != CLOSE_BRACKET)
			throw exception(in_, pos, "unexpected symbol");
		pos++;
		if (res)
			quoted_str.erase(quoted_str.end() - num_white_chars, quoted_str.end());
		else
			expr_str = Holder(begin, end - num_white_chars);
		return res;
	}

	bool parse_expression(const_iterator_type& pos, std::string& expr_str, std::string& quoted_str) {
		bool res = false;
		while (*pos == WHITE_CHAR and pos != in_.end())
			pos++; //skip all white chars
		if (pos == in_.end())
			throw exception(in_, pos, "unexpected end of template");
		//extract variable name
		int quote = 0;
		int num_white_chars = 0;
		for (; pos != in_.end() && (*pos != CLOSE_BRACKET || quote != 0); pos++) {
			if (quote != 0) { //inside quote
				if (*pos == quote)
					quote = 0;
				else
					quoted_str += *pos;
				num_white_chars = 0; //white characters in the tail
			} else {
				if (*pos == QUOTE1 || *pos == QUOTE2) {
					res = true;
					quoted_str.append(expr_str); //append all unquoted string so far
					expr_str.clear(); //make sure nothing will be added anymore
					quote = *pos;   //will check for corresponding same type of quote
				} else {
					if (res)
						quoted_str += *pos;
					else
						expr_str += *pos;

					if (*pos == WHITE_CHAR)
						num_white_chars++;
					else
						num_white_chars = 0;
				}
			}
		}
		if (pos == in_.end())
			throw exception(in_, pos, "unexpected end of template");
		pos++;
		if (*pos != CLOSE_BRACKET)
			throw exception(in_, pos, "unexpected symbol");
		pos++;
		if (res)
			quoted_str.erase(quoted_str.end() - num_white_chars, quoted_str.end());
		else
			expr_str.erase(expr_str.end() - num_white_chars, expr_str.end());
		return res;
	}

	const_iterator_type create_variable_node(const_iterator_type pos, node_with_children<context_type, out_type>& cur_level_node) {
		holder_type expr_text;
		std::string unquoted_expr_text;

		std::shared_ptr<node<context_type, out_type> > v_node;
		if (parse_expression(pos, expr_text, unquoted_expr_text)) {
			text_node<context_type, out_type, std::string>* node = new text_node<context_type, out_type, std::string>;
			v_node.reset(node);
			node->text_ = unquoted_expr_text;
		} else {
			if (std::all_of(std::begin(expr_text), std::end(expr_text), isalnum)) { //todo use better algorithm to check if variable or not
				variable_node<context_type, out_type, holder_type>* node = new variable_node<context_type, out_type, holder_type>;
				v_node.reset(node);
				node->name_ = expr_text;
			} else {
				expression_node<context_type, out_type, expression_type>* ex_node = new expression_node<context_type, out_type, expression_type>;
				expression_type::parse(expr_text, ex_node->expression_);
				v_node.reset(ex_node);
			}
		}
		cur_level_node.add_child(v_node);

		return pos;
	}

	const_iterator_type create_comment_node(const_iterator_type pos) {
		do {
			while (pos != in_.end() && *pos != DIEZ)
				pos++;
			pos++;
		} while (pos != in_.end() && *pos != CLOSE_BRACKET);
		pos++;
		return pos;
	}

	std::shared_ptr<node<context_type, out_type> > create_loop_node(boost::iterator_range<const_iterator_type>& var, boost::iterator_range<const_iterator_type>& local_var, node_with_children<context_type, out_type>& cur_level_node) { //for row in rows
		std::shared_ptr<for_loop_node<context_type, out_type, holder_type> > loop_node(new for_loop_node<context_type, out_type, holder_type>);
		loop_node->loop_variable_ = holder_type(var.begin(), var.end());
		loop_node->local_loop_variable_ = holder_type(local_var.begin(), local_var.end());
		cur_level_node.add_child(loop_node);
		return loop_node;
	}

	std::shared_ptr<node_with_children<context_type, out_type> > create_if_condition_node(const_iterator_type expr_begin, const_iterator_type expr_end, bool ifnode,
			node_with_children<context_type, out_type>& cur_level_node) {       //for row in rows
		holder_type expression_string(expr_begin, expr_end);

		std::shared_ptr<if_node<context_type, out_type, condition_type> > c_node;
		if (ifnode)
			c_node.reset(new if_node<context_type, out_type, condition_type>);
		else {
			c_node.reset(new elif_node<context_type, out_type, condition_type>);
			try {
				if_node<context_type, out_type, condition_type>& ifnode = dynamic_cast<if_node<context_type, out_type, condition_type>&>(cur_level_node);
				ifnode.else_node_ = c_node;
			} catch (std::bad_cast&) {
				throw exception(in_, expr_begin, std::string("template error: expected node was if or elif, got: ") + typeid(cur_level_node).name());
			}
		}
		condition_type::parse(expression_string, c_node->condition_);
		cur_level_node.add_child(c_node);
		return c_node;
	}

	std::shared_ptr<set_node<context_type, out_type, expression_type, holder_type> > create_set_node(const_iterator_type begin, const_iterator_type end, node_with_children<context_type, out_type>& cur_level_node) {
		const_iterator_type eq_pos = boost::range::find(boost::make_iterator_range(begin, end), '=');
		if(eq_pos != end){
			auto variable = boost::trim_copy_if(boost::make_iterator_range(begin, eq_pos), boost::is_any_of(" \t")); //variable name
			auto expression_string = boost::trim_copy_if(boost::make_iterator_range(eq_pos + 1, end), boost::is_any_of(" \t")); //expression string

			std::shared_ptr<set_node<context_type, out_type, expression_type, holder_type> > node_ptr(new set_node<context_type, out_type, expression_type, holder_type>);
			node_ptr->variable_name_ = holder_type(variable.begin(), variable.end());
			expression_type::parse(holder_type(expression_string.begin(), expression_string.end()), node_ptr->expression_);
			cur_level_node.add_child(node_ptr);
			return node_ptr;
		} else
			throw exception(in_, begin, "cannot parse set node");
	}

	std::pair<std::shared_ptr<node<context_type, out_type> >, int> create_statement_node(const_iterator_type begin, const_iterator_type end,
			node_with_children<context_type, out_type>& cur_level_node) {
		boost::iterator_range<const_iterator_type> node_text(begin, end);

		std::vector<boost::iterator_range<const_iterator_type> > tokens(5); // Search for tokens
		boost::split(tokens, node_text, boost::is_any_of(" \t"), boost::token_compress_on);
		//remove spaces at the beginning and at the end if any
		if (tokens.back().size() == 0)
			tokens.erase(tokens.end() - 1);
		if (tokens.front().size() == 0)
			tokens.erase(tokens.begin());
		//for in
		if (tokens[0] == TOKEN_FOR && tokens[2] == TOKEN_IN && tokens.size() == 4)
			return std::make_pair(create_loop_node(tokens[3], tokens[1], cur_level_node), 1);
		//endfor
		else if (tokens[0] == TOKEN_ENDFOR && tokens.size() == 1)
			return std::make_pair(std::shared_ptr<node<context_type, out_type> >(NULL), -1);
		//if, elif
		else if (tokens[0] == TOKEN_IF && tokens.size() > 1)
			return std::make_pair(create_if_condition_node(tokens[1].begin(), tokens.back().end(), true, cur_level_node), 1);
		else if (tokens[0] == TOKEN_ELIF && tokens.size() > 1)
			return std::make_pair(create_if_condition_node(tokens[1].begin(), tokens.back().end(), false, cur_level_node), 1);
		//else
		else if (tokens[0] == TOKEN_ELSE && tokens.size() == 1) {
			try {
				if_node<context_type, out_type, condition_type>& ifnode = dynamic_cast<if_node<context_type, out_type, condition_type>&>(cur_level_node);
				std::shared_ptr<node_with_children<context_type, out_type> > else_node(new node_with_children<context_type, out_type>);
				ifnode.else_node_ = else_node;
				cur_level_node.add_child(else_node);
				return std::make_pair(else_node, 1);
			} catch (std::bad_cast&) {
				throw exception(in_, begin, std::string("template error: expected node was if or elif, got: ") + typeid(cur_level_node).name());
			}
		}
		//endif
		else if (tokens[0] == TOKEN_ENDIF && tokens.size() == 1) {
			int count = -1;
			node_with_children<context_type, out_type>* n = &cur_level_node;
			while (typeid(*n) != typeid(if_node<context_type, out_type, condition_type> )) {
				if (n->parent_ == NULL)
					throw exception(in_, begin, std::string("not valid template"));
				n = n->parent_;
				count--;
			}

			return std::make_pair(std::shared_ptr<node<context_type, out_type> >(NULL), count);
		}
		//set
		else if (tokens[0] == TOKEN_SET) {
			return std::make_pair(create_set_node(tokens[1].begin(), tokens.back().end(), cur_level_node), 0);
		}
		else
			throw exception(in_, begin, "unknown node");
	}

	void parse_template() {
		const_iterator_type pos = in_.begin();
		node_with_children<context_type, out_type>* cur_level_node = &tree_.root_;

		while (pos != in_.end()) {
			pos = create_text_node(pos, *cur_level_node);  //text node
			if (pos == in_.end())
				break;
			if (*pos == OPEN_BRACKET) {	//variable node
				pos++;
				pos = create_variable_node(pos, *cur_level_node);
			} else if (*pos == PERCENT) { 	//statement node
				pos++;
				const_iterator_type begin = pos;
				for (; *pos != PERCENT; pos++) {
				}
				if (pos == in_.end())
					throw exception(in_, pos, std::string("cannot find second open bracket"));
				std::shared_ptr<node<context_type, out_type> > st_node;
				int level;
				std::tie(st_node, level) = create_statement_node(begin, pos, *cur_level_node);
				//if (level == 0) //just add child node - stay on that level
				//nothing to do. node was added to cur_level_node
				if (level < 0) { //go as many levels up as necessary
					while (level < 0) {
						if (cur_level_node == NULL)
							throw exception(in_, pos, std::string("not valid template"));
						cur_level_node = cur_level_node->parent_;
						level++;
					}
				} else if (level == 1) {  //add 1 more level
					if (!dynamic_cast<node_with_children<context_type, out_type>*>(st_node.get()))
						throw exception(in_, pos, std::string("node_with_children_expected, got: ") + typeid(*(st_node.get())).name());
					cur_level_node = static_cast<node_with_children<context_type, out_type>*>(st_node.get());
				}
				pos++;
				if (*pos != CLOSE_BRACKET)
					throw exception(in_, pos, std::string("cannot find second open bracket"));
				pos++;
			} else if (*pos == DIEZ) {		//comment node
				pos++;
				pos = create_comment_node(pos);
			} else
				throw exception(in_, pos, std::string("unexpected symbol"));
		}
		if (cur_level_node != &tree_.root_)
			throw exception(in_, pos, std::string("not valid template"));
	}

private:
//utilities
}; }

#endif /* PARSER_HPP_ */
