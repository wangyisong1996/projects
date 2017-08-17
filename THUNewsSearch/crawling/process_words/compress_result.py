# -*- coding: UTF-8 -*-

import json

f = open("result.txt", "r")
data_all = json.loads(f.read())
f.close()

f_res = open("result_comp.txt", "w")
f_res.write(json.dumps(data_all, separators=(',',':')).encode("utf8"))
f_res.close()
