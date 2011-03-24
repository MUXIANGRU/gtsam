/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    Factor-inl.h
 * @brief   
 * @author  Richard Roberts
 * @created Sep 1, 2010
 */

#pragma once

#include <iostream>
#include <boost/foreach.hpp>
#include <boost/make_shared.hpp>
#include <gtsam/inference/Factor.h>

namespace gtsam {

	/* ************************************************************************* */
	template<typename KEY>
	Factor<KEY>::Factor(const Factor<KEY>& f) :
		keys_(f.keys_) {
		assertInvariants();
	}

	/* ************************************************************************* */
	template<typename KEY>
	Factor<KEY>::Factor(const ConditionalType& c) :
		keys_(c.keys()) {
		assertInvariants();
	}

	/* ************************************************************************* */
	template<typename KEY>
	void Factor<KEY>::assertInvariants() const {
#ifndef NDEBUG
		// Check that keys are all unique
		std::multiset<Key> nonunique(keys_.begin(), keys_.end());
		std::set<Key> unique(keys_.begin(), keys_.end());
		assert(nonunique.size() == unique.size() && std::equal(nonunique.begin(), nonunique.end(), unique.begin()));
#endif
	}

	/* ************************************************************************* */
	template<typename KEY>
	void Factor<KEY>::print(const std::string& s) const {
		std::cout << s << " ";
		BOOST_FOREACH(KEY key, keys_)
						std::cout << " " << key;
		std::cout << std::endl;
	}

	/* ************************************************************************* */
	template<typename KEY>
	bool Factor<KEY>::equals(const This& other, double tol) const {
		return keys_ == other.keys_;
	}

	/* ************************************************************************* */
#ifdef TRACK_ELIMINATE
	template<typename KEY>
	template<class COND>
	typename COND::shared_ptr Factor<KEY>::eliminateFirst() {
		assert(!keys_.empty());
		assertInvariants();
		KEY eliminated = keys_.front();
		keys_.erase(keys_.begin());
		assertInvariants();
		return typename COND::shared_ptr(new COND(eliminated, keys_));
	}

	/* ************************************************************************* */
	template<typename KEY>
	template<class COND>
	typename BayesNet<COND>::shared_ptr Factor<KEY>::eliminate(size_t nrFrontals) {
		assert(keys_.size() >= nrFrontals);
		assertInvariants();
		typename BayesNet<COND>::shared_ptr fragment(new BayesNet<COND> ());
		const_iterator it = this->begin();
		for (KEY n = 0; n < nrFrontals; ++n, ++it)
			fragment->push_back(COND::FromRange(it, const_iterator(this->end()), 1));
		if (nrFrontals > 0) keys_.assign(fragment->back()->beginParents(),
				fragment->back()->endParents());
		assertInvariants();
		return fragment;
	}
#endif

}
