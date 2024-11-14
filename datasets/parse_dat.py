#!/usr/bin/python3
# -*- coding: utf-8 -*-


from dat_parser import DatParser
from graph_parser import get_file_list
import os


fp_l = get_file_list("." + os.path.sep + "dats", "." + os.path.sep + "real")
for fp in fp_l:
	gp = DatParser(fp[0], fp[1], os.path.splitext(fp[0])[-1])
	gp.parse()
	gp.store()
