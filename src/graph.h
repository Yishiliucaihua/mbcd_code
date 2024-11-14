#pragma once

#include <fstream>
#include <algorithm>
#include "./utils.h"

using std::ifstream;
using std::ofstream;
using std::stable_sort;

namespace core
{
	class graph
	{
	public:
		graph() {};
		void load(string);
		const vertex_set* data() const
		{
			return &this->vs;
		};
		void print_info() const;
		ST get_order(const VIT _v) const
		{
			return this->vsm.find(_v)->second;
		};
		ST get_ea() const
		{
			return this->ea;
		};
	private:
		ST va = 0, ea = 0, eas = 0, dm = 0, dmo = 0, dmi = 0;
		vertex_set vs;
		unordered_map<VIT, ST> vsm;
		void compute_order();
	};
}
