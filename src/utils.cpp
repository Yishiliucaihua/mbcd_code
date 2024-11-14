#include "./utils.h"

using namespace core;

bool core::vs_sort(const vs_pair &_vs1, const vs_pair &_vs2)
{
	return _vs1.second < _vs2.second;
}

void core::split(const string &_str, const string &_del, vector<string> &_container)
{
	_container.clear();
	if (_str.size())
	{
		regex reg("\\" + _del);
		sregex_token_iterator iter(_str.begin(), _str.end(), reg, -1);
		sregex_token_iterator end;

		while (iter != end)
		{
			_container.emplace_back(iter->str());
			++iter;
		}
	}
}

static ST bs = 1025;
static char *buffer = new char[1025];
string core::to_string(const vertex_index_list &_vil)
{
	ST n = (ST)_vil.size();
	ST max = n * 11;

	if (!max)
	{
		return string("none");
	}

	if (bs < max)
	{
		delete[] buffer;
		bs = max;
		buffer = new char[max];
	}

	char *pb = buffer;
	memset(pb, 0, max);
	ST len = snprintf(pb, max, "%u", _vil[0]);
	pb += len;
	max -= len;

	for (ST i = 1; i < n; ++i)
	{
		len = snprintf(pb, max, "_%u", _vil[i]);
		pb += len;
		max -= len;
	}

	return string(buffer);
}

void timer::clear()
{
	this->currentTime = this->lastTime = system_clock::now();
}

ULL timer::elapse(bool _clear)
{
	this->currentTime = system_clock::now();
	ULL ret = duration_cast<milliseconds>(this->currentTime - this->lastTime).count();
	if (_clear)
	{
		this->lastTime = this->currentTime;
	}
	return ret;
}

void info_reporter::clear_t()
{
	this->tt.clear();
	this->mt.clear();
}

ULL info_reporter::get_time()
{
	return this->tt.elapse();
}

void info_reporter::print_mem(string _prefix, bool _force)
{
	if (_force || this->mt.elapse(false) > span)
	{
#ifdef win
		this->get_memory_usage(_prefix);
#else
		this->print_mem(getpid(), _prefix);
#endif
		this->mt.clear();
	}
}

#ifdef win
void info_reporter::get_memory_usage(string _prefix) const
{
	PROCESS_MEMORY_COUNTERS pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
	{
		uint64_t ms = pmc.PagefileUsage;
		cout << _prefix << " mem:=>{" << ms / M << "}" << endl;
	}
}
#else
void info_reporter::print_mem(int _pid, string _prefix) const
{
	FILE *stream;
	char cache[512];
	char mem_info[64];

	sprintf(mem_info, "/proc/%d/status", _pid);
	stream = fopen(mem_info, "r");
	if (stream == NULL)
	{
		return;
	}

	while (fscanf(stream, "%s", cache) != EOF)
	{
		if (strncmp(cache, data_mem, strlen(data_mem)) == 0)
		{
			if (fscanf(stream, "%s", cache) != EOF)
			{
				cout << _prefix << " mem:=>{" << string(cache) << "}" << endl;
				break;
			}
		}
	}
}
#endif
