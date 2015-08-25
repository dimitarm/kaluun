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
#include <boost/algorithm/string.hpp>

#ifndef PARSER_HPP_
#define PARSER_HPP_

namespace templater {

template<class Template>
struct parser {

	typedef Template templ_type;
	typedef typename Template::context_type context_type;
	typedef typename Template::out_type out_type;
	typedef typename Template::in_type in_type;
	typedef typename Template::holder_type holder_type;
	typedef typename templ_type::in_type::iterator iterator_type;
	typedef typename templ_type::in_type::const_iterator const_iterator_type;
	typedef typename Template::expression_type expression_type;
	typedef typename Template::condition_type condition_type;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct exception: public std::exception {
		std::string msg_;
		exception(const in_type& in, iterator_type pos, std::string msg) {
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
	in_type& in_;
	templ_type& tree_;

	parser(in_type& in, templ_type& template_tree) :
			in_(in), tree_(template_tree) {
	}

	iterator_type create_text_node(iterator_type pos, node_with_children<context_type, out_type>& cur_level_node) {
		std::shared_ptr<text_node<context_type, out_type, holder_type> > tx_node(new text_node<context_type, out_type, holder_type>);
		iterator_type begin = pos;

		bool open_bracket = false;
		while (pos != in_.end()) {
			if ((*pos == OPEN_BRACKET || *pos == PERCENT || *pos == DIEZ) && open_bracket){
				break;
			}
			else if(*pos == OPEN_BRACKET)
				open_bracket = true;
			else
				open_bracket = false;
			pos++;
		}
		if(pos == in_.end()){
			holder_type pure_text(begin, pos);
			tx_node->text_ = std::move(pure_text);
		}
		else{
			holder_type pure_text(begin, pos - 1);
			tx_node->text_ = std::move(pure_text);
		}
		if(tx_node->text_.size() > 0)
			cur_level_node.add_child(tx_node);
		return pos;
	}

	iterator_type create_variable_node(iterator_type pos, node_with_children<context_type, out_type>& cur_level_node) {
		holder_type var_text;
		if (pos == in_.end())
			throw exception(in_, pos, "cannot handle variable node");

		//extract variable name
		char quote = 0;
		std::string::size_type num_white_chars = 0;
		for (; pos != in_.end() && (*pos != CLOSE_BRACKET || quote != 0); pos++) {
			if (quote != 0) { //inside quote
				if (*pos == quote) {
					quote = 0;
				} else
					var_text += *pos;
			} else {
				if (*pos == QUOTE1 || *pos == QUOTE2) {
					//append num_white_chars to var name
					if (num_white_chars != 0) {
						var_text.append(num_white_chars, WHITE_CHAR);
						num_white_chars = 0;
					}
					quote = *pos;   //will check for corresponding same type of quote
				} else {
					if (*pos == WHITE_CHAR)
						num_white_chars++;
					else {
						//append num_white_chars to var name
						if (num_white_chars != 0) {
							var_text.append(num_white_chars, WHITE_CHAR);
							num_white_chars = 0;
						}
						var_text += *pos;
					}
				}
			}
		}
		pos++;
		if (pos == in_.end() || *pos != CLOSE_BRACKET) {
			throw exception(in_, pos, "unexpected end of template:unfinished end of node");
		}

		pos++;
		//todo logic to create variable or expression node
		std::shared_ptr<variable_node<context_type, out_type, holder_type> > v_node(new variable_node<context_type, out_type, holder_type>);
		v_node->name_ = var_text;
		cur_level_node.add_child(v_node);
//		std::shared_ptr<expression_node> ex_node(new expression_node);
//		//exprt expression
//		std::shared_ptr<exprtk_mj::expression> expr(new exprtk_mj::expression);
//		exprtk_mj::expression::parse(var_text, *expr.get());
//		ex_node->expression_ = expr;

		return pos;
	}

	iterator_type create_comment_node(iterator_type pos) {
		do{
			while (pos != in_.end() && *pos != DIEZ)
				pos++;
			pos++;
		}
		while (pos != in_.end() && *pos != CLOSE_BRACKET);
		pos++;
		return pos;
	}

	std::shared_ptr<node<context_type, out_type> > create_loop_node(std::string var, std::string local_var, node_with_children<context_type, out_type>& cur_level_node) { //for row in rows
		std::shared_ptr<for_loop_node<context_type, out_type, holder_type> > loop_node(new for_loop_node<context_type, out_type, holder_type>);
		loop_node->loop_variable_ = var;
		loop_node->local_loop_variable_ = local_var;
		cur_level_node.add_child(loop_node);
		return loop_node;
	}

	std::shared_ptr<node_with_children<context_type, out_type> > create_if_condition_node(iterator_type begin, std::vector<std::string>& tokens,
			node_with_children<context_type, out_type>& cur_level_node) {       //for row in rows
		std::string expression_string;
		for (int i = 1; i < tokens.size(); i++)
			expression_string += tokens[i];

		std::shared_ptr<if_node<context_type, out_type, condition_type> > c_node;
		if (tokens[0] == "if")
			c_node.reset(new if_node<context_type, out_type, condition_type>);
		else{
			c_node.reset(new elif_node<context_type, out_type, condition_type>);
			try{
				if_node<context_type, out_type, condition_type>& ifnode = dynamic_cast<if_node<context_type, out_type, condition_type>&>(cur_level_node);
				ifnode.else_node_ = c_node;
			}
			catch(std::bad_cast&){
				throw exception(in_, begin, std::string("template error: expected node was if or elif, got: ") + typeid(cur_level_node).name());
			}
		}
		condition_type::parse(expression_string, c_node->condition_);
		cur_level_node.add_child(c_node);
		return c_node;
	}

	std::shared_ptr<set_node<context_type, out_type, expression_type, holder_type> > create_set_node(iterator_type begin, std::vector<std::string>& tokens, node_with_children<context_type, out_type>& cur_level_node){
		std::string expression_string;
		for(int i = 1; i < tokens.size(); i++ )
			expression_string += tokens[i];
		std::string::size_type eq_pos = expression_string.find('=');
		if(eq_pos != std::string::npos){
			holder_type variable = expression_string.substr(0, eq_pos);
			expression_string.erase(0, eq_pos + 1); //erase variable name and '=' sign

			set_node<context_type, out_type, expression_type, holder_type>* seet_node = new set_node<context_type, out_type, expression_type, holder_type>;
			seet_node->variable_name_ = variable;
			expression_type::parse(expression_string, seet_node->expression_);
			std::shared_ptr<set_node<context_type, out_type, expression_type, holder_type> > ptr(seet_node);
			cur_level_node.add_child(ptr);
			return ptr;
		}
		else
			throw exception(in_, begin, "cannot parse set node");
	}

	std::pair<std::shared_ptr<node<context_type, out_type> >, int> create_statement_node(iterator_type begin, iterator_type end,
			node_with_children<context_type, out_type>& cur_level_node) {
		boost::iterator_range<iterator_type> range(begin, end);

		std::vector<std::string> tokens; // Search for tokens
		boost::split(tokens, range, boost::is_any_of(" \t"), boost::token_compress_on);
		//remove spaces at the beginning and at the end if any
		auto endd = tokens.end() - 1;
		if (endd->size() == 0)
			tokens.erase(endd);
		if (tokens.begin()->size() == 0)
			tokens.erase(tokens.begin());
		//to lower case
		std::for_each(tokens.begin(), tokens.end(), [](std::string &s) {
			boost::iterator_range<iterator_type> range(boost::iterator_range<iterator_type>(s.begin(), s.end()));
			boost::to_lower(range);
		});
		//for in
		if (tokens.size() == 4 && tokens[0] == "for" && tokens[2] == "in")
			return std::make_pair(create_loop_node(tokens[3], tokens[1], cur_level_node), 1);
		//endfor
		if (tokens.size() == 1 && tokens[0] == "endfor")
			return std::make_pair(std::shared_ptr<node<context_type, out_type> >(NULL), -1);
		//if, elif
		if (tokens[0] == "if" || tokens[0] == "elif")
			return std::make_pair(create_if_condition_node(begin, tokens, cur_level_node), 1);
		//else
		if (tokens.size() == 1 && tokens[0] == "else") {
			try{
				if_node<context_type, out_type, condition_type>& ifnode = dynamic_cast<if_node<context_type, out_type, condition_type>&>(cur_level_node);
				std::shared_ptr<node_with_children<context_type, out_type> > else_node(new node_with_children<context_type, out_type>);
				ifnode.else_node_ = else_node;
				cur_level_node.add_child(else_node);
				return std::make_pair(else_node, 1);
			}
			catch(std::bad_cast&){
				throw exception(in_, begin, std::string("template error: expected node was if or elif, got: ") + typeid(cur_level_node).name());
			}
		}
		//endif
		if (tokens.size() == 1 && tokens[0] == "endif") {
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
		if (tokens[0] == "set"){
			return std::make_pair(create_set_node(begin, tokens, cur_level_node), 0);
		}

		throw exception(in_, begin, "unknown node");
	}

	void parse_template() {
		iterator_type pos = in_.begin();
		node_with_children<context_type, out_type>* cur_level_node = &tree_.root_;

		while (pos != in_.end()) {
			pos = create_text_node(pos, *cur_level_node);  //text node
			if(pos == in_.end())
				break;
			if (*pos == OPEN_BRACKET) {	//variable node
				pos++;
				pos = create_variable_node(pos, *cur_level_node);
			} else if (*pos == PERCENT) { 	//statement node
				pos++;
				iterator_type begin = pos;
				for (; *pos != PERCENT; pos++) { }
				if (pos == in_.end())
					throw exception(in_, pos, std::string("cannot find second open bracket"));
				std::shared_ptr<node<context_type, out_type> > st_node;
				int level;
				std::tie(st_node, level) = create_statement_node(begin, pos, *cur_level_node);
				//if (level == 0) //just add child node - stay on that level
				//nothing to do. node was added to cur_level_node
				if (level < 0) { //go as many levels up as necessary
					while(level < 0){
						if(cur_level_node == NULL)
							throw exception(in_, pos, std::string("not valid template"));
						cur_level_node = cur_level_node->parent_;
						level++;
					}
				}
				else if (level == 1) {  //add 1 more level
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
};

} //namespace templater

#endif /* PARSER_HPP_ */
