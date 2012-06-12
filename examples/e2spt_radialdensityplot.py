#!/usr/bin/env python
#
# Author: Jesus Galaz, 06/05/2012
# Copyright (c) 2011 Baylor College of Medicine
#
# This software is issued under a joint BSD/GNU license. You may use the
# source code in this file under either license. However, note that the
# complete EMAN2 and SPARX software packages have some GPL dependencies,
# so you are responsible for compliance with the licenses of these packages
# if you opt to use BSD licensing. The warranty disclaimer below holds
# in either instance.
#
# This complete copyright notice must be included in any revised version of the
# source code. Additional authorship citations may be added, but existing
# author citations must be preserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  2111-1307 USA
#
#
	
import os, sys, commands
from EMAN2 import *
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
from pylab import figure, show	

def main():

	progname = os.path.basename(sys.argv[0])
	usage = """Aligns a 3d volume to another by executing e2classaverage3d.py and then calculates the FSC between them by calling e2proc3d.py . It returns both a number for the resolution based on the FSC0.5 
	criterion(on the screen) and a plot as an image in .png format."""
			
	parser = EMArgumentParser(usage=usage,version=EMANVERSION)
	
	parser.add_argument("--vols", type=str, help="Volume whose radial density plot you want to compute. For multiple volumes, either provide them as an .hdf stack, or separate them by commas --vols=first.hdf,second.hdf,etc...", default=None)
	parser.add_argument("--output", type=str, help="Name for the output .png and .txt files that contain the plots and the numeric values for them. Must be specified if --singleplot is on.", default=None)

	parser.add_argument("--mask",type=str,help="Mask processor applied to volumes before alignment. Default is mask.sharp:outer_radius=-2", default="mask.sharp:outer_radius=-2")
	parser.add_argument("--normproc",type=str,help="Normalization processor applied to volumes before alignment. Default is to use normalize.mask. If normalize.mask is used, results of the mask option will be passed in automatically. If you want to turn this option off specify 'None' ", default="normalize")
	parser.add_argument("--preprocess",type=str,help="Any processor (as in e2proc3d.py) to be applied to each volume prior to radial density plot computation.", default=None)
	parser.add_argument("--lowpass",type=str,help="A lowpass filtering processor (as in e2proc3d.py) to be applied to each volume prior to radial density plot computation.", default=None)
	parser.add_argument("--highpass",type=str,help="A highpass filtering processor (as in e2proc3d.py) to be applied to each volume prior to radial density plot computation.", default=None)	
	parser.add_argument("--shrink", type=int,default=1,help="Optionally shrink the input volumes by an integer amount.")	
	#parser.add_argument("--apix", type=float, help="Provide --apix to overrride the value found in the volumes' header paramter.", default=0)
	parser.add_argument("--singleplot", action="store_true",default=False,help="Plot all the Radial Density Profiles of the volumes provided in one single plot.")	

	parser.add_argument("--ppid", type=int, help="Set the PID of the parent process, used for cross platform PPID",default=-1)
	parser.add_argument("--verbose", "-v", dest="verbose", action="store", metavar="n",type=int, default=0, help="verbose level [0-9], higner number means higher level of verboseness")
	
	(options, args) = parser.parse_args()
	
	logger = E2init(sys.argv, options.ppid)

	if options.normproc: 
		options.normproc=parsemodopt(options.normproc)
	
	if options.mask: 
		options.mask=parsemodopt(options.mask)
	
	if options.preprocess: 
		options.preprocess=parsemodopt(options.preprocess)
		
	if options.lowpass: 
		options.lowpass=parsemodopt(options.lowpass)
	
	if options.highpass: 
		options.highpass=parsemodopt(options.highpass)
		
	names = options.vols
	names = names.split(',')
	
	finalvalues = {}
	for i in names:
		
		n = EMUtil.get_image_count(i)
		
		stackvalues = []
		print "The stack %s has %d images in it" % ( i, n ) 
		for j in range(n):
			a = EMData(i,j)
			radius = a.get_xsize()/2
			values = calcvalues(a,options,radius)	
			stackvalues.append(values)
		finalvalues.update({i:stackvalues})	
	
	print "\n\nfinal values are", finalvalues
	
	if options.singleplot and len(names) > 1:
		for i in names:
			if len(finalvalues[i]) > 1:
				print "ERROR: You can plot RD profiles for multiple particles IN ONE PLOT if each individual .hdf file has one particle only, or if you supply ONE stack only with all the particles in it."
				print "In this case, you've supplied %d files, and the file %s has %d particles in it" % (len(names), i, len(finalvalues[i]))
				sys.exit()

		if not options.output:
			print "ERROR: You must supply and output name if you want to plot multiple RD profiels from different .hdf files into one plot."
			sys.exit()
		else:	
			plotname = options.output
			if '.png' not in plotname:
				plotname += '.png'

			plt.title("Radial density plot")
			plt.ylabel("Density (arbitrary units)")
			plt.xlabel("Radius [pixels]")
			
			for i in names:		
				values = finalvalues[i][0]
				txtname = plotname.replace('.png', '_' + str(j).zfill(len(names)) + '.txt') 
				f = open(txtname,'w')
				lines = []
				for value in values:
					line = str(value) + '\n'
					lines.append(line)
				f.writelines(lines)
				f.close()
				
				plt.plot(values, linewidth=2)

			p = plt.gca()
			plt.savefig(plotname)
			plt.clf()

	if not options.singleplot:
		for i in names:
			plotname = ''
			if options.output:
				plotname = options.output
			else:
				plotname = i.replace('.hdf','.png')
			
			if plotname and '.png' not in plotname:
				plotname += '.png'
				
			#print "\n\nfinal values for", i
			#print "are", finalvalues[i]
			#print "\n\n"
			
			for j in range(len(finalvalues[i])):
				plotname = plotname.replace('.png','_' + str(j).zfill(len(finalvalues[i])) + '.png')
				txtname = plotname.replace('.png','.txt')
				values = finalvalues[i][j]
				
				plt.title("Radial density plot")
				plt.ylabel("Density (arbitrary units)")
				plt.xlabel("Radius [pixels]")
				
				plt.plot(values, linewidth=2)
				a = plt.gca()
				a.set_xlim([0,radius])
				plt.savefig(plotname)
				plt.clf()


	return()				
				
def calcvalues(a,options,radius):
	# Make the mask first, use it to normalize (optionally), then apply it 
	mask=EMData(a["nx"],a["ny"],a["nz"])
	mask.to_one()

	if options.mask != None:
		mask.process_inplace(options.mask[0],options.mask[1])

	# normalize
	if options.normproc != None:
		#if options["normproc"][0]=="normalize.mask": 
		#	options["normproc"][1]["mask"]=mask
		a.process_inplace(options.normproc[0],options.normproc[1])

	a.mult(mask)

	if options.normproc != None:
		#if options["normproc"][0]=="normalize.mask": 
		#	options["normproc"][1]["mask"]=mask
		a.process_inplace(options.normproc[0],options.normproc[1])

	a.mult(mask)

	# preprocess
	if options.preprocess != None:
		a.process_inplace(options.preprocess[0],options.preprocess[1])

	# lowpass
	if options.lowpass != None:
		a.process_inplace(options.lowpass[0],options.lowpass[1])

	# highpass
	if options.highpass != None:
		a.process_inplace(options.highpass[0],options.highpass[1])

	# Shrink
	if options.shrink !=None and options.shrink>1 :
		a=a.process("math.meanshrink",{"n":options.shrink})

	values = a.calc_radial_dist(radius, 0, 1, 1)
	
	return(values)

	
if __name__ == '__main__':
	main()
