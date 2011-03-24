/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    GenericMultifrontalSolver-inl.h
 * @brief   
 * @author  Richard Roberts
 * @created Oct 21, 2010
 */

#pragma once

#include <gtsam/inference/GenericMultifrontalSolver.h>
#include <gtsam/inference/Factor-inl.h>
#include <gtsam/inference/JunctionTree-inl.h>
#include <gtsam/inference/BayesNet-inl.h>

#include <boost/foreach.hpp>

using namespace std;

namespace gtsam {

/* ************************************************************************* */
template<class FACTOR, class JUNCTIONTREE>
GenericMultifrontalSolver<FACTOR, JUNCTIONTREE>::GenericMultifrontalSolver(const FactorGraph<FACTOR>& factorGraph) :
    structure_(new VariableIndex(factorGraph)), junctionTree_(new JUNCTIONTREE(factorGraph, *structure_)) {}

/* ************************************************************************* */
template<class FACTOR, class JUNCTIONTREE>
GenericMultifrontalSolver<FACTOR, JUNCTIONTREE>::GenericMultifrontalSolver(
    const typename FactorGraph<FACTOR>::shared_ptr& factorGraph, const VariableIndex::shared_ptr& variableIndex) :
    structure_(variableIndex), junctionTree_(new JUNCTIONTREE(*factorGraph, *structure_)) {}

/* ************************************************************************* */
template<class FACTOR, class JUNCTIONTREE>
void GenericMultifrontalSolver<FACTOR, JUNCTIONTREE>::replaceFactors(const typename FactorGraph<FACTOR>::shared_ptr& factorGraph) {
  junctionTree_.reset(new JUNCTIONTREE(*factorGraph, *structure_));
}

/* ************************************************************************* */
template<class FACTOR, class JUNCTIONTREE>
typename JUNCTIONTREE::BayesTree::shared_ptr GenericMultifrontalSolver<
		FACTOR, JUNCTIONTREE>::eliminate(
		typename FactorGraph<FACTOR>::Eliminate function) const {
	typename JUNCTIONTREE::BayesTree::shared_ptr bayesTree(
			new typename JUNCTIONTREE::BayesTree);
	bayesTree->insert(junctionTree_->eliminate(function));
	return bayesTree;
}

/* ************************************************************************* */
template<class FACTOR, class JUNCTIONTREE>
typename FactorGraph<FACTOR>::shared_ptr GenericMultifrontalSolver<FACTOR,
		JUNCTIONTREE>::jointFactorGraph(const std::vector<Index>& js,
		Eliminate function) const {

	// We currently have code written only for computing the

	if (js.size() != 2) throw domain_error(
			"*MultifrontalSolver::joint(js) currently can only compute joint marginals\n"
				"for exactly two variables.  You can call marginal to compute the\n"
				"marginal for one variable.  *SequentialSolver::joint(js) can compute the\n"
				"joint marginal over any number of variables, so use that if necessary.\n");

	return eliminate(function)->joint(js[0], js[1], function);
}

/* ************************************************************************* */
template<class FACTOR, class JUNCTIONTREE>
typename FACTOR::shared_ptr GenericMultifrontalSolver<FACTOR, JUNCTIONTREE>::marginalFactor(
		Index j, Eliminate function) const {
	return eliminate(function)->marginalFactor(j, function);
}

}


