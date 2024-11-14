#include "./mbcd.h"

using core::ST;
using core::ULL;
using core::graph;
using core::info_reporter;
using core::search_interface;
using core::mbcd;
using core::to_string;

constexpr int parameter_num = 4;

int main(int _argc, char *_argv[])
{
	if (_argc < parameter_num)
	{
		cout << "error: miss necessary parameters" << endl;
		exit(-1);
	}
	
	info_reporter ir;
	ULL run_time = 0;
	ST prt = atoi(_argv[2]), met = atoi(_argv[3]);

	// load
	graph g;
	g.load(_argv[1]);
	cout << "original graph:=>{" << endl;
	g.print_info();
	cout << "}" << endl;

	search_interface *si = new mbcd(&ir, prt, met);
	if (met == 2 && _argc > parameter_num)
	{
		dynamic_cast<mbcd*>(si)->set_I(atoi(_argv[4]));
	}

	// running
	if (g.data()->size())
	{
		ir.clear_t();
		si->solve(g);
		run_time = ir.get_time();
	}

	auto mbc = si->result();
	cout << "result size:=>{" << mbc.size() << "}" << endl;
	stable_sort(mbc.begin(), mbc.end());
	cout << to_string(mbc) << endl;
	cout << "time info:=>{" << run_time << "}" << endl;

#ifdef debug
	si->print_mbc();
#endif

	return 0;
}
