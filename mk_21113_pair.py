#!/usr/bin/python 
# serching for the pair of 21113 port 
import sys

cs_fp = file('C-S_only.dat', 'r')

sip = ['172.16.20.67','172.16.20.68','172.16.20.69']
data = []

idxcnt = 0
for l in cs_fp.xreadlines():
  l = l.rstrip().split(',')

  if (l[3] != '21113' and l[4] != '21113') : 
    idxcnt += 1
    continue

  if (l[3] == '21113' and l[2] in sip) or (l[4] == '21113' and l[1] in sip) :
    pass
  elif l[1] not in sip :
    sip.append(l[2])
    data.append(l)
  elif l[2] not in sip : 
    sip.append(l[1])
    data.append(l)

  idxcnt += 1

  if idxcnt % 10000 == 0 : 
    print '\r', idxcnt,'/90360496', len(sip),
    sys.stdout.flush()

print '\r', idxcnt, '/90360496', len(sip)



cs_fp.close()


fp = file('TG_result7_21113.csv', 'w')
#for ip in sip : 
#  fp.write( ip + '\n')
for l in data : 
    fp.write(','.join(l) + '\n')
fp.close()

