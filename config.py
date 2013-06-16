import pickle

config = { 'source_filename' : 'abc.csv', 
           'result_filename' : 'result_3.csv' }

output = open('config.pkl', 'wb')

pickle.dump(config, output)
output.close()

