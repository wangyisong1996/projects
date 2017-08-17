# -*- coding: UTF-8 -*-

import json

data_all = []
n = 0

def check(i):
	f = open("../pages_raw_all/%d.html" % i)
	s = f.read()
	f.close()
	if s.find("<article class=\"article\">") == -1:
		return 0
	else:
		return 1

def main():
	f = open("../list_parsed/result.txt", "r")
	global data_all
	data_all = json.loads(f.read())
	f.close()
	
	global n
	n = len(data_all)
	
	ok_cnt = 0
	for i in range(n):
		if check(i):
			ok_cnt += 1
		else:
			print "[%d]  %s" % (i, "http://news.tsinghua.edu.cn/" + data_all[i]["htmlurl"])
	
	print ok_cnt


main()

