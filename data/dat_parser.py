#!/usr/bin/python3
# -*- coding: utf-8 -*-


import re
from graph_parser import GraphParser
from graph_parser import Progress


TYPES = [".dat"]
pattern = r"[ ,\n\t]+"


class DatParser(GraphParser):
	def __init__(self, _src, _des, _type):
		super().__init__(_src, _des)
		self.type = _type

	def parse(self):
		super().parse()
		try:
			TYPES.index(self.type)
		except ValueError:
			print("error: type " + self.type + " is not supported")
		else:
			with open(self.data_src, "r") as f:
				print("parsing: " + self.data_src + " start")
				lines = f.readlines()
				e = len(lines)
				p = Progress(0, e)
				log_span = max(int(e * 0.01), 1)
				be = 0
				me = 0

				for i in range(e):
					if i % log_span == 0:
						p.show(i)
					if i == e - 1:
						print("")

					items = re.split(pattern, lines[i].strip())
					if items[0].strip() == items[1].strip():
						continue

					uid = items[0].strip()
					if self.vm.get(uid, None) is None:
						self.vm[uid] = self.cm
						self.cm = self.cm + 1
					vid = items[1].strip()
					if self.vm.get(vid, None) is None:
						self.vm[vid] = self.cm
						self.cm = self.cm + 1

					if self.cache.get(uid, None) is None:
						# [out, in]
						self.cache[uid] = [{}, {}]
					if self.cache.get(vid, None) is None:
						self.cache[vid] = [{}, {}]

					self.cache[uid][0][vid] = self.cache[uid][0].get(vid, 0) + 1
					self.cache[vid][1][uid] = self.cache[vid][1].get(uid, 0) + 1

					if self.cache[uid][0][vid] > 1 or self.cache[vid][1][uid] > 1:
						me = 1

					# info
					if self.cache[uid][1].get(vid, None) is not None:
						be = be + 1

				self.info.append(be)
				self.info.append(me)
				self.ready = True
				print("parsing: " + self.data_src + " success")
