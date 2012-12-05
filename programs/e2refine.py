#!/usr/bin/env python

#
# Author: David Woolford, 10/19/2007 (woolford@bcm.edu)
# Copyright (c) 2000-2007 Baylor College of Medicine
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


from EMAN2 import *
from EMAN2fsc import *
from EMAN2db import db_open_dict, db_close_dict, db_check_dict
from math import *
import os
import sys
import traceback

def main():
	progname = os.path.basename(sys.argv[0])
	usage = """prog [options] 
	
	Single particle reconstruction refinement program. This is the main program used to perform
	iterative single-model single particle reconstruction in EMAN2. It has MANY options, many
	of which are passed on to other programs called as part of this process. For more information
	on the parameters and using this program, we suggest reading the tutorial, using the
	e2workflow.py interface or http://blake.bcm.edu/emanwiki/EMAN2/Programs/e2refine
"""
	parser = EMArgumentParser(usage=usage,version=EMANVERSION)
		
	#options associated with e2refine.py
	parser.add_header(name="refineheader", help='Options below this label are specific to e2refine', title="### e2refine options ###", row=1, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_header(name="modelheader", help='Options below this label are specific to e2refine Model', title="### e2refine model options ###", row=4, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--iter", dest = "iter", type = int, default=0, help = "The total number of refinement iterations to perform", guitype='intbox', row=2, col=2, rowspan=1, colspan=1, mode="refinement[2]")
	parser.add_argument("--startiter", dest = "startiter", type = int, default=0, help = "If a refinement crashes, this can be used to pick it up where it left off. This should NOT be used to change parameters, but only to resume an incomplete run.")
	parser.add_argument("--check", "-c", dest="check", default=False, action="store_true",help="Checks the contents of the current directory to verify that e2refine.py command will work - checks for the existence of the necessary starting files and checks their dimensions. Performs no work ")
	parser.add_argument("--verbose", "-v", dest="verbose", action="store", metavar="n", type=int, default=0, help="verbose level [0-9], higner number means higher level of verboseness")
	parser.add_argument("--input", dest="input", default=None,type=str, help="The name of the set containing the particle data", browser='EMSetsTable(withmodal=True,multiselect=False)', guitype='filebox', row=0, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--model", dest="model", type=str,default="threed.0a.mrc", help="The name of the 3D image that will seed the refinement", guitype='filebox', browser='EMModelsTable(withmodal=True,multiselect=False)', row=5, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--usefilt", dest="usefilt", type=str,default=None, help="Specify a particle data file that has been low pass or Wiener filtered. Has a one to one correspondence with your particle data. If specified will be used in projection matching routines, and elsewhere.")
	parser.add_argument("--path", default=None, type=str,help="The name of a directory where results are placed. If unspecified will generate one automatically of type refine_??.")
	parser.add_argument("--mass", default=0, type=float,help="The mass of the particle in kilodaltons, used to run normalize.bymass. If unspecified (set to 0) nothing happens. Requires the --apix argument.", guitype='floatbox', row=2, col=1, rowspan=1, colspan=1, mode="refinement['self.pm().getMass()']")
	parser.add_argument("--apix", default=0, type=float,help="The angstrom per pixel of the input particles. This argument is required if you specify the --mass argument. \n If unspecified (set to 0), the convergence plot is generated using either the project apix, or if not an apix of 1.", guitype='floatbox', row=2, col=0, rowspan=1, colspan=1, mode="refinement['self.pm().getAPIX()']")
	parser.add_argument("--automask3d", default=None, type=str,help="The 5 parameters of the mask.auto3d processor, applied after 3D reconstruction. \n These parameters are, in order, isosurface threshold, radius, nshells, nshellsgauss, nmaxseed. \n From e2proc3d.py you could achieve the same thing using: \n --process=mask.auto3d:threshold=1.1:radius=30:nshells=5:nshellsgauss=5:nmaxseed=0.", guitype='automask3d', row=6, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--automaskalign",action="store_true",help="This will use the automask to improve 2-D alignments and classification.",default=False, guitype='boolbox', row=7, col=0, rowspan=1, colspan=1,mode="refinement")

	# options associated with e2project3d.py
	parser.add_header(name="projectheader", help='Options below this label are specific to e2project', title="### e2project options ###", row=10, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--sym", dest = "sym", default="c1", help = "Specify symmetry - choices are: c<n>, d<n>, h<n>, tet, oct, icos. For asymmetric reconstruction omit this option or specify c1.", guitype='symbox', row=13, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--projector", dest = "projector", default = "standard",help = "Projector to use", guitype='comboparambox', choicelist='dump_projectors_list()', row=11, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--orientgen", type = str, default='eman:delta=5.0:inc_mirror=0:perturb=1',help = "The orientation generation argument for e2project3d.py", guitype='comboparambox', choicelist='dump_orientgens_list()', row=12, col=0, rowspan=1, colspan=3, mode="refinement")
		
	# options associated with e2simmx.py
	parser.add_header(name="simmxheader", help='Options below this label are specific to e2simmx', title="### e2simmx options ###", row=16, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--simalign",type=str,help="The name of an 'aligner' to use prior to comparing the images", default="rotate_translate_flip", guitype='comboparambox', choicelist='re_filter_list(dump_aligners_list(),\'refine|3d\', 1)', row=19, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--simaligncmp",type=str,help="Name and parameters of the comparator used by the first stage aligner",default="ccc", guitype='comboparambox', choicelist='re_filter_list(dump_cmps_list(),\'tomo\', True)', row=20, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--simralign",type=str,help="The name and parameters of the second stage aligner which refines the results of the first alignment, currently 'refine' or not specified.", default=None, guitype='comboparambox', choicelist='re_filter_list(dump_aligners_list(),\'refine\', 0)', row=21, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--simraligncmp",type=str,help="The name and parameters of the comparator used by the second stage aligner. Default is dot.",default="ccc", guitype='comboparambox', choicelist='re_filter_list(dump_cmps_list(),\'tomo\', True)', row=22, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--simcmp",type=str,help="The name of a comparator to be used in comparing the aligned images", default="frc:zeromask=1:snrweight=1", guitype='comboparambox', choicelist='re_filter_list(dump_cmps_list(),\'tomo\', True)', row=18, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--simmask",type=str,help="A file containing a single 0/1 image to apply as a mask before comparison but after alignment", default=None)
	parser.add_argument("--shrink", dest="shrink", type = int, default=0, help="Optionally shrink the input particles by an integer amount prior to computing similarity scores, for speed purposes. Default=0, no shrinking", guitype='shrinkbox', row=17, col=0, rowspan=1, colspan=1, mode="refinement")
	parser.add_argument("--twostage", dest="twostage", type = int, help="Optionally run a faster 2-stage similarity matrix, ~5-10x faster, generally same accuracy. Value specifies shrink factor for first stage, typ 1-3",default=0, guitype='intbox', row=17, col=1, rowspan=1, colspan=1, mode="refinement")
	parser.add_argument("--prefilt",action="store_true",help="Filter each reference (c) to match the power spectrum of each particle (r) before alignment and comparison",default=False, guitype='boolbox', row=17, col=2, rowspan=1, colspan=1)
	
	# options associated with e2classify.py
	parser.add_argument("--simvec", action="store_true",help="Causes the classification algorithm to use patterns rather than peak values",default=False)
	parser.add_argument("--sep", type=int, help="The number of classes a particle can contribute towards (default is 1)", default=1, guitype='intbox', row=24, col=2, rowspan=1, colspan=1, mode="refinement")
	
	# options associated with e2classaverage.py
	parser.add_header(name="caheader", help='Options below this label are specific to e2classaverage', title="### e2classaverage options ###", row=26, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--classkeep",type=float,help="The fraction of particles to keep in each class, based on the similarity score generated by the --cmp argument.",default=0.8, guitype='floatbox', row=28, col=0, rowspan=1, colspan=2, mode="refinement")
	parser.add_argument("--classkeepsig", default=False, action="store_true", help="Change the keep (\'--keep\') criterion from fraction-based to sigma-based.", guitype='boolbox', row=28, col=2, rowspan=1, colspan=1, mode="refinement")
	parser.add_argument("--classiter", type=int, help="The number of iterations to perform. Default is 1.", default=1, guitype='intbox', row=27, col=0, rowspan=1, colspan=2, mode="refinement")
	parser.add_argument("--classalign",type=str,help="If doing more than one iteration, this is the name and parameters of the 'aligner' used to align particles to the previous class average.", default="rotate_translate_flip", guitype='comboparambox', choicelist='re_filter_list(dump_aligners_list(),\'refine|3d\', 1)', row=32, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--classaligncmp",type=str,help="This is the name and parameters of the comparator used by the first stage aligner  Default is dot.",default="ccc", guitype='comboparambox', choicelist='re_filter_list(dump_cmps_list(),\'tomo\', True)', row=33, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--classralign",type=str,help="The second stage aligner which refines the results of the first alignment in class averaging. Either specify 'refine' or omit the option.", default=None, guitype='comboparambox', choicelist='re_filter_list(dump_aligners_list(),\'refine\', 0)', row=34, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--classraligncmp",type=str,help="The comparator used by the second stage aligner in class averageing. Default is ccc.",default="ccc", guitype='comboparambox', choicelist='re_filter_list(dump_cmps_list(),\'tomo\', True)', row=35, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--classaverager",type=str,help="The averager used to generate the class averages. Default is \'mean\'.",default="mean", guitype='combobox', choicelist='dump_averagers_list()', row=30, col=0, rowspan=1, colspan=2, mode="refinement")
	parser.add_argument("--classcmp",type=str,help="The name and parameters of the comparator used to generate similarity scores, when class averaging. Default is ccc", default="frc:snrweight=1", guitype='comboparambox', choicelist='re_filter_list(dump_cmps_list(),\'tomo\', True)', row=31, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--classnormproc",type=str,default="normalize.edgemean",help="Normalization applied during class averaging", guitype='combobox', choicelist='re_filter_list(dump_processors_list(),\'normalize\')', row=29, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--classrefsf",default=False, action="store_true", help="Use the setsfref option in class averaging to produce better filtered averages.", guitype='boolbox', row=30, col=2, rowspan=1, colspan=1, mode="refinement")
	parser.add_argument("--classautomask",default=False, action="store_true", help="This will apply an automask to the class-average during iterative alignment for better accuracy. The final class averages are unmasked.")
	
	
	#options associated with e2make3d.py
	parser.add_header(name="make3dheader", help='Options below this label are specific to e2make3d', title="### e2make3d options ###", row=38, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--pad", type=int, dest="pad", help="To reduce Fourier artifacts, the model is typically padded by ~25 percent - only applies to Fourier reconstruction", default=0, guitype='intbox', row=40, col=2, rowspan=1, colspan=1, mode="refinement")
	parser.add_argument("--recon", dest="recon", default="fourier", help="Reconstructor to use. See 'e2help.py reconstructors -v' for more information", guitype='combobox', choicelist='dump_reconstructors_list()', row=39, col=0, rowspan=1, colspan=2, mode="refinement")
	parser.add_argument("--m3dkeep", type=float, help="The percentage of slices to keep in e2make3d.py", default=0.8, guitype='floatbox', row=41, col=0, rowspan=1, colspan=1, mode="refinement")
	parser.add_argument("--m3dkeepsig", default=False, action="store_true", help="The standard deviation alternative to the --m3dkeep argument", guitype='boolbox', row=41, col=1, rowspan=1, colspan=1, mode="refinement")
	parser.add_argument("--m3dsetsf", default=False, action="store_true", help="This will filter the final map to match the precomputed structure factor", guitype='boolbox', row=41, col=2, rowspan=1, colspan=1, mode="refinement")
	parser.add_argument("--m3dsffile", default=None, type=str, help="If specified, will use the structure factor from specified file rather than project default")
	parser.add_argument("--m3diter", type=int, default=2, help="The number of times the 3D reconstruction should be iterated", guitype='intbox', row=39, col=2, rowspan=1, colspan=1, mode="refinement")
	parser.add_argument("--m3dpreprocess", type=str, default="normalize.edgemean", help="Normalization processor applied before 3D reconstruction", guitype='combobox', choicelist='re_filter_list(dump_processors_list(),\'normalize\')', row=40, col=0, rowspan=1, colspan=2, mode="refinement")
	parser.add_argument("--m3dpostprocess", type=str, default=None, help="Post processor to be applied to the 3D volume once the reconstruction is completed", guitype='comboparambox', choicelist='re_filter_list(dump_processors_list(),\'filter.lowpass|filter.highpass\')', row=42, col=0, rowspan=1, colspan=3, mode="refinement")
	parser.add_argument("--m3dpostprocess2", type=str, default=None, help="A second post processor to be applied to the 3D volume once the reconstruction is completed")
	
	#lowmem!
	parser.add_argument("--lowmem", default=False, action="store_true",help="Make limited use of memory when possible - useful on lower end machines", guitype='boolbox', row=3, col=2, rowspan=1, colspan=1, mode="refinement")
	parser.add_argument("--parallel","-P",type=str,help="Run in parallel, specify type:<option>=<value>:<option>:<value> EX thread:4",default=None, guitype='strbox', row=3, col=0, rowspan=1, colspan=2, mode="refinement")
	parser.add_argument("--ppid", type=int, help="Set the PID of the parent process, used for cross platform PPID",default=-1)
	
	(options, args) = parser.parse_args()
	error = False
	if check(options,True) == True : 
		error = True
	if check_projection_args(options) == True : 
		error = True
	if check_simmx_args(options,True) == True :
		error = True
	if check_classify_args(options,True) == True :
		error = True
	if check_classaverage_args(options,True) == True :
		error = True
#	if check_make3d_args(options,True) == True:
#		error = True
	
	logid=E2init(sys.argv,options.ppid)
	
	
	if options.classrefsf : options.classrefsf=" --setsfref"
	else : options.classrefsf=" "
	
	if options.classautomask : options.classautomask=" --automask"
	else: options.classautomask=" "
	
	if error:
		print "Error encountered while checking command line, bailing"
		exit_refine(1,logid)
	
	if (options.check):
		exit_refine(0,logid)
	
	if options.path == None:
		options.path = numbered_path("refine",True)

	# store the inputs arguments forever in the refinement directory
	db = db_open_dict("bdb:"+options.path+"#register")
	db["cmd_dict"] = options.__dict__
		
	# this is the main refinement loop
	
	progress = 0.0
	total_procs = 5*(options.iter-options.startiter)
	
	# no longer want this behavior
#	if options.automask3d: automask_parms = parsemodopt(options.automask3d) # this is just so we only ever have to do it
	
	apix = get_apix_used(options)
	
	for i in range(options.startiter,options.iter) :
		
		number_options_file(i,"projections",options,"projfile")
		if ( launch_childprocess(get_projection_cmd(options)) != 0 ):
			print "Failed to execute %s" %get_projection_cmd(options)
			exit_refine(1,logid)
		progress += 1.0
		E2progress(logid,progress/total_procs)

		if options.automaskalign :
			# This generates projections of the automask from the previous iteration
			# TODO : consider whether these should be true projections, or maxval projections
			number_options_file(i,"proj_masks",options,"projmask")
			if ( launch_childprocess(get_maskproj_cmd(options)) != 0 ):
				print "Failed to execute %s" %get_maskproj_cmd(options)
				exit_refine(1,logid)

			# Combine the mask projections to produce a single mask which is good for all projections
			# Using the individual masks for different projections in the similarity matrix might produce a bias towards projections with smaller masks
			number_options_file(i,"proj_simmask",options,"projmaskavg")
			cmd="e2proc2d.py %s %s --inplace --average --averager=minmax:max=1"%(options.projmask,options.projmaskavg)
			if ( launch_childprocess(cmd) != 0 ):
				print "Failed to execute %s" %cmd
				exit_refine(1,logid)

			# Clamp the single mask to 0-1
			# TODO : consider how to treat this depending on whether the individual mask projections are maxval or not
			cmd="e2proc2d.py %s %s --inplace --process=threshold.clampminmax:maxval=1:minval=0"%(options.projmaskavg,options.projmaskavg)
			if ( launch_childprocess(cmd) != 0 ):
				print "Failed to execute %s" %cmd
				exit_refine(1,logid)
			

		number_options_file(i,"simmx",options,"simmxfile")
		if options.twostage>0 :
			number_options_file(i,"proj_simmx",options,"simmxprojfile")
			number_options_file(i,"proj_stg1",options,"projstg1file")
			number_options_file(i,"simmx_stg1",options,"simmxstg1file")
			
		if ( launch_childprocess(get_simmx_cmd(options)) != 0 ):
			print "Failed to execute %s" %get_simmx_cmd(options)
			exit_refine(1,logid)
		progress += 1.0
		E2progress(logid,progress/total_procs)
		
		number_options_file(i,"classify",options,"classifyfile")
		if ( launch_childprocess(get_classify_cmd(options)) != 0 ):
			print "Failed to execute %s" %get_classify_cmd(options)
			exit_refine(1,logid)
		progress += 1.0
		E2progress(logid,progress/total_procs)
			
		number_options_file(i,"classes",options,"cafile")
		number_options_file(i,"cls_result",options,"resultfile")
		if ( launch_childprocess(get_classaverage_cmd(options)) != 0 ):
			print "Failed to execute %s" %get_classaverage_cmd(options)
			exit_refine(1,logid)
		progress += 1.0
		E2progress(logid,progress/total_procs)
			
		try : previous_model = options.filtered_model
		except : 
			if options.startiter>0:
				number_options_file(options.startiter-1,"threed_filt",options,"model")
				print "startiter: overriding model with ",options.model
			previous_model = options.model
		number_options_file(i,"threed",options,"model")
		new_model = options.model
		if ( launch_childprocess(get_make3d_cmd(options)) != 0 ):
			print "Failed to execute %s" %get_make3d_cmd(options)
			exit_refine(1,logid)
		progress += 1.0
		
		# User may specify a second postprocessor
		if options.m3dpostprocess2!=None :
			com="e2proc3d.py %s %s --process=%s"%(new_model,new_model,options.m3dpostprocess2)
			if ( launch_childprocess(com) != 0 ):
				print "Failed to execute %s" %com
				exit_refine(1,logid)
		
		
		a = EMData(previous_model,0)
		b = EMData(new_model,0)
		
		# redid this using e2proc3d commands rather than internally for better logging. Performance will take a very slight hit this way
		if options.mass:
			# if options.mass is not none, the check function has already ascertained that it's postivie non zero, and that the 
			# apix argument has been specified.
			number_options_file(i,"threed_filt",options,"filtered_model")
			b.write_image(options.filtered_model,0)
			com="e2proc3d.py %s %s --process=normalize.bymass:apix=%1.3f:mass=%1.1f"%(options.filtered_model,options.filtered_model,options.apix,options.mass)
			if ( launch_childprocess(com) != 0 ):
				print "Failed to execute %s" %com
				exit_refine(1,logid)
			

			if options.automask3d:
				number_options_file(i,"threed_mask",options,"mask_image")
				com="e2proc3d.py %s %s --process=%s"%(options.filtered_model,options.mask_image,options.automask3d)
				if ( launch_childprocess(com) != 0 ):
					print "Failed to execute %s" %com
					exit_refine(1,logid)
				
				com="e2proc3d.py %s %s --multfile=%s"%(options.filtered_model,options.filtered_model,options.mask_image)
				if ( launch_childprocess(com) != 0 ):
					print "Failed to execute %s" %com
					exit_refine(1,logid)
			
			b=EMData(options.filtered_model,0)		# reload result for FSC
		#fsc = a.calc_fourier_shell_correlation(b)
		#third = len(fsc)/3
		#xaxis = fsc[0:third]
		#plot = fsc[third:2*third]
		#error = fsc[2*third:]
		
		if i == 0:
			s = "init_00"
		else:
			s1 = str(i-1)
			s2 = str(i)
			if len(s1) == 1: s1 = "0" + s1
			if len(s2) == 1: s2 = "0" + s2
			s = s1 + "_" + s2
		
		db_compute_fsc(a, b, apix, options.path, s+"_fsc") 
		#convergence_db_name = "bdb:"+options.path+"#convergence.results"
		#db = db_open_dict(convergence_db_name)
		
		#tmpaxis = [x/apix for x in xaxis]
		#db[s+"_fsc"] = [tmpaxis,plot]
		#db["error_"+s+"_fsc"] = [xaxis,error] #we're not plotting the errors
		#db_close_dict(convergence_db_name)
		
		E2progress(logid,progress/total_procs)
	E2end(logid)

def get_apix_used(options):
	'''
	Just an encapsulation apix retrieval
	Basically, if the apix argument is in the options, that's what you get
	Else the project db is checked for the global.apix parameter
	Else you just get 1
	'''
	apix = 1.0
	if options.apix: apix = options.apix # check function checks whether or not this value is positive, non zero
	else:
		img=EMData(options.input,0)
		try: apix=img["ctf"].apix
		except: apix=img["apix_x"]

	return apix
		
def number_options_file(i,file,options,attr):
	name = "bdb:"+options.path+"#" + file+"_"
	if i < 10:
		name += "0"
	
	name += str(i)
	setattr(options,attr,name)
	
def exit_refine(n,logid):
	E2end(logid)
	exit(n)

def get_make3d_cmd(options,check=False,nofilecheck=False):
	e2make3dcmd = "e2make3d.py --input=%s --iter=%d -f --keepabs " %(options.cafile,options.m3diter)

	try : 
		if "breaksym=1" in options.orientgen : e2make3dcmd+=" --sym=c1"
		else: raise Exception
	except : 
		e2make3dcmd+=" --sym=%s"%options.sym
	
	e2make3dcmd += " --recon=%s --output=%s" %(options.recon,options.model)

	if str(options.m3dpreprocess) != "None":
		e2make3dcmd += " --preprocess=%s" %options.m3dpreprocess
		
	if str(options.m3dpostprocess) != "None":
		e2make3dcmd += " --postprocess=%s" %options.m3dpostprocess

	
	if (options.m3dkeep):
		e2make3dcmd += " --keep=%f" %options.m3dkeep
		if (options.m3dkeepsig): e2make3dcmd += " --keepsig"
	
	if options.m3dsffile!=None: e2make3dcmd += " --setsf=%s"%options.m3dsffile
	elif options.m3dsetsf : e2make3dcmd += " --setsf=auto"
	
	if (options.lowmem): e2make3dcmd += " --lowmem"

	if (options.pad != 0):
		e2make3dcmd += " --pad=%d" %options.pad
		
	if (options.verbose):
		e2make3dcmd += " --verbose=" + str(options.verbose - 1)
	
	if ( check ):
		e2make3dcmd += " --check"	
			
	if ( nofilecheck ):
		e2make3dcmd += " --nofilecheck"
	
	return e2make3dcmd

def check_make3d_args(options, nofilecheck=False):
	
	cmd = get_make3d_cmd(options,True,nofilecheck)
	print ""
	print "#### Test executing make3d command: %s" %cmd
	return ( launch_childprocess(cmd) != 0)

def get_classaverage_cmd(options,check=False,nofilecheck=False):
	
	e2cacmd = "e2classaverage.py --input=%s --classmx=%s --storebad --output=%s" %(options.input,options.classifyfile,options.cafile)
	
	e2cacmd += " --ref=%s --iter=%d -f --resultmx=%s --normproc=%s --averager=%s %s %s" %(options.projfile,options.classiter,options.resultfile,options.classnormproc,options.classaverager,options.classrefsf,options.classautomask)
	
	e2cacmd += " --idxcache --dbpath=%s" %options.path
	
	if (options.classkeep):
		e2cacmd += " --keep=%f" %options.classkeep
		
	if (options.classkeepsig):
		e2cacmd += " --keepsig"
	
	e2cacmd += " --cmp=%s --align=%s --aligncmp=%s" %(options.classcmp,options.classalign,options.classaligncmp)

	if (options.classralign != None):
		e2cacmd += " --ralign=%s --raligncmp=%s" %(options.classralign,options.classraligncmp)
	
	if options.usefilt != None:
		e2cacmd += " --usefilt=%s" %options.usefilt
	
	if (options.verbose):
		e2cacmd += " --verbose=" + str(options.verbose - 1)
	
	if options.prefilt:
		e2cacmd += " --prefilt"
	
	if options.parallel: e2cacmd += " --parallel=%s" %options.parallel

		
	#lowmem becamoe the only supportable behaviour as of May 5th 2009
#	if (options.lowmem): e2cacmd += " --lowmem"
	
	if ( check ):
		e2cacmd += " --check"	
			
	if ( nofilecheck ):
		e2cacmd += " --nofilecheck"
	
	return e2cacmd

def check_classaverage_args(options, nofilecheck=False):
	if not hasattr(options,"cafile"): setattr(options,"cafile","dummy")
	if not hasattr(options,"resultfile"): setattr(options,"resultfile","dummy")
	cmd = get_classaverage_cmd(options,True,nofilecheck)
	print ""
	print "#### Test executing classaverage command: %s" %cmd
	return ( launch_childprocess(cmd) != 0)

def get_classify_cmd(options,check=False,nofilecheck=False):
	e2classifycmd = "e2classify.py %s %s --sep=%d -f" %(options.simmxfile,options.classifyfile,options.sep)
	
	if (options.simvec):
		e2classifycmd += " --simvec"

	if (options.verbose):
		e2classifycmd += " --verbose=" + str(options.verbose - 1)
	
	if ( check ):
		e2classifycmd += " --check"	
			
	if ( nofilecheck ):
		e2classifycmd += " --nofilecheck"
	
	return e2classifycmd

def check_classify_args(options, nofilecheck=False):
	if not hasattr(options,"classifyfile"): setattr(options,"classifyfile","dummy")
	cmd = get_classify_cmd(options,True,nofilecheck)
	print ""
	print "#### Test executing classify command: %s" %cmd
	return ( launch_childprocess(cmd) != 0)

def get_simmx_cmd(options,check=False,nofilecheck=False):
	
	if options.usefilt != None:
		image = options.usefilt
	else:
		image = options.input
	
	if options.twostage>0 : 
		try : e2simmxcmd = "e2simmx2stage.py %s %s %s %s %s %s -f --saveali --cmp=%s --align=%s --aligncmp=%s --shrinks1=%d"  %(options.projfile, image,options.simmxfile,options.simmxprojfile,options.projstg1file,options.simmxstg1file,options.simcmp,options.simalign,options.simaligncmp,options.twostage)
		except: print options
	else :
		e2simmxcmd = "e2simmx.py %s %s %s -f --saveali --cmp=%s --align=%s --aligncmp=%s"  %(options.projfile, image,options.simmxfile,options.simcmp,options.simalign,options.simaligncmp)

	if options.prefilt : e2simmxcmd+=" --prefilt"

	# we skip this for check with the try
	try:
		if options.automaskalign : e2simmxcmd+=" --colmasks=%s --mask=%s"%(options.projmask,options.projmaskavg)
		elif options.simmask!=None : e2simmxcmd += " --mask=%s"%options.simmask
	except: pass
	
	if ( options.simralign != None ):
		e2simmxcmd += " --ralign=%s --raligncmp=%s" %(options.simralign,options.simraligncmp)
	
	if (options.verbose):
		e2simmxcmd += " --verbose=" + str(options.verbose - 1)
	
	if options.parallel: e2simmxcmd += " --parallel=%s" %options.parallel
	
	#if (options.lowmem): e2simmxcmd += " --lowmem"	
	
	if (options.shrink):
		e2simmxcmd += " --shrink="+str(options.shrink)
		
	if ( check ):
		e2simmxcmd += " --check"	
			
	if ( nofilecheck ):
		e2simmxcmd += " --nofilecheck"
		
	
	return e2simmxcmd

def check_simmx_args(options, nofilecheck=False):
	if not hasattr(options,"simmxfile"): setattr(options,"simmxfile","dummy")
	number_options_file(0,"proj_simmx",options,"simmxprojfile")
	number_options_file(0,"proj_stg1",options,"projstg1file")
	number_options_file(0,"simmx_stg1",options,"simmxstg1file")
	cmd = get_simmx_cmd(options,True,nofilecheck)
	print ""
	print "#### Test executing simmx command: %s" %cmd
	return (False)
	return ( launch_childprocess(cmd) != 0)

def get_maskproj_cmd(options,check=False):
	"""This generates projections of the last automask"""
	
	if hasattr(options,"filtered_model"): # sometimes there is a filtered model, i.e. if mass or automask3d is specified
		model = options.filtered_model.replace("filt","mask")
	else:
		print "WARNING: automaskalign option specified, but automask not found. Generating non-zero mask from %s. This is normal in the first iteration."%options.model
		if options.model[:4].lower()=="bdb:" : model=options.model+"_mask"
		else: model=options.model.rsplit(".",1)[0]+"_mask."+options.model.rsplit(".",1)[1]
		launch_childprocess("e2proc3d.py %s %s --process=threshold.notzero"%(options.model,model))
		
	e2projcmd = "e2project3d.py %s -f --sym=%s --projector=%s --outfile=%s --orientgen=%s --postprocess=normalize.circlemean" %(model,options.sym,options.projector,options.projmask,options.orientgen)

	if options.parallel: e2projcmd += " --parallel=%s" %options.parallel
	
	if ( check ):
		e2projcmd += " --check"	
		
	if (options.verbose):
		e2projcmd += " --verbose=" + str(options.verbose - 1)
	
	return e2projcmd

def get_projection_cmd(options,check=False):
	
	model = options.model
	if hasattr(options,"filtered_model"): # sometimes there is a filtered model, i.e. if mass or automask3d is specified
		model = options.filtered_model
		
	e2projcmd = "e2project3d.py %s -f --sym=%s --projector=%s --outfile=%s --orientgen=%s --postprocess=normalize.circlemean" %(model,options.sym,options.projector,options.projfile,options.orientgen)

	if options.parallel: e2projcmd += " --parallel=%s" %options.parallel
	
	if ( check ):
		e2projcmd += " --check"	
		
	if (options.verbose):
		e2projcmd += " --verbose=" + str(options.verbose - 1)
	
	return e2projcmd
	
def check_projection_args(options):
	if not hasattr(options,"projfile"): setattr(options,"projfile","dummy")
	cmd = get_projection_cmd(options,True)
	print ""
	print "#### Test executing projection command: %s" %cmd
	return ( launch_childprocess(cmd) != 0 )

def check(options,verbose=0):
	if (options.verbose>0):
		print ""
		print "#### Testing directory contents and command line arguments for e2refine.py"
	
	error = False
	if options.input == None or not file_exists(options.input):
		print "Error: failed to find input file %s" %options.input
		error = True
	
	if options.usefilt != None:
		if not file_exists(options.usefilt):
			print "Error: failed to find usefilt file %s" %options.usefilt
			error = True
		n1 = EMUtil.get_image_count(options.usefilt)
		n2 = EMUtil.get_image_count(options.input)
		if n1 != n2:
			print "Error, the number of images in the starting particle set:",n2,"does not match the number in the usefilt set:",n1
			error = True
		read_header_only=True
		img1 = EMData()
		img1.read_image(options.input,0,read_header_only)
		img2 = EMData()
		img2.read_image(options.usefilt,0,read_header_only)
		img3 = EMData(options.model,0,True)
		
		nx1 = img1.get_attr("nx") 
		nx2 = img2.get_attr("nx") 
		
		ny1 = img1.get_attr("ny") 
		ny2 = img2.get_attr("ny") 
		
		if nx1 != nx2 or ny1 != ny2:
			error = True
			if options.verbose>0: print "Error, the dimensions of particle data (%i x %i) and the usefilt data (%i x %i) do not match" %(nx1,ny1,nx2,ny2)
	
	if not file_exists(options.model):
		print "Error: 3D image %s does not exist" %options.model
		error = True
	
	if options.mass:
		if options.mass <=0:
			print "If specified, the mass argument must be greater than 0"
			error = True
		if not options.apix:
			print "If you specify the mass argument, you must also specify the apix argument"
			error = True
			
	if options.apix:
		if options.apix <=0:
			print "If specified, the apix argument must be greater than 0"
			error = True
		
	if options.automask3d:
		vals = options.automask3d.split(",")
		mapping = ["threshold","radius","nshells","nshellsgauss","nmaxseed"]
		if len(vals) != 5:
			print "If specified, the automask3d options must provide 5 parameters (threshold,radius,nshells,nshellsgauss,nmaxseed), for example --automask3d=1.7,0,5,5,3"
			error = True
		else:
			# Here I turn options.automask3d into what we would have expected if the user was supplying the whole processor argument,
			# e.g. --automask3d=mask.auto3d:threshold=1.7:radi.... etc. I also add the return_mask=1 parameters - this could be misleading for future
			# programmers, who will potentially wander where it came from
			s = "mask.auto3d"
			for i,p in enumerate(mapping):
				s += ":"+p+"="+vals[i]
			s+= ":return_mask=1"
			options.automask3d = s
			
			if not check_eman2_type(options.automask3d,Processors,"Processors"): error = True
	
	if not options.iter:
		print "Error: you must specify the --iter argument"
		error = True
		
	if options.path != None:
		if not os.path.exists(options.path):
			print "Error: the path %s does not exist" %options.path
			error = True
	else:
		options.path = numbered_path("refine",True)
		
	if ( file_exists(options.model) and options.input != None and file_exists(options.input)):
		(xsize, ysize ) = gimme_image_dimensions2D(options.input);
		(xsize3d,ysize3d,zsize3d) = gimme_image_dimensions3D(options.model)
		
		if (options.verbose>0):
			print "%s contains %d images of dimensions %dx%d" %(options.input,EMUtil.get_image_count(options.input),xsize,ysize)
			print "%s has dimensions %dx%dx%d" %(options.model,xsize3d,ysize3d,zsize3d)
		
		if ( xsize != ysize ):
			if ( ysize == zsize3d and xsize == ysize3d and ysize3D == xsize3d ):
				print "Error: it appears as though you are trying to do helical reconstruction. This is not supported"
				error = True
			else:
				print "Error: images dimensions (%d x %d) of %s are not identical. This mode of operation is not supported" %(xsize,ysize,options.input)
				error = True
		
		if ( xsize3d != ysize3d or ysize3d != zsize3d ):
			print "Error: image dimensions (%dx%dx%d) of %s are not equal" %(xsize3d,ysize3d,zsize3d,options.model)
			error = True
			
		if ( xsize3d != xsize ) :
			print "WARNING: the dimensions of the particles (%d) do not match the dimensions of the starting model (%d). I will attempt to adjust the model appropriately."%(xsize,xsize3d)
			img1 = EMData(options.input,0,True)
			img3 = EMData(options.model,0,True)
			try:
				scale=img3["apix_x"]/img1["apix_x"]
			except:
				print "A/pix unknown, assuming scale same as relative box size"
				scale=float(xsize)/xsize3d
			if scale>1 : cmd="e2proc3d.py %s bdb:%s#initial_model --clip=%d,%d,%d --scale=%1.5f"%(options.model,options.path,xsize,xsize,xsize,scale)
			else :       cmd="e2proc3d.py %s bdb:%s#initial_model --scale=%1.5f --clip=%d,%d,%d"%(options.model,options.path,scale,xsize,xsize,xsize)
			print cmd
			launch_childprocess(cmd)
			options.model="bdb:%s#initial_model"%options.path
			
	if hasattr(options,"parallel") and options.parallel != None:
  		if len(options.parallel) < 2:
  			print "The parallel option %s does not make sense" %options.parallel
  			error = True
 
	if (options.verbose>0):
		if (error):
			s = "FAILED"
		else:
			s = "PASSED"
			
		print "e2refine.py test.... %s" %s

	return error == True
	
if __name__ == "__main__":
    main()
