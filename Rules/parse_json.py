import json
import sys
# 读取json数据
with open(sys.argv[1],'r') as file:
    data = json.load(file)