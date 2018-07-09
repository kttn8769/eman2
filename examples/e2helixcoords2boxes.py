#!/usr/bin/env python
from __future__ import print_function

from EMAN2 import *
import numpy as np

def main():
	progname = os.path.basename(sys.argv[0])
	usage = """e2helixcoords2boxes.py [options] <boxfile> <boxfile> ... <boxfile>
	
	Convert helix particle coordinates generated by the EMAN2 helix boxer into .box files to be read by EMAN2 or RELION.
	
	Example: e2helixcoords2boxes.py --boxsize 256 my_project/helical_boxes/*_boxes.txt
	"""
	
	parser = EMArgumentParser(usage=usage,version=EMANVERSION)
	parser.add_argument("--boxsize", type=int, help="Set the boxsize used to compute power spectra across movie frames",required=True)
	parser.add_argument("--savetodir", type=int, help="Specify the directory into which you want the files saved. Default is the current directory.",default=None)
	(options, args) = parser.parse_args()
	
	pid=E2init(sys.argv)
	
	if options.savetodir: dir=options.savetodir
	else: dir = os.getcwd()
	
	for fname in args:
		a = np.loadtxt(fname)# './GL1089B-40K-40e-PC6s_0007.box'
		b = np.zeros_like(a)
		c = np.hstack([a-options.boxsize/2,b+options.boxsize])
		if "boxes" in fname: out = dir+"/"+fname.split('.')[-2][:-6]+".box"
		else: out = dir+"/"+fname.split('.')[-2]+".box"
		np.savetxt(out,c,delimiter="\t",fmt="%d")

if __name__=="__main__":
	main()
