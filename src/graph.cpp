#include "./graph.h"

using namespace core;

void graph::load(string _filePath)
{
	this->va = 0;
	this->ea = 0;
	this->eas = 0;
	this->dm = 0;
	this->dmo = 0;
	this->dmi = 0;
	this->vs.clear();
	this->vsm.clear();

	ifstream ifs(_filePath);
	if (ifs.is_open())
	{
		string line;
		vector<string> temp1, temp2;
		vertex_index_set vis;

		// load graph to vs
		getline(ifs, line);
		this->va = atoi(line.c_str());
		this->vs.resize(this->va);
		ST vn = 0;

		while (getline(ifs, line))
		{
			split(line, "|", temp1);
			auto &oin = this->vs[vn++];

			// out neighbors
			split(temp1[0], ",", temp2);
			for (const auto &on : temp2)
			{
				oin.first.emplace(atoi(on.c_str()));
			}
			this->ea += (ST)temp2.size();

			// in neighbors
			if (temp1.size() > 1)
			{
				split(temp1[1], ",", temp2);
				for (const auto &in : temp2)
				{
					oin.second.emplace(atoi(in.c_str()));
				}
			}

			vis.clear();
			vis.insert(oin.first.begin(), oin.first.end());
			vis.insert(oin.second.begin(), oin.second.end());
			this->eas += (ST)vis.size();

			if ((ST)vis.size() > this->dm)
			{
				this->dm = (ST)vis.size();
			}
			if ((ST)oin.first.size() > this->dmo)
			{
				this->dmo = (ST)oin.first.size();
			}
			if ((ST)oin.second.size() > this->dmi)
			{
				this->dmi = (ST)oin.second.size();
			}
		}

		// calculate vertex order
		ifs.close();
		this->compute_order();
	}
}

void graph::print_info() const
{
	cout << "#vertices:" << this->va << endl;
	cout << "#edges:" << this->ea << endl;
	cout << "average out-degree:" << (double)this->ea / this->va << endl;
	cout << "average degree:" << (double)this->eas / this->va << endl;
	cout << "max degree:" << this->dm << endl;
	cout << "max out-degree:" << this->dmo << endl;
	cout << "max in-degree:" << this->dmi << endl;
}

void graph::compute_order()
{
	ST va = this->va;
	vs_list vsl(va);

	// sort
	for (ST i = 0; i < va; ++i)
	{
		vsl[i] = { i, (ST)(this->vs[i].first.size() + this->vs[i].second.size()) };
	}
	stable_sort(vsl.begin(), vsl.end(), vs_sort);

	// obtain vertex-position map
	for (ST i = 0; i < va; ++i)
	{
		this->vsm.emplace(vsl[i].first, i);
	}
}
