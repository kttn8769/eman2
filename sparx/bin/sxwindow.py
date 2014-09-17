#!/usr/bin/env python
#
# Author: T. Durmaz 08/29/2014 (tunay.durmaz@uth.tmc.edu)
# Copyright (c) 2014 The University of Texas - Houston Medical School
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
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
import os, sys
import json

from optparse import *
from EMAN2 import *
from EMAN2db import *
from EMAN2jsondb import *
from emboxerbase import *

from utilities import *
from fundamentals import *
from filter import *
from global_def import *

def read_coordinates(location):
	"""
	Read coordinates from .json files. Each micrograph outputs separate json file.
	Usually generated by executing:
		$ e2boxer.py --write_dbbox mic*.hdf --format=json
	"""
	output = {}
	for _f in os.listdir(location):
		if _f.endswith('.json'):
			data = json.loads(open(os.path.join(location, _f), 'r').read())
			for mic_name, coordinates in data.items():
				output[str(mic_name)] = coordinates
	return output

def window(data):
	"""
	Using coordinates window particles, and add ctf information to it.
	"""
	for k, info in data.items():
		print 'Processing {0}'.format(k)
		box_size = data[k]['box_size']
		pixel_ratio = float(data[k]['input_pixel'])/float(data[k]['output_pixel'])
		img = EMData()
		img.read_image(k)
		img_filt = filt_gaussh(img, pixel_ratio/box_size)


		if pixel_ratio != 1.0:
			print "Generating downsampled image\n"
			sb = Util.sincBlackman(15, .5 * pixel_ratio,1999) # 1999 taken directly from util_sparx.h
			img_filt = img_filt.downsample(sb, pixel_ratio)
			box_size = box_size / pixel_ratio

		output_file_name = 'out_' + os.path.basename(k)
		clip = EMData()
		for i, (x, y) in enumerate(data[k]['coordinates']):
			reg = Region((x * pixel_ratio)-box_size//2, (y * pixel_ratio)-box_size//2, box_size, box_size)
			clip = img_filt.get_clip(reg)
			clip.write_image(output_file_name, i)
		# Set ctf
		set_ctf(clip, data[k]['ctf'])
		print 'Windowed prticles for {0} -> {1}'.format(k, output_file_name)


def main():
	progname = os.path.basename(sys.argv[0])
	usage = progname + " --coords_dir=coords_dir  --importctf=ctf_file  --topdir=topdir  --input_pixel=input_pixel  --output_pixel=output_pixel"
	
	parser = OptionParser(usage, version=SPARXVERSION)

	parser.add_option('--coords_dir',   dest='coordsdir',                help='Directory containing particle coordinates')
# 	parser.add_option('--importctf',    dest='ctffile',                  help='File name with CTF parameters produced by sxcter.')
 	parser.add_option('--topdir',       dest='topdir',       default='./', help='Path name of directory containing relevant micrograph directories')
	parser.add_option('--input_pixel',  dest='input_pixel',  default=1,  help='input pixel size')
	parser.add_option('--output_pixel', dest='output_pixel', default=1,  help='output pixel size')
	parser.add_option('--box_size',     dest='box_size',     type=int,   help='box size')
	parser.add_option('--outdir',     dest='outdir',      help='Output directory')
	parser.add_option('--outstack',     dest='outstack',      help='Output stack name')
	
	parser.add_option("--invert",     help="If writing outputt inverts pixel intensities",default=False)
	???parser.add_option("--norm", type=str,help="Normalization processor to apply to written particle images. Should be normalize, normalize.edgemean,etc.Specifc \"None\" to turn this off", default="normalize.edgemean")

	(options, args) = parser.parse_args()
	box_size = options.box_size
	
	if len(args) > 0:
		print "\nusage: " + usage
		print "Please run '" + progname + " -h' for detailed options\n"
	else:
		database = "e2boxercache"
		??db = js_open_dict(os.path.join(database,"quality.json"))
		suffix    = str(db['suffix'])    # db['suffix'] is unicode, so need to call str()
		extension = str(db['extension']) # db['extension'] is unicode, so need to call str()
		
		??dbGAUSS = js_open_dict(os.path.join(database,"gauss_box_DB.json"))
		??use_variance = str(dbGAUSS['demoparms']['use_variance'])
		
		info_suffix = '_info.json'
		
		micnames = []
		import glob
		
		for f in glob.glob(os.path.join(options.topdir, ???'*.hdf')):   # currently handles only hdf formatted micrographs
			micnames.append(base_name(f))
		
		mask = pad(model_circle(box_size//2, box_size, box_size),??? box_size, box_size, 1, 0.0)
		
# 		otcl_images  = "bdb:%s/"%options.outdir + options.outstack + suffix
# 		iImg=0

		for i in range(len(micnames)):
			if i>0: continue
			basename = micnames[i]
			f_mic = os.path.join(options.topdir, basename + extension)
			f_info = info_name(f_mic)
			
			otcl_images  = "bdb:%s/"%options.outdir + basename + suffix
			print basename, f_info, f_mic

			im = get_im(f_mic)
			
			if options.invert:
				[avg,sigma,fmin,fmax] = Util.infomask( im, None, True )
				im -= avg
				im *= -1
				im += avg
			
# 			im = ramp(im)
			img_filt = filt_gaussh( im, ??0.015625)
			
			subsample_rate = options.input_pixel / options.output_pixel
			if subsample_rate != 1.0:
				print "Generating downsampled image\n"
				????sb = Util.sincBlackman(template_min, frequency_cutoff,1999) # 1999 taken directly from util_sparx.h
				small_img = img_filt.downsample(sb,subsample_rate)
				del sb
			else:
				small_img = ???img_filt.copy()
		
			[avg,sigma,fmin,fmax] = Util.infomask( small_img, None, True )
			??small_img -= avg
			small_img /= sigma
			
			if(use_variance):
				from morphology import power
				???small_img = power(small_img, 2.0)
				print "using variance"

			x0 = small_img.get_xsize()//2  #  Floor division or integer division
			y0 = small_img.get_ysize()//2

			coords = js_open_dict(f_info)["boxes"]

			for i in range(len(coords)):

				x = int(coords[i][0])
				y = int(coords[i][1])
				
				image = Util.window(small_img, box_size, box_size, 1, x-x0, y-y0)

				???if options.invert: image.mult(-1)
				???if str(options.norm) != "None": image.process_inplace(options.norm)

				image.write_image(otcl_images, i)
# 				imn.write_image(otcl_images, iImg)
# 				iImg = iImg + 1

if __name__=='__main__':
	main()
