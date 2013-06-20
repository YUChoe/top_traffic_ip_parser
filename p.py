#!/usr/bin/python 

source_file = "/Users/pp/Downloads/TG_0619/2013-06-19.group.sort"
# DMZ,61.33.233.1,DMZ,53,61.33.233.34,17,45
s_fp = file(source_file, 'r')

data = {}


debug_line_count = 0;

for line in s_fp.readlines() :
  debug_line_count = debug_line_count + 1
  #if debug_line_count > 1000 :
  #  break
  if debug_line_count % 10000 == 0 : 
    print debug_line_count
  #  
  line = line[:-1]
  row = line.split(',')
  key = '%s,%s,%s,%s' % (row[0],row[1],row[2],row[4])
  if data.has_key(key) == True :
    v_tmp = data[key]
    if row[5] in v_tmp : 
      continue
    if row[5] == '17' :
      v_tmp.append('udp%s' % (row[3])) 
    elif row[5] == '6' :
      v_tmp.append('%s' % (row[3])) 
    else : 
      print "unknown proto:%s", row[5]
      v_tmp.append('%s' % (row[3])) 
    data[key] = v_tmp
  else : 
    data[key] = []
    v_tmp = data[key]
    if row[5] == '17' :
      v_tmp.append('udp%s' % (row[3])) 
    elif row[5] == '6' :
      v_tmp.append('%s' % (row[3])) 
    else :  
      print "unknown proto:%s", row[5]
      v_tmp.append('%s' % (row[3])) 
    data[key] = v_tmp
print debug_line_count
s_fp.close()

debug_line_count = 0;
d_fp = file('/Users/pp/Downloads/TG_0619/2013-06-19.groupport.csv', 'w')
d2_fp = file('/Users/pp/Downloads/TG_0619/2013-06-19.others_groupport.csv', 'w')
d3_fp = file('/Users/pp/Downloads/TG_0619/2013-06-19.only_udp53.csv', 'w')
d4_fp = file('/Users/pp/Downloads/TG_0619/2013-06-19.only_udp137.csv', 'w')
for key in data : 
  debug_line_count = debug_line_count + 1
  if debug_line_count % 5000 == 0 : 
    print debug_line_count
  p = data[key]
  p = '\n'.join(p)
  if 'udp53' == p :
    d3_fp.write('%s,"%s"\n' % (key,p))
  elif 'udp137' == p :
    d4_fp.write('%s,"%s"\n' % (key,p))
  elif 'Others' in key :
    d2_fp.write('%s,"%s"\n' % (key,p))
  else :
    d_fp.write('%s,"%s"\n' % (key,p))
print debug_line_count
d2_fp.close()
d3_fp.close()
d4_fp.close()
d_fp.close()