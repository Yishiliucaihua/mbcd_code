#pragma once

#include <queue>
#include "./search_interface.h"

using std::queue;
using std::make_pair;
using std::max;
using std::min;

namespace core
{
	class mbcd : public search_interface
	{
	public:
		mbcd(info_reporter *const _ir, ST _prt, ST _met) : search_interface(_ir), pruneT(_prt), methodT(_met) {};
		void solve(const graph&);
		void set_I(const ST _mi)
		{
			this->maxI = _mi;
		};
	private:
		ST pruneT = 0;  // 0: disable pruning; 1: enable pruning
		// 0: normal search;
		// 1: decompose-based search;
		// 2: heuristic search;
		ST methodT = 0;
		ST maxI = 20;
		vertex_index_set tmpV, tmpVO, tmpVI;
		v_info tmpP, tmpC, tmpI;
		vs_list coreN, tmpL;
		unordered_map<VIT, ST> heuP;
		void method_0();
		void method_1();
		void method_11();
		void method_12(const ST);
		void method_2();
		void solve_heu(v_info&, v_info&);
		void solve_imp(v_info&, v_info&);
		bool is_balanced(const v_info&);
		void updatePC(v_info&, v_info&);
		void generateC(const VIT, const v_info&, v_info&);
		void updateP(const VIT, v_info&);
		bool prunePC(v_info&, v_info&);
		bool requireN(const VIT, const v_info&, const ST, const ST, bool=true);
		bool is_valid(v_info&, v_info&);
		void decompose();
		void analyzeC(const v_info&, const v_info&, vector<pair<VIT, LL>>&);
	};
}
