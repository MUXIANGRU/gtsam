/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation, 
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/*
 * SymbolicFactorGraph.h
 *
 *  Created on: Oct 29, 2009
 *      Author: Frank Dellaert
 */

#pragma once

#include <gtsam/inference/BayesNet.h>
#include <gtsam/inference/IndexConditional.h>
#include <gtsam/inference/EliminationTree.h>

namespace gtsam {

	typedef BayesNet<IndexConditional> SymbolicBayesNet;
	typedef EliminationTree<IndexFactor> SymbolicEliminationTree;

	/** Symbolic IndexFactor Graph */
	class SymbolicFactorGraph: public FactorGraph<IndexFactor> {

	public:

		/** Construct empty factor graph */
		SymbolicFactorGraph() {
		}

		/** Construct from a BayesNet */
		SymbolicFactorGraph(const BayesNet<IndexConditional>& bayesNet);

		/** Push back unary factor */
		void push_factor(Index key);

		/** Push back binary factor */
		void push_factor(Index key1, Index key2);

		/** Push back ternary factor */
		void push_factor(Index key1, Index key2, Index key3);

		/** Push back 4-way factor */
		void push_factor(Index key1, Index key2, Index key3, Index key4);

		/**
		 * Construct from a factor graph of any type
		 */
		template<class FACTOR>
		SymbolicFactorGraph(const FactorGraph<FACTOR>& fg);

		/**
		 * Return the set of variables involved in the factors (computes a set
		 * union).
		 */
		FastSet<Index> keys() const;
	};

	/** Create a combined joint factor (new style for EliminationTree). */
	IndexFactor::shared_ptr CombineSymbolic(
			const FactorGraph<IndexFactor>& factors, const FastMap<Index,
					std::vector<Index> >& variableSlots);

	/**
	 * CombineAndEliminate provides symbolic elimination.
	 * Combine and eliminate can also be called separately, but for this and
	 * derived classes calling them separately generally does extra work.
	 */
	std::pair<BayesNet<IndexConditional>::shared_ptr, IndexFactor::shared_ptr>
	EliminateSymbolic(const FactorGraph<IndexFactor>&, size_t nrFrontals = 1);

	/* Template function implementation */
	template<class FACTOR>
	SymbolicFactorGraph::SymbolicFactorGraph(const FactorGraph<FACTOR>& fg) {
		for (size_t i = 0; i < fg.size(); i++) {
			if (fg[i]) {
				IndexFactor::shared_ptr factor(new IndexFactor(*fg[i]));
				push_back(factor);
			} else
				push_back(IndexFactor::shared_ptr());
		}
	}

} // namespace gtsam
