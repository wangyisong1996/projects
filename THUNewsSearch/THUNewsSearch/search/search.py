# -*- coding: UTF-8 -*-

import json

from django.utils.html import escape

f_indexes = open("search/data/indexes.txt", "r")
indexes = json.loads(f_indexes.read())
f_indexes.close()

f_pages = open("search/data/all_pages.txt", "r")
pages = json.loads(f_pages.read())
f_pages.close()

for x in indexes.keys():
	indexes[x] = set(indexes[x])

print "load indexes ok"

page_size = 10

def query_indexes(words):
	return reduce(lambda x, y : x.intersection(y), map(lambda x : indexes[x] if indexes.has_key(x) else set(), words))

def get_em_text(text, words):
	return reduce(lambda x, y : x.replace(y, "<em>" + y + "</em>"), map(escape, [text] + words))

def get_em_content(content, words):
	max_len = 160
	
	now_len = 0
	now_pos = 0
	now_R = -10000
	content_len = len(content)
	res = []
	res_last = ""
	while 1:
		new_pos = -1
		for w in words:
			tmp = content.find(w, now_pos + 1)
			if tmp == -1:
				continue
			if (new_pos != -1) and (tmp >= new_pos):
				continue
			new_pos = tmp
			L = max(1, tmp - 15)
			R = min(content_len, tmp + len(w) + 15)
		if new_pos == -1:
			break
		R = max(now_R, R)
		if L <= now_R + 10:
			if now_len + R - now_R > max_len:
				break
			res_last += content[now_R:R]
			now_len += R - now_R
			now_R = R
			now_pos = new_pos
		else:
			if now_len + R - L + 2 > max_len:
				break
			if res_last != "":
				res.append(res_last)
			res_last = content[L:R]
			now_len += R - L + 2
			now_R = R
			now_pos = new_pos
	if res_last != "":
		res.append(res_last)
	res.append("")
	
	return "…".decode("utf8").join(map(lambda x : get_em_text(x, words), res))
		

def get_page_info(pid, words):
	page = pages[pid]
	return (get_em_text(page["title"], words), "http://news.tsinghua.edu.cn/" + page["htmlurl"], "%d-%02d-%02d" % (page["yy"], page["mm"], page["dd"]), get_em_content(page["content"], words))

def query(words, pg = 1):
	ids = query_indexes(words)
	ids = list(sorted(ids))
	ids.reverse()
	
	n = len(ids)
	cnt_pg = (n - 1) / page_size + 1
	if pg < 1:
		pg = 1
	if pg > cnt_pg:
		pg = cnt_pg
	
	# return (n, pg, cnt_pg, map(lambda x : "<a href = http://news.tsinghua.edu.cn/" + pages[x]["htmlurl"] + ">" + ("%d" % x) + "</a>", ids[(pg - 1) * page_size : pg * page_size]))
	return dict(n=n, pg=pg, cnt_pg=cnt_pg, data=map(lambda x : get_page_info(x, words), ids[(pg - 1) * page_size : pg * page_size]))
	

