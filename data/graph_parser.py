#!/usr/bin/python3
# -*- coding: utf-8 -*-


import sys
import os


def get_file_list(_dir_in, _dir_out):
	ret = []
	if not os.path.exists(_dir_out):
		os.mkdir(_dir_out)
	for root, _, files in os.walk(_dir_in, topdown=False):
		for name in files:
			sp = os.path.join(root, name)
			dp = os.path.join(_dir_out, name.replace(".", "_") + ".txt")
			if not os.path.exists(dp):
				ret.append((sp, dp))
	return ret


class GraphParser:
	def __init__(self, _src, _des):
		self.ready = False
		self.cache = {}
		self.data_src = _src
		self.data_des = _des
		self.vm = {}
		self.cm = 0
		self.info = []

	def parse(self):
		self.ready = False

	def store(self):
		if self.ready:
			with open(self.data_des, "w") as f:
				print("storing: " + self.data_des + " start")
				print("vertices %d, bi-edges %d, is multi-edge %d" % (self.cm, self.info[0], self.info[1]))
				p = Progress(0, self.cm)
				log_span = max(int(self.cm * 0.01), 1)

				# reorder
				tg = []
				for i in range(self.cm):
					tg.append([set(), set()])

				for u, n in self.cache.items():
					nu = self.vm.get(u)
					for vo, _ in n[0].items():
						nvo = self.vm.get(vo)
						tg[nu][0].add(nvo)
					for vi, _ in n[1].items():
						nvi = self.vm.get(vi)
						tg[nu][1].add(nvi)

				# store
				f.write(str(self.cm) + "\n")
				for i in range(self.cm):
					if i % log_span == 0:
						p.show(i)
					if i == self.cm - 1:
						print("")

					t = 0
					e = len(tg[i][0])
					for v in tg[i][0]:
						t = t + 1
						if t != e:
							f.write(str(v) + ",")
						else:
							f.write(str(v))
					f.write("|")

					t = 0
					e = len(tg[i][1])
					for v in tg[i][1]:
						t = t + 1
						if t != e:
							f.write(str(v) + ",")
						else:
							f.write(str(v))
					f.write("\n")
				print("storing: " + self.data_des + " end")


class Progress:
	def __init__(self, _min, _max):
		self.min = _min
		self.max = _max
		self.span = _max - _min

	def show(self, _cur):
		sys.stdout.write("\rprocessing: {0}%".format(int(round(float(_cur) * 100 / self.span))))
		sys.stdout.flush()
