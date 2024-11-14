#include "./mbcd.h"

using namespace core;

static bool vl_sort(const pair<VIT, LL> &_vl1, const pair<VIT, LL> &_vl2)
{
	return _vl1.second < _vl2.second;
}

void mbcd::solve(const graph &_g)
{
#ifdef mem_on
	this->ir->print_mem("graph", true);
#endif

	this->iters = 0;
	this->mbc.clear();
	this->g = &_g;
	this->vs = _g.data();
	this->coreN.clear();
	this->heuP.clear();

	switch (this->methodT)
	{
	case 1:  // decompose-based search
		this->method_1();
		break;
	case 2:  // heuristic search
		this->method_2();
		break;
	default:  // normal search
		this->method_0();
		break;
	}

	cout << "number of iterations:=>{" << this->iters << "}" << endl;
}

void mbcd::method_0()
{
	cout << "normal search" << endl;
	v_info P, C;
	ST va = (ST)this->vs->size();

	for (ST k = 0; k < va; ++k)
	{
		C.emplace(k, make_pair(0, 0));
	}
	this->solve_imp(P, C);
}

static const ST ips_valve = 2, c_max = 18, min_v = 1000;
static const double c_valve = 0.1, v_valve = 0.1, sv = 0.01;

void mbcd::method_1()
{
	auto get_info_inc = [this](const vertex_index_set &_N, const vertex_index_set &_O, ST &_eao, vector<double> &_info)
	{
		for (const auto v : _N)
		{
			for (const auto u : (*this->vs)[v].first)
			{
				if (_N.find(u) != _N.end())
				{
					++_eao;
				}
				if (_O.find(u) != _O.end())
				{
					++_eao;
				}
			}
		}

		for (const auto v : _O)
		{
			for (const auto u : (*this->vs)[v].first)
			{
				if (_N.find(u) != _N.end())
				{
					++_eao;
				}
			}
		}
		
		_info.emplace_back((double)_eao / (_N.size() + _O.size()));
	};

	cout << "decompose-based search" << endl;
	this->decompose();

	vertex_index_set N, O;
	vector<double> info;
	vector<ST> coreI;
	ST va = (ST)this->vs->size(), last = va - 1, eao = 0, ips = 0, cc = 0, bv = max(min_v, (ST)(v_valve * va));
	
	N.emplace(this->coreN[last].first);
	for (ST k = last - 1; k >= 0; --k)
	{
		if (this->coreN[(ULL)k + 1].second != this->coreN[k].second)
		{
			// touching a subset is enough
			if ((ST)(N.size() + O.size()) >= bv)
			{
				break;
			}

			// check special case
			if ((ST)(N.size() + O.size()) == this->coreN[(ULL)k + 1].second + 1)
			{
				this->method_11();
				return;
			}

			// check discontinuous coreness
			ST td = this->coreN[(ULL)k + 1].second - this->coreN[k].second;
			if (td > 2 && (double)td / this->coreN[k].second >= c_valve)
			{
				this->method_11();
				return;
			}

			get_info_inc(N, O, eao, info);
			coreI.emplace_back(k + 1);

			// touching a subset is enough
			++cc;
			if (cc >= c_max)
			{
				break;
			}

			O.insert(N.begin(), N.end());
			N.clear();
			N.emplace(this->coreN[k].first);
		}
		else
		{
			N.emplace(this->coreN[k].first);
		}
	}

	if (info.size() <= 3)
	{
		this->method_11();
	}
	else
	{
		// check special point
		ST coreS = va;
		last = (ST)info.size() - 2;

		for (ST k = 1; k <= last; ++k)
		{
			auto lk = k - 1, rk = k + 1;
			if (info[k] > info[lk] && info[k] > info[rk] && (((info[k] - info[lk]) / info[lk] >= sv) || ((info[k] - info[rk]) / info[rk] >= sv)))
			{
				++ips;
				if (ips >= ips_valve)
				{
					coreS = coreI[k];
					break;
				}
			}
		}

		this->method_12(coreS);
	}
}

void mbcd::method_11()
{
	cout << "class descending" << endl;
	this->decompose();

	v_info P, C, nC;
	vertex_index_set Q, E;
	vs_list cl;
	ST va = (ST)this->vs->size(), last = va - 1, start, cur;

	while (last >= 0 && this->coreN[last].second >= (ST)(this->mbc.size() + 1) / 2)
	{
		// update E
		E.insert(Q.begin(), Q.end());

		for (start = last - 1; start >= 0; --start)
		{
			if (this->coreN[start].second != this->coreN[(ULL)start + 1].second)
			{
				break;
			}
		}
		++start;

		// construct search set Q
		Q.clear();
		for (ST k = start; k <= last; ++k)
		{
			Q.emplace(this->coreN[k].first);
		}

		// conduct search
		C.clear();
		cl.clear();
		for (const auto v : Q)
		{
			cl.emplace_back(make_pair(v, this->g->get_order(v)));
			C.emplace(v, make_pair(0, 0));
		}
		for (const auto v : E)
		{
			C.emplace(v, make_pair(0, 0));
		}
		stable_sort(cl.begin(), cl.end(), vs_sort);

		cur = 0;
		while (cur < (ST)Q.size() && C.size() > this->mbc.size())
		{
			auto vi = cl[cur++].first;
			C.erase(vi);

			// generate P
			P.clear();
			P.emplace(vi, make_pair(0, 0));

			// generate C
			nC.clear();
			this->generateC(vi, C, nC);

			if (nC.size() + 1 > this->mbc.size())
			{
				this->solve_imp(P, nC);
			}
		}

		last = start - 1;
	}
}

void mbcd::method_12(const ST _s)
{
	cout << "analysis" << endl;
	v_info P, C, nC;
	vertex_index_set Q, E;
	vs_list cl;
	ST va = (ST)this->vs->size(), last = va - 1, start, cur;

	Q.clear();
	for (ST k = _s; k <= last; ++k)
	{
		Q.emplace(this->coreN[k].first);
	}

	// conduct search
	C.clear();
	cl.clear();
	for (const auto v : Q)
	{
		cl.emplace_back(make_pair(v, this->g->get_order(v)));
		C.emplace(v, make_pair(0, 0));
	}
	for (const auto v : E)
	{
		C.emplace(v, make_pair(0, 0));
	}
	stable_sort(cl.begin(), cl.end(), vs_sort);

	cur = 0;
	while (cur < (ST)Q.size() && C.size() > this->mbc.size())
	{
		auto vi = cl[cur++].first;
		C.erase(vi);

		// generate P
		P.clear();
		P.emplace(vi, make_pair(0, 0));

		// generate C
		nC.clear();
		this->generateC(vi, C, nC);

		if (nC.size() + 1 > this->mbc.size())
		{
			this->solve_imp(P, nC);
		}
	}
	last = _s - 1;

	while (last >= 0 && this->coreN[last].second >= (ST)(this->mbc.size() + 1) / 2)
	{
		// update E
		E.insert(Q.begin(), Q.end());

		for (start = last - 1; start >= 0; --start)
		{
			if (this->coreN[start].second != this->coreN[(ULL)start + 1].second)
			{
				break;
			}
		}
		++start;

		// construct search set Q
		Q.clear();
		for (ST k = start; k <= last; ++k)
		{
			Q.emplace(this->coreN[k].first);
		}

		// conduct search
		C.clear();
		cl.clear();
		for (const auto v : Q)
		{
			cl.emplace_back(make_pair(v, this->g->get_order(v)));
			C.emplace(v, make_pair(0, 0));
		}
		for (const auto v : E)
		{
			C.emplace(v, make_pair(0, 0));
		}
		stable_sort(cl.begin(), cl.end(), vs_sort);

		cur = 0;
		while (cur < (ST)Q.size() && C.size() > this->mbc.size())
		{
			auto vi = cl[cur++].first;
			C.erase(vi);

			// generate P
			P.clear();
			P.emplace(vi, make_pair(0, 0));

			// generate C
			nC.clear();
			this->generateC(vi, C, nC);

			if (nC.size() + 1 > this->mbc.size())
			{
				this->solve_imp(P, nC);
			}
		}

		last = start - 1;
	}
}

void mbcd::method_2()
{
	cout << "heuristic search" << endl;
	v_info P, C;
	ST va = (ST)this->vs->size();

	this->decompose();
	for (ST k = 0; k < va; ++k)
	{
		this->heuP.emplace(this->coreN[k].first, k);
	}

	for (ST k = 0; k < va; ++k)
	{
		C.emplace(k, make_pair(0, 0));
	}
	this->solve_heu(P, C);
}

void mbcd::solve_heu(v_info &_P, v_info &_C)
{
	auto updateR = [this](const v_info &_P)
	{
		if (_P.size() > this->mbc.size() && this->is_balanced(_P))
		{
			this->mbc.clear();
			for (const auto &v : _P)
			{
				this->mbc.emplace_back(v.first);
			}
		}
	};

	auto head = [this](const v_info &_P, const v_info &_C)
	{
		LL va = this->vs->size(), maxP = -ST_INVALID, tmP;
		VIT maxV = VIT_INVALID;

		for (const auto &u : _C)
		{
			auto ui = u.first;
			const auto &oin = (*this->vs)[ui];
			auto iteroe = oin.first.end(), iterie = oin.second.end();
			ST pr = 0;

			for (const auto &v : _P)
			{
				auto vi = v.first;
				ST t = v.second.first - v.second.second;
				auto itero = oin.first.find(vi), iteri = oin.second.find(vi);

				if (t > 0)
				{
					if (itero != iteroe && iteri == iterie)
					{
						pr += 1;
					}
					else if (itero == iteroe && iteri != iterie)
					{
						pr -= 1;
					}
				}
				else if (t < 0)
				{
					if (itero != iteroe && iteri == iterie)
					{
						pr -= 1;
					}
					else if (itero == iteroe && iteri != iterie)
					{
						pr += 1;
					}
				}
				else
				{
					if (itero != iteroe && iteri != iterie)
					{
						pr += 1;
					}
					else
					{
						pr -= 1;
					}
				}
			}

			tmP = pr * va + this->heuP.find(ui)->second;
			if (maxV == VIT_INVALID || maxP < tmP)
			{
				maxV = ui;
				maxP = tmP;
			}
		}

		return maxV;
	};

	auto tail = [this](const v_info &_P, const VIT _pv)
	{
		LL va = this->vs->size(), maxP = -ST_INVALID, tmP;
		VIT maxV = VIT_INVALID;

		// find the most imbalance vertex
		for (const auto &v : _P)
		{
			if (v.first != _pv)
			{
				tmP = va * (LL)abs(v.second.first - v.second.second) - this->g->get_order(v.first);
				if (maxV == VIT_INVALID || maxP < tmP)
				{
					maxV = v.first;
					maxP = tmP;
				}
			}
		}

		return maxV;
	};

	auto grow = [updateR, head, this](v_info &_P, v_info &_C)
	{
		if (this->prunePC(_P, _C))
		{
			return false;
		}
		updateR(_P);

		while (_C.size() && _P.size() + _C.size() > this->mbc.size())
		{
			auto vi = head(_P, _C);
			auto info = _C.find(vi)->second;
			_C.erase(vi);

			// update P
			this->updateP(vi, _P);
			_P.emplace(vi, info);

			// generate C
			this->tmpC.clear();
			this->generateC(vi, _C, this->tmpC);
			_C.swap(this->tmpC);

			if (this->prunePC(_P, _C))
			{
				break;
			}
			updateR(_P);
		}

		return true;
	};

	auto shrink = [tail, this](const VIT _pv, vertex_index_set &_X, v_info &_P, const v_info &_C, v_info &_nC)
	{
		auto tv = tail(_P, _pv);
		if (tv == VIT_INVALID)
		{
			return false;
		}
		_X.emplace(tv);
		_P.erase(tv);

		// update P
		const auto &oin = (*this->vs)[tv];
		for (auto &v : _P)
		{
			if (oin.first.find(v.first) != oin.first.end())
			{
				--v.second.second;
			}
			if (oin.second.find(v.first) != oin.second.end())
			{
				--v.second.first;
			}
		}

		// generate C
		_nC.clear();
		auto iter = _P.begin();

		for (const auto v : (*this->vs)[iter->first].first)
		{
			if (_C.find(v) != _C.end() && _P.find(v) == _P.end() && _X.find(v) == _X.end())
			{
				_nC.emplace(v, make_pair(0, 1));
			}
		}
		for (const auto v : (*this->vs)[iter->first].second)
		{
			if (_C.find(v) != _C.end() && _P.find(v) == _P.end() && _X.find(v) == _X.end())
			{
				auto f = _nC.emplace(v, make_pair(1, 0));
				if (!f.second)
				{
					++f.first->second.first;
				}
			}
		}

		while (++iter != _P.end())
		{
			this->tmpC.clear();
			this->generateC(iter->first, _nC, this->tmpC);
			_nC.swap(this->tmpC);
		}

		return true;
	};
	
	v_info C;
	vertex_index_set X;

	vs_list cl;
	for (const auto &v : _C)
	{
		cl.emplace_back(v.first, -this->heuP.find(v.first)->second);
	}
	stable_sort(cl.begin(), cl.end(), vs_sort);

	ST cur = 0, iC;
	while (cur < this->maxI && _C.size() > this->mbc.size())
	{
#ifdef mem_on
		this->ir->print_mem("running");
#endif

		auto pv = cl[cur++].first;
		_C.erase(pv);

		// update P
		_P.clear();
		_P.emplace(pv, make_pair(0, 0));

		// generate C
		C.clear();
		this->generateC(pv, _C, C);

		if (grow(_P, C))
		{
			X.clear();
			iC = 0;

			while (iC++ < this->maxI)
			{
#ifdef mem_on
				this->ir->print_mem("running");
#endif

				if (!shrink(pv, X, _P, _C, C))
				{
					break;
				}
				grow(_P, C);
			}
		}
	}
}

void mbcd::solve_imp(v_info &_P, v_info &_C)
{
#ifdef mem_on
	this->ir->print_mem("running");
#endif

	// pruning
	++this->iters;
	if (this->pruneT && this->prunePC(_P, _C))
	{
		return;
	}

	this->updatePC(_P, _C);

	// balance verification
	if (_P.size() > this->mbc.size() && this->is_balanced(_P))
	{
		this->mbc.clear();
		for (const auto &v : _P)
		{
			this->mbc.emplace_back(v.first);
		}
	}

	// sort candidates
	vector<pair<VIT, LL>> cl;
	if (this->methodT > 10)
	{
		this->analyzeC(_P, _C, cl);
	}
	else
	{
		for (const auto &v : _C)
		{
			cl.emplace_back(make_pair(v.first, v.first));
		}
	}
	stable_sort(cl.begin(), cl.end(), vl_sort);

	v_info nP, nC;
	ST cur = 0;

	while (_C.size() && _P.size() + _C.size() > this->mbc.size())
	{
		auto vi = cl[cur++].first;
		auto info = _C.find(vi)->second;
		_C.erase(vi);

		// update P
		nP = _P;
		this->updateP(vi, nP);
		nP.emplace(vi, info);

		// generate C
		nC.clear();
		this->generateC(vi, _C, nC);

		if (nP.size() + nC.size() > this->mbc.size())
		{
			this->solve_imp(nP, nC);
		}
	}
}

bool mbcd::is_balanced(const v_info &_P)
{
	for (const auto &v : _P)
	{
		if (v.second.first != v.second.second)
		{
			return false;
		}
	}

	return true;
}

void mbcd::updatePC(v_info &_P, v_info &_C)
{
	ST ps = (ST)_P.size();
	this->tmpV.clear();

	for (const auto &v : _C)
	{
		auto vi = v.first;
		auto info = v.second;
		const auto &oin = (*this->vs)[vi];

		if (info.first == ps && info.first == info.second)
		{
			bool f = true;
			for (const auto &u :_C)
			{
				auto ui = u.first;
				if (ui != vi && (oin.first.find(ui) == oin.first.end() || oin.second.find(ui) == oin.second.end()))
				{
					f = false;
					break;
				}
			}

			if (f)
			{
				this->tmpV.emplace(vi);
			}
		}
	}

	ST ts = (ST)this->tmpV.size();
	for (auto &v : _P)
	{
		v.second.first += ts;
		v.second.second += ts;
	}

	auto ti = ps + ts - 1;
	for (const auto v : this->tmpV)
	{
		_P.emplace(v, make_pair(ti, ti));
		_C.erase(v);
	}

	for (auto &v: _C)
	{
		v.second.first += ts;
		v.second.second += ts;
	}
}

void mbcd::generateC(const VIT _vi, const v_info &_C, v_info &_nC)
{
	const auto &oin = (*this->vs)[_vi];
	if (oin.first.size() + oin.second.size() > _C.size() * 2)
	{
		for (const auto &v : _C)
		{
			if (oin.first.find(v.first) != oin.first.end())
			{
				_nC.emplace(v.first, make_pair(v.second.first, v.second.second + 1));
			}
			if (oin.second.find(v.first) != oin.second.end())
			{
				auto f = _nC.emplace(v.first, make_pair(v.second.first + 1, v.second.second));
				if (!f.second)
				{
					++f.first->second.first;
				}
			}
		}
	}
	else
	{
		for (const auto v : oin.first)
		{
			auto iter = _C.find(v);
			if (iter != _C.end())
			{
				_nC.emplace(iter->first, make_pair(iter->second.first, iter->second.second + 1));
			}
		}
		for (const auto v : oin.second)
		{
			auto iter = _C.find(v);
			if (iter != _C.end())
			{
				auto f = _nC.emplace(iter->first, make_pair(iter->second.first + 1, iter->second.second));
				if (!f.second)
				{
					++f.first->second.first;
				}
			}
		}
	}
}

void mbcd::updateP(const VIT _vi, v_info &_P)
{
	const auto &oin = (*this->vs)[_vi];
	if (oin.first.size() + oin.second.size() > _P.size() * 2)
	{
		for (auto &v : _P)
		{
			if (oin.first.find(v.first) != oin.first.end())
			{
				++v.second.second;
			}
			if (oin.second.find(v.first) != oin.second.end())
			{
				++v.second.first;
			}
		}
	}
	else
	{
		for (const auto v : oin.first)
		{
			auto iter = _P.find(v);
			if (iter != _P.end())
			{
				++iter->second.second;
			}
		}
		for (const auto v : oin.second)
		{
			auto iter = _P.find(v);
			if (iter != _P.end())
			{
				++iter->second.first;
			}
		}
	}
}

bool mbcd::prunePC(v_info &_P, v_info &_C)
{
	ST rV = (ST)(this->mbc.size() + 1 - _P.size()), tmp, tOut, tIn;
	for (const auto &v : _P)
	{
		tmp = abs(v.second.first - v.second.second);
		if (tmp > rV)
		{
			rV = tmp;
		}
	}

	// analyze P
	this->tmpV.clear();
	for (const auto &v : _P)
	{
		tmp = v.second.first - v.second.second;
		ST k = (rV - abs(tmp) + 1) / 2;

		// obtain tOut and tIn
		if (tmp > 0)
		{
			tOut = k;
			tIn = k + tmp;
		}
		else if (tmp < 0)
		{
			tOut = k - tmp;
			tIn = k;
		}
		else
		{
			tOut = tIn = k;
		}

		// test
		if (!this->requireN(v.first, _C, tOut, tIn))
		{
			return true;
		}

		if ((ST)this->tmpVO.size() == tOut)
		{
			this->tmpV.insert(this->tmpVO.begin(), this->tmpVO.end());
		}
		if ((ST)this->tmpVI.size() == tIn)
		{
			this->tmpV.insert(this->tmpVI.begin(), this->tmpVI.end());
		}
	}

	// try to move critical vertices from C to P
	if (this->tmpV.size())
	{
		if (!this->is_valid(_P, _C))
		{
			return true;
		}

		// update rV
		rV = (ST)(this->mbc.size() + 1 - _P.size());
		for (const auto &v : _P)
		{
			tmp = abs(v.second.first - v.second.second);
			if (tmp > rV)
			{
				rV = tmp;
			}
		}
	}

	// analyze C
	this->tmpV.clear();
	for (const auto &v : _C)
	{
		tmp = v.second.first - v.second.second;
		ST k = (max(rV - 1, abs(tmp)) - abs(tmp) + 1) / 2;

		// obtain tOut and tIn
		if (tmp > 0)
		{
			tOut = k;
			tIn = k + tmp;
		}
		else if (tmp < 0)
		{
			tOut = k - tmp;
			tIn = k;
		}
		else
		{
			tOut = tIn = k;
		}

		if (!this->requireN(v.first, _C, tOut, tIn, false))
		{
			this->tmpV.emplace(v.first);
		}
	}

	for (const auto v : this->tmpV)
	{
		_C.erase(v);
	}

	return false;
}

bool mbcd::requireN(const VIT _vi, const v_info &_C, const ST _tOut, const ST _tIn, bool _f)
{
	const auto &oin = (*this->vs)[_vi];
	ST tmpO = 0, tmpI = 0;

	if (_f)
	{
		this->tmpVO.clear();
		this->tmpVI.clear();
	}

	// counting
	if (oin.first.size() + oin.second.size() > _C.size() * 2)
	{
		VIT tv;
		if (_tOut)
		{
			for (const auto &v : _C)
			{
				tv = v.first;
				if (_vi != tv && oin.first.find(tv) != oin.first.end())
				{
					++tmpO;
					if (_f)
					{
						this->tmpVO.emplace(tv);
					}

					if (tmpO > _tOut)
					{
						break;
					}
				}
			}
		}
		
		if (_tIn)
		{
			for (const auto &v : _C)
			{
				tv = v.first;
				if (_vi != tv && oin.second.find(tv) != oin.second.end())
				{
					++tmpI;
					if (_f)
					{
						this->tmpVI.emplace(tv);
					}

					if (tmpI > _tIn)
					{
						break;
					}
				}
			}
		}
	}
	else
	{
		if (_tOut)
		{
			for (const auto v : oin.first)
			{
				if (_vi != v && _C.find(v) != _C.end())
				{
					++tmpO;
					if (_f)
					{
						this->tmpVO.emplace(v);
					}

					if (tmpO > _tOut)
					{
						break;
					}
				}
			}
		}
		
		if (_tIn)
		{
			for (const auto v : oin.second)
			{
				if (_vi != v && _C.find(v) != _C.end())
				{
					++tmpI;
					if (_f)
					{
						this->tmpVI.emplace(v);
					}

					if (tmpI > _tIn)
					{
						break;
					}
				}
			}
		}
	}

	// analyze
	if (tmpO < _tOut || tmpI < _tIn)
	{
		return false;
	}

	return true;
}

bool mbcd::is_valid(v_info &_P, v_info &_C)
{
	if (!this->tmpV.size())
	{
		return true;
	}

	auto vi = *this->tmpV.begin();
	this->tmpV.erase(vi);

	this->tmpP = _P;
	this->updateP(vi, this->tmpP);
	this->tmpP.emplace(vi, _C.find(vi)->second);
	
	this->tmpC.clear();
	this->generateC(vi, _C, this->tmpC);
	this->tmpC.erase(vi);

	if (this->tmpV.size() > this->tmpC.size())
	{
		return false;
	}
	for (const auto v : this->tmpV)
	{
		if (this->tmpC.find(v) == this->tmpC.end())
		{
			return false;
		}
	}

	while (this->tmpV.size())
	{
		auto vi = *this->tmpV.begin();
		this->tmpV.erase(vi);
		auto info = this->tmpC.find(vi)->second;
		this->tmpC.erase(vi);

		// update P
		this->updateP(vi, this->tmpP);
		this->tmpP.emplace(vi, info);

		// generate C
		this->tmpI.clear();
		this->generateC(vi, this->tmpC, this->tmpI);
		this->tmpC.swap(this->tmpI);

		if (this->tmpV.size() > this->tmpC.size())
		{
			return false;
		}
		for (const auto v : this->tmpV)
		{
			if (this->tmpC.find(v) == this->tmpC.end())
			{
				return false;
			}
		}
	}

	_P.swap(this->tmpP);
	_C.swap(this->tmpC);

	return true;
}

void mbcd::decompose()
{
	if (this->coreN.size())
	{
		return;
	}

	const auto &vs = (*this->vs);
	ST n = (ST)vs.size();
	vertex_index_list vert(n), pos(n), bin;
	vector<pair<ST, ST>> tmpD(n, make_pair(0, 0));

	// init D
	ST md = 0;
	for (ST k = 0; k < n; ++k)
	{
		tmpD[k].first = (ST)vs[k].first.size();
		tmpD[k].second = (ST)vs[k].second.size();
		if (min(tmpD[k].first, tmpD[k].second) > md)
		{
			md = min(tmpD[k].first, tmpD[k].second);
		}
	}

	bin.resize((ULL)md + 1, 0);

	// insert to bins
	for (ST k = 0; k < n; ++k)
	{
		++bin[min(tmpD[k].first, tmpD[k].second)];
	}

	// set start pos of each bin
	ST start = 0, num;
	for (ST k = 0; k <= md; ++k)
	{
		num = bin[k];
		bin[k] = start;
		start += num;
	}

	// associate bin, pos and vert
	for (ST k = 0; k < n; ++k)
	{
		pos[k] = bin[min(tmpD[k].first, tmpD[k].second)]++;
		vert[pos[k]] = k;
	}

	// recover bin
	for (ST k = md; k >= 1; --k)
	{
		bin[k] = bin[(ULL)k - 1];
	}
	bin[0] = 0;

	// peeling
	VIT v, w;
	ST dv, du, pu, pw;
	v_info tmpM;

	// is changed
	auto update = [&tmpD](const ST _du, const VIT _u, const ST _dv, const pair<ST, ST> &_info)
	{
		if (_du > _dv)
		{
			tmpD[_u].first -= _info.first;
			tmpD[_u].second -= _info.second;
			auto ndu = min(tmpD[_u].first, tmpD[_u].second);

			if (_du != ndu)
			{
				return true;
			}
		}

		return false;
	};

	for (ST k = 0; k < n; ++k)
	{
		v = vert[k];
		dv = min(tmpD[v].first, tmpD[v].second);
		tmpM.clear();
		
		// collect neighbors
		for (const auto u : vs[v].first)
		{
			tmpM.emplace(u, make_pair(0, 1));
		}
		for (const auto u : vs[v].second)
		{
			auto f = tmpM.emplace(u, make_pair(1, 0));
			if (!f.second)
			{
				++f.first->second.first;
			}
		}

		for (const auto &u : tmpM)
		{
			auto ui = u.first;
			du = min(tmpD[ui].first, tmpD[ui].second);

			if (update(du, ui, dv, u.second))
			{
				pu = pos[ui];
				pw = bin[du];
				w = vert[pw];

				if (ui != w)
				{
					pos[ui] = pw;
					vert[pu] = w;
					pos[w] = pu;
					vert[pw] = ui;
				}

				++bin[du];
			}
		}
	}

	// sort core number
	this->coreN.clear();
	for (ST k = 0; k < n; ++k)
	{
		this->coreN.emplace_back(make_pair(k, min(tmpD[k].first, tmpD[k].second)));
	}
	stable_sort(this->coreN.begin(), this->coreN.end(), vs_sort);
}

void mbcd::analyzeC(const v_info &_P, const v_info &_C, vector<pair<VIT, LL>> &_cl)
{
	ST tmp, pr;
	LL va = this->vs->size();

	for (const auto &u : _C)
	{
		const auto &oin = (*this->vs)[u.first];
		auto iteroe = oin.first.end(), iterie = oin.second.end();
		pr = 0;

		for (const auto &v : _P)
		{
			auto vi = v.first;
			tmp = v.second.first - v.second.second;
			auto itero = oin.first.find(vi), iteri = oin.second.find(vi);

			if (tmp > 0)
			{
				if (itero != iteroe && iteri == iterie)
				{
					pr += 1;
				}
				else if (itero == iteroe && iteri != iterie)
				{
					pr -= 1;
				}
			}
			else if (tmp < 0)
			{
				if (itero != iteroe && iteri == iterie)
				{
					pr -= 1;
				}
				else if (itero == iteroe && iteri != iterie)
				{
					pr += 1;
				}
			}
			else
			{
				if (itero != iteroe && iteri != iterie)
				{
					pr += 1;
				}
				else
				{
					pr -= 1;
				}
			}
		}

		_cl.emplace_back(make_pair(u.first, -pr * va + this->g->get_order(u.first)));
	}
}
