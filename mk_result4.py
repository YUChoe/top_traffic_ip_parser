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
        if tip[0] == '61' and (not (tip[1] == '33' and tip[2] == '233')) :
                return False
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
            # 2013-06-08 23:59:28,61.33.233.29,172.20.152.7,80,3348,538083268,4,6,S-C
            continue
            #server_ip = r[1]
            #listen_port = r[4]
        elif r[8] == 'C-S' : 
            # 2013-06-08 23:59:28,172.25.0.213,172.29.0.1,58900,22,-1308739514,24,6,C-S
            client_ip = r[1]
            server_ip = r[2]
            req_port = r[3]
            listen_port = r[4]
        else :
            continue

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
        elif port_data.has_key(client_ip) :
            # mismatch ? 
            continue
            #_ports = port_data[client_ip]
            #if req_port not in _ports : 
            #    _ports.append(req_port)

        else : 
            _ports = [listen_port]
            port_data[server_ip] = _ports
            #print "new", server_ip, listen_port
    print "finished -", len(port_data)
    fp.close()
    break

total_rows = len(port_data)
fp = file('TG_result5.csv', 'w')
idxcount = 0
for server_ip in port_data : 
    listen_ports = port_data[server_ip]
    _str = '%s,%s\n' % (server_ip, ','.join(listen_ports))
    fp.write(_str)
    idxcount += 1
    if idxcount % 1000 == 0 : 
        print idxcount, '/', total_rows


print idxcount, '/', total_rows











