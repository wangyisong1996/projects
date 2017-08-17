# -*- coding: UTF-8 -*-

import json
import sys
from HTMLParser import HTMLParser

h = HTMLParser()

data_all = []
n = 0

data_new = []
m = 0

total_len = 0

def work(i):
	f = open("../pages_raw_all/%d.html" % i)
	s = f.read().decode("utf8")  # 不要忘记decode
	f.close()
	
	if s.find("<article class=\"article\">") == -1:
		return 0
	
	global m
	global data_new
	global data_all
	data_new.append(data_all[i].copy())
	m += 1
	data_new[-1]["id"] = m - 1
	
	L = s.find("<article class=\"article\">")
	R = s.find("</article>")
	s = s[L:R]
	
	while 1:
		pos1 = s.find("<")
		if pos1 == -1:
			break
		pos2 = s.find(">", pos1)
		if pos2 == -1:
			break
		s = s[0:pos1] + s[pos2+1:]
	
	global h
	s = h.unescape(s)
	
	data_new[-1]["content"] = s
	
	global total_len
	total_len += len(s)
	
	
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
		if work(i):
			ok_cnt += 1
		else:
			print "[%d]  %s" % (i, "http://news.tsinghua.edu.cn/" + data_all[i]["htmlurl"])
		if i % 1000 == 0:
			print "done %d" % i
		sys.stdout.flush()
	
	print ok_cnt


main()

print len(data_new)
print "%d = %.1lf M" % (total_len, total_len / 1048576.0)

f_out = open("result.txt", "w")
f_out.write(json.dumps(data_new, indent = 4, ensure_ascii = False).encode("utf8"))
f_out.close()
