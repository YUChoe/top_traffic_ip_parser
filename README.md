top_traffic_ip_parser
=====================

analyze the result from Smartexpert > **top_sip_dip_service.pl**

### SmartExpert

```
$ cd /usr/local/Smartexpert/bin
$ ./????? 1 eth3 eth4
```

*few hours for gathering the traffic data*

```
$ cd /usr/local/Smartexpert/bin
$ ls -l ../data
$ ./top_sip_dip_service.pl # parse and gether the datas as csv format 
---------------------------------------------------------------------------
eNClue Co, LTD.                        
COPYRIGHT 2001-2006
PROTOCOL-SIP-DIP-SERVICE-SUMBYTE
---------------------------------------------------------------------------
Continue? (Y/N): y
...
$ cd ../data
$ cat *.csv > result_total.csv 
```

### work flow 
* ~/data/*.dat
* ~/bin/top_sip_dip_service.pl
* ~/data/2013-xx-xx.toppacket
* ~/bin/classify_group.pl
* ~/etc/ip.group
* ~/data/2013-xx-xx.group.sort
* python p.py 2013-xx-xx.group.sort
* 2013-xx-xx.groupport.csv, *.others_groupport.csv, *.only_udp53.csv, *.only_udp137.csv
* python p2.py 2013-xx-xx.groupport.csv
* 2013-xx-xx.groupresort.csv

### Usage 

```
$ cd /usr/local/Smartexprty/
$ git clone git://github.com/YUChoe/top_traffic_ip_parser.git
$ cd top_traffic_ip_parser
$ vi config.py 
# config source_filename, result_filename and path of data directory(../data) 
...
$ python config.py # for making config.pkl
```
```
$ python process.py 
```
