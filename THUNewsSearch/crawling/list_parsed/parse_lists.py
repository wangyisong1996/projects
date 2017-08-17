# -*- coding: UTF-8 -*-

import json

data_all = []

def parse_file(yy, mm):
	fname = "../list_raw/d_%d_%d.json" % (yy, mm)
	f = open(fname, "r")
	s = f.read()
	f.close()
	A = json.loads(s)
	B = A["data"]
	for i in range(1, 31 + 1):
		s_i = "%d" % i
		if B.has_key(s_i):
			l = B[s_i]
			for item in l:
				del item["day"]
				item["accessednum"] = int(item["accessednum"])
				item["yy"] = yy
				item["mm"] = mm
				item["dd"] = i
				if item["htmlurl"].find("index") != -1:
					continue
				item["htmlurl"] = item["htmlurl"][0:len(item["htmlurl"])-5] + "_.html"
				global data_all
				data_all.append(item)

def main():
	for i in range(2001, 2016 + 1):
		for j in range(1, 13):
			if (i == 2016) and (j > 9):
				continue
			parse_file(i, j)
			# return
	global data_all
	tot = len(data_all)
	for i in range(tot):
		data_all[i]["id"] = i


main()

print len(data_all)

f_res = open("result.txt", "w")
f_res.write(json.dumps(data_all, indent = 4).decode("unicode-escape").encode("utf8"))
f_res.close()
