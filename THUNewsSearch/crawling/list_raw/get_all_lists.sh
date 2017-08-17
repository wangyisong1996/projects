#!/bin/bash
for ((i=2001;i<=2016;i++)) do
	for ((j=1;j<=12;j++)) do
		echo "i = $i, j = $j"
		s="$i""_$j"
		wget "http://news.tsinghua.edu.cn/publish/thunews/newsCollections/d_$s.json" # -H 'Cookie: _ga=GA1.3.887496364.1456546367; JSESSIONID=abc2AUFIMEQaPdX1dp9Bv' -H 'Accept-Encoding: gzip, deflate, sdch' -H 'Accept-Language: en-US,en;q=0.8,zh-CN;q=0.6,zh;q=0.4' -H 'User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.89 Safari/537.36' -H 'Accept: */*' -H 'Referer: http://news.tsinghua.edu.cn/publish/thunews/9657/index.html' -H 'X-Requested-With: XMLHttpRequest' -H 'Connection: keep-alive' --compressed
	done;
done;
