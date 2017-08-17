# -*- coding: UTF-8 -*-

import json
import jieba
import sys

jieba.cut("")

data_all = []

tot = 0

dic = {}

print len(dic.keys())

words_all = {}

def main():
	f = open("../filter_pages/result.txt", "r")
	global data_all
	data_all = json.loads(f.read())
	f.close()
	
	print len(data_all)
	
	global tot
	
	for i in range(len(data_all)):
		item = data_all[i]
		tmp = filter(lambda x: not x.isspace(), jieba.lcut_for_search(item["content"]))
		for x in item["content"]:
			if x.isspace(): continue
			tmp.append(x)
		dic_tmp = {}
		for x in tmp:
			dic_tmp[x] = 1
		for x in dic_tmp.keys():
			if not words_all.has_key(x):
				words_all[x] = []
			words_all[x].append(i)
		tot += len(tmp)
		if i % 1000 == 0:
			print "done %d" % i
			sys.stdout.flush()


main()

print "tot = %d = %.2lf M" % (tot, tot / 1048576.0)

print "%d" % len(dic.keys())

f_res = open("result.txt", "w")
f_res.write(json.dumps(words_all, indent = 1).encode("utf8"))
f_res.close()
