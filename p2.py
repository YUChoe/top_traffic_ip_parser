
import sys

if len(sys.argv) != 2 : 
  print "Usage:"
  print sys.argv[0], "2013-06-20.groupport.csv"
  sys.exit()

source_file = sys.argv[1]
file_prefix = source_file.split('.')[0]

d_fp = file(file_prefix + '.groupport.csv', 'r')

# normailize 
lines = []
tmp_line = ''
for line in d_fp.readlines() :
    line = line[:-1]
    if line[-1] != '"' :
        if tmp_line != '' :
            tmp_line = tmp_line + ' ' + line
        else :
            tmp_line = line
        continue
    else :
        if tmp_line != '' :
            lines.append(tmp_line + ' ' + line)
        else :
            lines.append(line)
    tmp_line = ''
d_fp.close()

# sort by flow 
datas = {}

for line in lines : 
    r = line.split(',')    
    # DMZ,61.33.233.21,VPN,192.168.242.63,"80"
    group_name = '%s,%s' % (r[0], r[2])

    if group_name == 'DMZ,VPN' :
        group_name = 'VPN,DMZ'
        line = '%s,%s,%s,%s,%s' % (r[2],r[3],r[0],r[1],r[4])

    if datas.has_key(group_name) : 
        data = datas[group_name]
        data.append(line)
        datas[group_name] = data
    else :
        datas[group_name] = []
        data = datas[group_name]
        data.append(line)
        datas[group_name] = data

del lines
sdata = {}
for group_name_key in datas :
    lines = datas[group_name_key]
    gdata = {}
    for line in lines :
        r = line.split(',')    
        # DMZ,61.33.233.21,VPN,192.168.242.63,"80"
        if gdata.has_key(r[4]) : 
            value = gdata[r[4]]
            if r[1] not in value['sip'] : 
                value['sip'].append(r[1])
            if r[3] not in value['dip'] :
                value['dip'].append(r[3])
        else : 
            value = {}
            value['sip'] = [r[1]]
            value['dip'] = [r[3]]
            gdata[r[4]] = value
    sdata[group_name_key] = gdata

#print sdata['VPN,DMZ']
"""
{
'"80"': 
  {'sip': ['192.168.152.212', '192.168.1.37'], 
  'dip': ['61.33.233.80']}, 

'"udp500"': 
  {'sip': ['192.168.11.218', '192.168.11.2', '192.168.133.71', '192.168.140.220', '192.168.11.193', '192.168.11.81', '192.168.131.60', '192.168.11.226', '192.168.11.147', '192.168.11.123', '192.168.157.25'], 
  'dip': ['61.33.233.76']}, 

'"udp1 udp1024 udp500"': 
  {'sip': ['192.168.131.205'], 
  'dip': ['61.33.233.76']}, 

'"udp3 udp4 udp5 udp500"': 
  {'sip': ['192.168.242.100'], 
  'dip': ['61.33.233.76']}
}
"""

fp = file(file_prefix + ".groupresort.csv", 'w')
for k in sdata : 
    print 'processing', k
    for port_key in sdata[k] :
        card = sdata[k][port_key]
        if len(card['sip']) > 1 : card['sip'].sort() 
        if len(card['dip']) > 1 : card['dip'].sort() 
        sip = '\n'.join(card['sip'])
        dip = '\n'.join(card['dip'])
        port = '\n'.join(port_key[1:-1].split(' '))
        line = '%s,"%s","%s","%s"\n' % (k, sip, port, dip)
        # VPN,DMZ,"1.1.1.1\n1.1.1.2","80\n443","3.3.3.3\n3.3.3.4"
        fp.write(line)

fp.close()

print file_prefix + ".groupresort.csv"



