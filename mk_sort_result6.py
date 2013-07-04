#!/usr/bin/python 
filename = 'TG_result6_all_working.csv'

data = []

fp = file(filename, 'r')
for l in fp.readlines() :
    l = l[:-1].split(',')
    _client_nw = l[0]
    _server_ip = l[1]
    _ports = l[2:]
    _nports = []
    for _p in _ports : 
        if _p == '' : 
            continue
        try : 
            _nports.append(int(_p))
        except:
            print _p, l
            import sys
            sys.exit()
    _nports = sorted(_nports)
    _ports = [str(i) for i in _nports]

    data.append('%s,%s,%s\n' % (_client_nw, _server_ip, ','.join(_ports)))
fp.close()

fp = file(filename+'_.csv', 'w')
fp2 = file(filename+'_21113.csv', 'w')
for l in data :
    if ',21113' in l :
        fp2.write(l)
    else:
        fp.write(l)
fp2.close()
fp.close()