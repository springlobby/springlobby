#!/usr/bin/env python

try:
	import simplejson as json
except:
	import json
import sys
from jinja2 import Environment, FileSystemLoader
conversion_map = { 'bool':int, 'int':int, 'std::string':str, 'float':float }

def spring_default_config(input_fn):
	inputdata = json.load(open(input_fn))
	filtered_data = dict()
	for key,value in  inputdata.iteritems():
		try:
			dtype = value['type']
			default = value['defaultValue']
			try:
				filtered_data[key] = conversion_map[dtype](default)
			except KeyError:
				print 'No conversion found for "%s" with type "%s"'%(key,dtype)
		except KeyError,k:
				print 'missing key %s in data for "%s" in file %s:%d'%(k,key,value['declarationFile'],value['declarationLine'])
	return filtered_data

def output(defaults,output_fn):
	env = Environment(loader=FileSystemLoader('.'))
	template = env.get_template('ctrlconstants.h.jinja')
	with open(output_fn,'wb') as outfile:
		outfile.write( template.render(defaults=defaults ) )

if __name__ == '__main__':
	d = spring_default_config( sys.argv[1] )
	output(d,'ctrlconstants.h')