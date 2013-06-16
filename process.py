import pickle

pkl_file = open('config.pkl', 'rb')
config = pickle.load(pkl_file)

result = {}
fp = file(config['source_filename'], "r")

for line in fp.readlines() :
  row = line.split(',')
  # row 
  # 0:tcp/udp 
  # 1:source ip 2:destination ip
  # 3: port number 
  # 4: count
  if len(row) != 5 : 
    continue

  if row[0] == "6" : 
    proto = "tcp"
  elif row[0] == "1" :
    proto = "icmp"
  elif row[0] == "17":
    proto = "udp"
  else :
    proto = "????"

  tip = row[2].split('.');
  
  _write = False
  
  if tip[0] == '10' or tip[0] == '172' or tip[0] == '192' or tip[0] == '190' or tip[0] == '100' :
    _write = True
  
  if _write == False : continue
  
  sipdipproto = "%s,%s,%s" % (row[1], row[2], proto)

  if result.has_key(sipdipproto) :
    # todo array later
    # old_ports = result[sipdipproto][0] 
    old_count = result[sipdipproto][1]
    result[sipdipproto][1] = old_count + int(row[4])
  else :
    result[sipdipproto] = [ row[3], int(row[4]) ]
fp.close()

fp = file(config['result_filename'], "w")
for key in result :
  value = result[key]
  row = "%s,%s,%d\r\n" % (key, value[0], value[1])
  fp.write(row)

fp.close()

