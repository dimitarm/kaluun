/*
 * node.hpp
 *
 *  Created on: Jun 23, 2015
 *      Author: dimitar
 */

#include <vector>
#include <string>
#include "context.hpp"
#include <boost/noncopyable.hpp>
#include <boost/range.hpp>
#include <boost/foreach.hpp>

#ifndef NODE_HPP_
#define NODE_HPP_

namespace kaluun {

template<class Context, class Out>
struct node_with_children;

template<class Context, class Out>
struct node: public boost::noncopyable {
	node_with_children<Context, Out>* parent_ = NULL;
	virtual ~node() {
	}  //need to have this, otherwise type is not polymorphic

	virtual void operator()(Context&, Out&) = 0;
};

template<class Context, class Out>
struct node_with_children: public node<Context, Out> {

	std::vector<node<Context, Out>* > child_nodes_;

	void add_child(node<Context, Out>* child) {
		child_nodes_.push_back(child);
		child->parent_ = this;
	}
	virtual ~node_with_children() {
		BOOST_FOREACH( auto n, child_nodes_)
		    {
		        delete n;
		    }
	}  //need to have this, otherwise type is not polymorphic

	virtual void operator()(Context& ctx, Out& out) {
		auto it = child_nodes_.begin();
		for (; it != child_nodes_.end(); it++) {
			(*it)->operator()(ctx, out);
		}
	}
};

//text
template<class Context, class Out, class Holder>
struct text_node: public node<Context, Out> {
	Holder text_;

	void operator()(Context&, Out& out) {
		out << text_;
	}

};

//specialize this for performance reasons
template<class Context>
struct text_node<Context, std::stringstream, boost::iterator_range<std::string::const_iterator> >: public node<Context, std::stringstream> {
	boost::iterator_range<std::string::const_iterator> text_;

	void operator()(Context&, std::stringstream& out) {
		out.write(&(*text_.begin()), text_.size());
	}

};


//set
template<class Context, class Out, class Expression, class Holder>
struct set_node: public node<Context, Out> {
	Holder variable_name_;
	Expression expression_;

	void operator()(Context& ctx, Out& ) {
		ctx[variable_name_] = expression_(ctx);
	}
};

//variable
template<class Context, class Out, class Holder>
struct variable_node: public node<Context, Out> {
	//Holder name_;
	Holder name_;

	void operator()(Context& ctx, Out& out) {
		if(!ctx.has(name_))
			throw std::logic_error(std::string("no value for: ").append(std::begin(name_), std::end(name_)));
		out << ctx[name_];
	}
};

//for ... in ...
template<class Context, class Out, class Holder>
struct for_loop_node: public node_with_children<Context, Out> {
	Holder loop_variable_;
	Holder local_loop_variable_;

	void operator()(Context& ctx, Out& out) {
		if(!ctx.has(loop_variable_))
			throw std::logic_error(std::string("no value for: ").assign(std::begin(loop_variable_), std::end(loop_variable_)));
		auto it = ctx[loop_variable_].begin();
		auto end = ctx[loop_variable_].end();

		Context subcontext = ctx.create_sub_context();
		for (; it != end; it++) {
			subcontext[local_loop_variable_] = *it;
			node_with_children<Context, Out>::operator()(subcontext, out);
		}
	}
};

//if
template<class Context, class Out, class Condition>
struct if_node: public node_with_children<Context, Out> {
	node<Context, Out>* else_node_ = nullptr;
	Condition condition_;

	void operator()(Context& ctx, Out& out) {
		if (condition_(ctx)) {
			//render all sub nodes without the else node
			for (auto it = this->child_nodes_.begin(); it != this->child_nodes_.end(); it++) {
				if (*it != else_node_) {
					(*it)->operator()(ctx, out);
				}
			}
		} else
			if(else_node_){ //check if we have else node!
				else_node_->operator()(ctx, out);
			}
	}
	virtual ~if_node() {
	}  //need to have this, otherwise type is not polymorphic
};
//elif
template<class Context, class Out, class Condition>
struct elif_node: public if_node<Context, Out, Condition> {
//everything is delegated to if_node
};

//expression
template<class Context, class Out, class Expression>
struct expression_node: public node<Context, Out> {
	Expression expression_;

	void operator()(Context& ctx, Out& out) {
		out << expression_(ctx);
	}
};

template<class Context, class Out>
struct root_node: public node_with_children<Context, Out> {
	node_with_children<Context, Out>* parent() const {
		throw std::logic_error("root node does not have parent");
	}
	void operator()(Context& ctx, Out& out) {
		node_with_children<Context, Out>::operator()(ctx, out);
	}
};

template<template <typename...> class Container, class In, class Out, class Expression, class Condition, class Holder>
struct template_tree {
	typedef context<Holder, Container<Holder, value> >	context_type;

	typedef Out out_type;
	typedef In in_type;
	typedef Holder holder_type;
	typedef Expression expression_type;
	typedef Condition condition_type;

	root_node<context_type, Out> root_;

	void generate(context_type& ctx, Out& out) {
		root_(ctx, out);
	}
}; }
#endif /* NODE_HPP_ */
