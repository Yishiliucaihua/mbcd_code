#pragma once

//#define win
//#define debug
//#define mem_on

#ifdef win
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef win
#include <Windows.h>
#include <Psapi.h>
#else
#include <limits>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#endif
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <chrono>
#include <regex>
#include <iostream>

#ifndef win
using std::numeric_limits;
#endif
using std::unordered_set;
using std::unordered_map;
using std::vector;
using std::string;
using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::regex;
using std::sregex_token_iterator;
using std::cout;
using std::endl;
using std::pair;

namespace core
{
	typedef unsigned int VIT;
	typedef int ST;
	typedef long long LL;
	typedef unsigned long long ULL;

#ifdef win
	constexpr VIT VIT_INVALID= UINT_MAX;
	constexpr ST ST_INVALID = INT_MAX;
#else
	constexpr VIT VIT_INVALID = numeric_limits<VIT>::max();
	constexpr ST ST_INVALID = numeric_limits<ST>::max();
#endif

	typedef vector<VIT> vertex_index_list;
	typedef unordered_set<VIT> vertex_index_set;
	typedef pair<VIT, ST> vs_pair;
	typedef vector<vs_pair> vs_list;
	typedef unordered_map<VIT, pair<ST, ST>> v_info;
	typedef pair<vertex_index_set, vertex_index_set> oi_neighbors;
	typedef vector<oi_neighbors> vertex_set;

	bool vs_sort(const vs_pair&, const vs_pair&);
	void split(const string&, const string&, vector<string>&);
	string to_string(const vertex_index_list&);

	class timer
	{
	public:
		void clear();
		ULL elapse(bool=true);
	private:
		system_clock::time_point lastTime, currentTime;
	};

	class info_reporter
	{
	public:
		void clear_t();
		ULL get_time();
		void print_mem(string, bool=false);
	private:
		const ULL span = 500;
#ifdef win
		const double M = 1024 * 1024;
		void get_memory_usage(string) const;
#else
		const char *data_mem = "VmRSS:";
		void print_mem(int, string) const;
#endif
		timer tt, mt;
	};
}
