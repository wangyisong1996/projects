from django.shortcuts import render, redirect
from django.urls import reverse
from . import search as search_helper
import jieba
jieba.cut("")

# Create your views here.

from django.http import HttpResponse


import urllib

def build_url(*args, **kwargs):
	get = kwargs.pop('get', {})
	url = reverse(*args, **kwargs)
	if get:
		url += '?' + urllib.urlencode(get)
	return url


def index(request):
	return HttpResponse("Hello World")

def search(request):
	try:
		words = filter(lambda x : not x.isspace(), jieba.lcut(request.GET["q"]))
	except:
		words = []
	
	if len(words) == 0:
		return redirect("/")
	
	words = list(set(words))
	
	try:
		get_pg = int(request.GET["pg"])
	except:
		get_pg = 1
	if get_pg < 1:
		get_pg = 1
	
	res = search_helper.query(words, get_pg)
	res["query"] = request.GET["q"]
	
	n = res["cnt_pg"] 
	pg = res["pg"]
	get_q = request.GET["q"].encode("utf8")
	
	res["last_pg_url"] = build_url("search", get={"q":get_q, "pg":"%d" % (1 if pg == 1 else pg - 1)})
	res["next_pg_url"] = build_url("search", get={"q":get_q, "pg":"%d" % (n if pg == n else pg + 1)})
	
	res["pg_urls"] = []
	for i in range(1 if pg < 9 else pg - 9, n + 1 if pg + 10 > n + 1 else pg + 10):
		res["pg_urls"].append((build_url("search", get={"q":get_q, "pg":"%d" % i}), i))
	
	# return HttpResponse(("/".join(words) + "\n" + reduce(lambda x, y : "%s, %s" % (x, y), s)).encode("utf8"))
	return render(request, "search.html", res)
