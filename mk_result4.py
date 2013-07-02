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

port_data = {}
# dip : [dports ...]

def in_pok(ipaddr) :
    tip = ipaddr.split('.')
    if tip[0] in ['10', '172','192','190','100','61'] : 
        return True
    else : 
        return False


for filename in source_files_ls.split('\n') :
    _time = time.localtime()
    print "processing", filename, '%02d:%02d:%02d' % (_time[3], _time[4], _time[5])
    fp = file(filename, 'r')
    for line in fp.readlines() :
        line = line[:-1]
        if line == '' : continue
        r = line.split(',')
        if len(r) < 8 : 
            print 'Error:', line
            continue
        if r[8] == 'S-C' : 
            continue
            #server_ip = r[1]
            #listen_port = r[4]
        elif r[8] == 'C-S' : 
            server_ip = r[2]
            listen_port = r[4]

        if not in_pok(server_ip) : continue

        if port_data.has_key(server_ip) : 
            _ports = port_data[server_ip]
            if listen_port not in _ports :
                _ports.append(listen_port)
                _ports = sorted(_ports)
                port_data[server_ip] = _ports
                #print "appended", server_ip, listen_port, len(_ports)
            else :
                #print "passed", server_ip, listen_port
                continue
        else : 
            _ports = [listen_port]
            port_data[server_ip] = _ports
            #print "new", server_ip, listen_port
    print "finished -", len(port_data)
    fp.close()

total_rows = len(port_data)
fp = file('TG_result4.csv', 'w')
idxcount = 0
for server_ip in port_data : 
    listen_ports = port_data[server_ip]
    _str = '%s,%s\n' % (server_ip, ','.join(listen_ports))
    fp.write(_str)
    idxcount += 1
    if idxcount % 1000 == 0 : 
        print idxcount, '/', total_rows


print idxcount, '/', total_rows











