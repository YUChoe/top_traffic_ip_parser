#!/usr/bin/python

import sys
import time

source_files_ls = """2013-05-29.dat
2013-05-30-1.dat
2013-05-30.dat
2013-05-31-1.dat
2013-05-31.dat
2013-06-01.dat
2013-06-02.dat
2013-06-03-1.dat
2013-06-03.dat
2013-06-04-1.dat
2013-06-04.dat
2013-06-05-1.dat
2013-06-05.dat
2013-06-06.dat
2013-06-07-1.dat
2013-06-07.dat
2013-06-08.dat
2013-06-09.dat
2013-06-10-1.dat
2013-06-10.dat"""

def _now() :
    _time = time.localtime()
    return str('%02d:%02d:%02d' % (_time[3], _time[4], _time[5]))

all_datas = {}

for filename in source_files_ls.split('\n') :
    filename = 'TG_result6_%s.csv' % (filename)
    print "start reading", filename, _now
    fp = file(filename, 'r')
    for l in fp.readlines() :
        l = l[:-1].split(',')
        _client_nw = l[0]
        _server_ip = l[1]
        _ports = l[2:]
        _key = '%s,%s' % (_client_nw, _server_ip)
        if _key not in all_datas :
            all_datas[_key] = _ports
        else :
            _value = all_datas[_key]
            for _p in _ports :
                if _p not in _value :
                    _value.append(_p)
            all_datas[_key] = sorted(_value)
    fp.close()
    print "read finished", _now

idxcnt = 0
fp = file('TG_result6_all.csv','w')
for _key in all_datas :
    _ports = all_datas[_key]
    (_client_nw, _server_ip) = _key.split(',')
    _str = '%s,%s,%s\n' % (_client_nw, _server_ip, ','.join(_ports))
    fp.write(_str)
    idxcnt += 1
    if idxcnt % 1000 == 0 : 
        print '\r', idxcnt, 

print '\r', idxcnt, 'finished'
fp.close()
