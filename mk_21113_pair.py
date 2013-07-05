#!/usr/bin/python 
# serching for the pair of 21113 port 
import sys

cs_fp = file('C-S_only.dat', 'r')

sip = ['172.16.20.67','172.16.20.68','172.16.20.69']
data = {}

idxcnt = 0
for l in cs_fp.xreadlines():
  l = l.rstrip().split(',')

  """
  if (l[3] != '21113' and l[4] != '21113') : 
    idxcnt += 1
    continue
  """

  if (l[4] == '21113' and l[1] in sip) :
    if l[2] not in data : 
      n_row = '%s,%s,%s' % (l[1], l[2], '21113')
      data[l[1]] = n_row

  idxcnt += 1

  if idxcnt % 10000 == 0 : 
    print '\r', idxcnt,'/90360496', int(idxcnt*100/90360496),'%', len(data), 
    sys.stdout.flush()

print '\r', idxcnt, '/90360496 100 %', len(data)



cs_fp.close()


fp = file('TG_result7_21113.csv', 'w')
#for ip in sip : 
#  fp.write( ip + '\n')
for _k in data : 
    fp.write( data[_k] + '\n')
fp.close()

