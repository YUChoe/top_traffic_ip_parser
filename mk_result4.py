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

# squuze function 
def addressInNetwork(ip, net):
   ipaddr = int(''.join([ '%02x' % int(x) for x in ip.split('.') ]), 16)
   netstr, bits = net.split('/')
   netaddr = int(''.join([ '%02x' % int(x) for x in netstr.split('.') ]), 16)
   mask = (0xffffffff << (32 - int(bits))) & 0xffffffff
   return (ipaddr & mask) == (netaddr & mask)


# -- main -- 

result5 = {}
fp = file('TG_result5_backup.csv')
for l in fp.readlines():
    l = l[:-1].split(',')
    sip = l[0]
    ports = l[1:]
    result5[sip] = ports

#
for filename in source_files_ls.split('\n') :

    port_data = {} # for loop 2 
    # '') dip : [dports ...]
    all_port_data = {} # for loop 1
    # '')
    ip_groups = {}

    _time = time.localtime()
    print "processing", filename, '%02d:%02d:%02d' % (_time[3], _time[4], _time[5])
    fp = file(filename, 'r')
    dbgcnt = 0

    # 1st loop: sip-dip or dip-sip pairing > make very large dictionary 
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

        # result6 start 
        if ('%s,%s' % (server_ip, client_ip)) in all_port_data :
            #mismatch
            continue 

        _client_ip_16 = '.'.join(client_ip.split('.')[0:2]) + '.0.0/16'

        if (server_ip not in ip_groups) and (server_ip not in result5) :
            continue
        elif (server_ip not in ip_groups) and (server_ip in result5) :
            _v5 = {}
            _v6 = []
            if listen_port in result5[server_ip] : 
                _v6.append(listen_port)
            elif req_port in result5[server_ip] : 
                _v6.append(req_port)
            if len(_v6) > 0 :
                _v5[_client_ip_16] = _v6
                ip_groups[server_ip] = _v5
        else :
            if _client_ip_16 in ip_groups[server_ip] : 
                _v5 = ip_groups[server_ip]
                _v6 = _v5[_client_ip_16]
                if (listen_port not in _v6) and (listen_port in result5[server_ip]) : 
                    _v6.append(listen_port)
                elif (req_port not in _v6) and (req_port in result5[server_ip]) : 
                    _v6.append(req_port)
                _v5[_client_ip_16] = _v6
                ip_groups[server_ip] = _v5
            else :
                _v5 = ip_groups[server_ip]
                _v6 = []
                if (listen_port not in _v6) and (listen_port in result5[server_ip]) : 
                    _v6.append(listen_port)
                elif (req_port not in _v6) and (req_port in result5[server_ip]) : 
                    _v6.append(req_port)
                else :
                    # not append 
                    continue
                _v5[_client_ip_16] = _v6
                ip_groups[server_ip] = _v5

        dbgcnt += 1
        if dbgcnt % 10000 == 0 :
            print '\r', dbgcnt,
            sys.stdout.flush()

    print '\r', dbgcnt
    _time = time.localtime()
    print "ip_groups finished for", filename, len(ip_groups), '%02d:%02d:%02d' % (_time[3], _time[4], _time[5])

    fp.close()
    

    #print port_data['10.200.33.1']

    #total_rows = len(port_data)
    fp = file('TG_result6_%s.csv' % (filename), 'w')
    #idxcount = 0
    # 3rd loop : write into the output file 

    for server_ip in ip_groups :
        v5 = ip_groups[server_ip]
        for client_group in v5 :
            v6 = v5[client_group]
            try:
                _str = '%s,%s,%s\n' % (client_group, server_ip, ','.join(sorted(v6)))
            except:
                print "Error2:", client_group, server_ip, v6
                sys.exit(-1)    
            fp.write(_str)
    fp.close()
    print "output", 'TG_result6_%s.csv' % (filename),
    

"""
del port_data
del all_port_data


for filename in source_files_ls.split('\n') :
    filename = 'TG_result5_%s.csv' % (filename)
    fp = file(filename, 'r')
    # merge 

"""


