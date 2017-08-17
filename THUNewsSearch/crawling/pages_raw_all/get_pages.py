# -*- coding: UTF-8 -*-

import json
import sys
import os

data_all = []
n = 0

url_prefix = "http://news.tsinghua.edu.cn/"

err_txt_name = ""

def get_url(i, url):
	global url_prefix
	url = url_prefix + url
	ret = os.system("wget %s -O %d.html -o /dev/null" % (url, i))
	if ret != 0:
		f = open(err_txt_name, "a")
		f.write("%d\n" % i)
		f.close()

def craw(l, r):
	if r > n:
		r = n
	if l >= r:
		return
	global data_all
	for i in range(l, r):
		url = data_all[i]["htmlurl"]
		get_url(i, url)
		print "%d of %d [%d, %d, %d], %.2lf%%" % (i - l + 1, r - l, l, r, i, 100.0 * (i - l + 1) / (r - l)) 

def main():
	cnt_argv = len(sys.argv)
	if cnt_argv != 1 + 2:
		print "usage: %s <start> <count>" % sys.argv[0]
		return
	
	f = open("../list_parsed/result.txt", "r")
	global data_all
	data_all = json.loads(f.read())
	f.close()
	
	global n
	n = len(data_all)
	
	start = int(sys.argv[1])
	end = start + int(sys.argv[2])
	global err_txt_name
	err_txt_name = "err_%d_%d.txt" % (start, int(sys.argv[2]))
	# if (start < 0) or (start > end):
	# 	return
	
	craw(start, end)


main()

