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

def in_pok(ipaddr) :
    tip = ipaddr.split('.')
    if tip[0] in ['10', '172','192','190','100','61'] : 
        if tip[0] == '61' and (not (tip[1] == '33' and tip[2] == '233')) :
                return False
        return True
    else : 
        return False

for filename in source_files_ls.split('\n') :

    port_data = {}
    # dip : [dports ...]
    all_port_data = {}

    _time = time.localtime()
    print "processing", filename, '%02d:%02d:%02d' % (_time[3], _time[4], _time[5])
    fp = file(filename, 'r')
    dbgcnt = 0

    # 1si loop: sip-dip or dip-sip pairing > make very large dictionary 
    for line in fp.readlines() :
        line = line[:-1]
        if line == '' : continue
        r = line.split(',')
        if len(r) < 8 : 
            print 'Error:', line
            continue
        if r[7] == '17' : # UDP
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
        if not in_pok(server_ip) or not in_pok(client_ip) : continue

        real_key_all_port_data = ''

        if ('%s,%s' % (client_ip, server_ip)) in all_port_data :
            real_key_all_port_data = '%s,%s' % (client_ip, server_ip)
            _values = all_port_data[real_key_all_port_data]
            _req_ports = _values[0]
            _listen_ports = _values[1]
            _cnt = _values[2]
            if req_port not in _req_ports : 
                _req_ports.append(req_port)
            if listen_port not in _listen_ports :
                _listen_ports.append(listen_port)
            all_port_data[real_key_all_port_data] = [_req_ports, _listen_ports, (_cnt+1)] 
        elif ('%s,%s' % (server_ip, client_ip)) in all_port_data :
            #mismatch
            """
            real_key_all_port_data = '%s,%s' % (server_ip, client_ip)
            _values = all_port_data[real_key_all_port_data]
            _req_ports = _values[0]
            _listen_ports = _values[1]
            _cnt = _values[2]
            if listen_port not in _req_ports : 
                _req_ports.append(listen_port)
            if _req_ports not in _listen_ports :
                _listen_ports.append(_req_ports)
            all_port_data[real_key_all_port_data] = [_req_ports, _listen_ports, (_cnt+1)]
            """
        else :
            # new all_port_data C-S flow 
            real_key_all_port_data = '%s,%s' % (client_ip, server_ip)
            _req_ports = [req_port]
            _listen_ports = [listen_port]
            all_port_data[real_key_all_port_data] = [_req_ports, _listen_ports, 1]

        dbgcnt += 1
        if dbgcnt % 10000 == 0 :
            print '\r', dbgcnt,
            sys.stdout.flush()

    print '\r', dbgcnt
    _time = time.localtime()
    print "all_port_data", len(all_port_data), '%02d:%02d:%02d' % (_time[3], _time[4], _time[5])


    # 2nd loop choose service ports 
    for _key in all_port_data : 
        _values = all_port_data[_key]
        if _values[2] < 5 : 
            continue
        
        if len(_values[0]) > len(_values[1]) :
            # C-S flow 
            (client_ip, server_ip) = _key.split(',')
            if server_ip in port_data :
                _ports = port_data[server_ip]
                _ports = sorted( set(_ports + _values[1]))
                #for _p in _values[1] :
                #    if _p not in _ports :
                #        _ports.append(_p)
                port_data[server_ip] = _ports
            else :
                _ports = []
                for _p in _values[1] :
                    if _p not in _ports :
                        _ports.append(_p)
                port_data[server_ip] = _ports

        elif len(_values[0]) < len(_values[1]) :
            # S-C flow - mismatch case 
            (client_ip, server_ip) = _key.split(',')
            if server_ip in port_data :
                _ports = port_data[server_ip]
                _ports = sorted( set(_ports + _values[0]))
                #for _p in _values[0] :
                #    if _p not in _ports :
                #        _ports.append(_p)
                
                port_data[server_ip] = _ports
            else :
                _ports = []
                for _p in _values[0] :
                    if _p not in _ports :
                        _ports.append(_p)
                port_data[server_ip] = _ports

    _time = time.localtime()
    print "finished - step 2 ", len(port_data), '%02d:%02d:%02d' % (_time[3], _time[4], _time[5])
    
    fp.close()
    

    #print port_data['10.200.33.1']

    #total_rows = len(port_data)
    fp = file('TG_result5_%s.csv' % (filename), 'w')
    #idxcount = 0
    for server_ip in port_data : 
        listen_ports = port_data[server_ip]
        try:
            _str = '%s,%s\n' % (server_ip, ','.join(listen_ports))
        except:
            print "Error2:", server_ip, listen_ports
            sys.exit(-1)
        fp.write(_str)
        #idxcount += 1
        #if idxcount % 1000 == 0 : 
        #    print idxcount, '/', total_rows


    #print idxcount, '/', total_rows
    fp.close()
    print "output", 'TG_result5_%s.csv' % (filename),

del port_data
del all_port_data


for filename in source_files_ls.split('\n') :
    filename = 'TG_result5_%s.csv' % (filename)
    fp = file(filename, 'r')
    # merge 







