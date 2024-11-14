#pragma once

#include "./graph.h"

namespace core
{
	class search_interface
	{
	public:
		search_interface(info_reporter *const _ir) : ir(_ir) {};
		virtual void solve(const graph&) = 0;
		vertex_index_list result() const
		{
			return this->mbc;
		};
#ifdef debug
		void print_mbc()
		{
			if (this->vs == nullptr)
			{
				return;
			}

			stable_sort(this->mbc.begin(), this->mbc.end());

			for (const auto v : this->mbc)
			{
				const auto &oin = (*this->vs)[v];
				cout << v << ": ";

				for (const auto u : this->mbc)
				{
					if (u != v)
					{
						if (oin.first.find(u) != oin.first.end())
						{
							cout << u << " ";
						}
					}
				}
				cout << "| ";
				for (const auto u : this->mbc)
				{
					if (u != v)
					{
						if (oin.second.find(u) != oin.second.end())
						{
							cout << u << " ";
						}
					}
				}
				cout << endl;
			}
		};
#endif
	protected:
		double iters = 0.0;
		vertex_index_list mbc;
		info_reporter *const ir;
		const graph *g = nullptr;
		const vertex_set *vs = nullptr;
	};
}
