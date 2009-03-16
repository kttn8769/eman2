/**
 * $Id$
 */
 
/*
 * Author: Steven Ludtke, 04/10/2003 (sludtke@bcm.edu)
 * Copyright (c) 2000-2006 Baylor College of Medicine
 * 
 * This software is issued under a joint BSD/GNU license. You may use the
 * source code in this file under either license. However, note that the
 * complete EMAN2 and SPARX software packages have some GPL dependencies,
 * so you are responsible for compliance with the licenses of these packages
 * if you opt to use BSD licensing. The warranty disclaimer below holds
 * in either instance.
 * 
 * This complete copyright notice must be included in any revised version of the
 * source code. Additional authorship citations may be added, but existing
 * author citations must be preserved.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 * */
 
#include "emdata.h"
#include "emfft.h"

#include <cstring>
#include <cstdio>

#ifdef EMAN2_USING_OPENGL
#include "GL/gl.h"
#endif //EMAN2_USING_OPENGL


using namespace EMAN;


#ifdef EMAN2_USING_CUDA

#include "cuda/cuda_emfft.h"

EMData *EMData::do_fft_cuda() const
{
	ENTERFUNC;

	if ( is_complex() ) {
		LOGERR("real image expected. Input image is complex image.");
		throw ImageFormatException("real image expected. Input image is complex image.");
	}
// 	int nxreal = nx;
	int offset;
	int ndim = get_ndim();
	EMData* dat = new EMData();
	
	offset = 2 - nx%2;
	dat->set_size_cuda(nx+offset,ny, nz);
	float *d = dat->get_cuda_data();
	if ( ndim == 1 ) {
		cuda_dd_fft_real_to_complex_nd(get_cuda_data(), d, nx, 1,1);
	} else if (ndim == 2) {
		cuda_dd_fft_real_to_complex_nd(get_cuda_data(), d, ny, nx, 1);
	} else if (ndim == 3) {
		cuda_dd_fft_real_to_complex_nd(get_cuda_data(), d, nz, ny, nx);
	} else throw ImageDimensionException("No cuda FFT support of images with dimensions exceeding 3");
	
	if (offset == 1) dat->set_fftodd(true);
	else             dat->set_fftodd(false);

	dat->set_fftpad(true);
	dat->set_complex(true);
	if(dat->get_ysize()==1 && dat->get_zsize()==1) dat->set_complex_x(true);
	dat->set_ri(true);
	dat->gpu_update();

	EXITFUNC;
	return dat;
}

EMData *EMData::do_ift_cuda(bool preserve_input) const
{
	ENTERFUNC;

	if (!is_complex()) {
		LOGERR("complex image expected. Input image is real image.");
		throw ImageFormatException("complex image expected. Input image is real image.");
	}

	if (!is_ri()) {
		throw ImageFormatException("complex ri expected. Got amplitude/phase.");
	}
	
	int offset = is_fftodd() ? 1 : 2;
	EMData* dat = new EMData();
	int ndim = get_ndim();
	dat->set_size_cuda(nx-offset, ny, nz);
	float *d = dat->get_cuda_data();
	float *this_d;
	EMData* tmp = 0;
	if (preserve_input){
		tmp = new EMData(*this);
		this_d = tmp->get_cuda_data();
	} else {
		this_d = get_cuda_data();
	}
	if ( ndim == 1 ) {
		cuda_dd_fft_complex_to_real_nd(this_d,d, nx-offset,1,1);
	} else if (ndim == 2) {
		cuda_dd_fft_complex_to_real_nd(this_d,d, ny,nx-offset,1);
	} else if (ndim == 3) {
		cuda_dd_fft_complex_to_real_nd(this_d,d, nz,ny,nx-offset);
	} else throw ImageDimensionException("No cuda FFT support of images with dimensions exceeding 3");
	
	if (tmp != 0) delete tmp;
	
	// SCALE the inverse FFT
	float scale = 1.0f/static_cast<float>((dat->get_size()));
	dat->mult_cuda(scale);
	
	dat->set_fftpad(false);
	dat->set_complex(false);
	if(dat->get_ysize()==1 && dat->get_zsize()==1)  dat->set_complex_x(false);
	dat->set_ri(false);
	dat->gpu_update();
	
	EXITFUNC;
	return dat;
}

#endif //EMAN2_USING_CUDA

EMData *EMData::do_fft() const
{
	ENTERFUNC;

	if ( is_complex() ) {
		LOGERR("real image expected. Input image is complex image.");
		throw ImageFormatException("real image expected. Input image is complex image.");
	}

	int nxreal = nx;
	int offset = 2 - nx%2;
	int nx2 = nx + offset;
	EMData* dat = copy_head();
	dat->set_size(nx2, ny, nz);
	//dat->to_zero();  // do not need it, real_to_complex will do it right anyway
	if (offset == 1) dat->set_fftodd(true);
	else             dat->set_fftodd(false);

	float *d = dat->get_data();
	//std::cout<<" do_fft "<<rdata[5]<<"  "<<d[5]<<std::endl;
	EMfft::real_to_complex_nd(get_data(), d, nxreal, ny, nz);

	dat->update();
	dat->set_fftpad(true);
	dat->set_complex(true);
	if(dat->get_ysize()==1 && dat->get_zsize()==1) dat->set_complex_x(true);
	dat->set_ri(true);

	EXITFUNC;
	return dat;
}

EMData *EMData::do_fft_inplace()
{
	ENTERFUNC;

	if ( is_complex() ) {
		LOGERR("real image expected. Input image is complex image.");
		throw ImageFormatException("real image expected. Input image is complex image.");
	}
	
	size_t offset;
	int nxreal;
	get_data(); // Required call if GPU caching is being used. Otherwise harmless
	if (!is_fftpadded()) {
		// need to extend the matrix along x
		// meaning nx is the un-fftpadded size
		nxreal = nx;
		offset = 2 - nx%2;
		if (1 == offset) set_fftodd(true);
		else             set_fftodd(false);
		int nxnew = nx + offset;
		set_size(nxnew, ny, nz);

		for (int iz = nz-1; iz >= 0; iz--) {
			for (int iy = ny-1; iy >= 0; iy--) {
				for (int ix = nxreal-1; ix >= 0; ix--) {
					size_t oldxpos = ix + (iy + iz*ny)*nxreal;
					size_t newxpos = ix + (iy + iz*ny)*nxnew;
					(*this)(newxpos) = (*this)(oldxpos);
				}
			}
		}
		set_fftpad(true);
	} else {
		offset = is_fftodd() ? 1 : 2;
		nxreal = nx - offset;
	}
	EMfft::real_to_complex_nd(rdata, rdata, nxreal, ny, nz);

	set_complex(true);
	if(ny==1 && nz==1)  set_complex_x(true);
	set_ri(true);

	update();

	EXITFUNC;
	return this;
}

EMData *EMData::do_ift()
{
	ENTERFUNC;

	if (!is_complex()) {
		LOGERR("complex image expected. Input image is real image.");
		throw ImageFormatException("complex image expected. Input image is real image.");
	}

	if (!is_ri()) {
		LOGWARN("run IFT on AP data, only RI should be used. Converting.");
	}

	get_data(); // Required call if GPU caching is being used. Otherwise harmless
	EMData* dat = copy_head();
	dat->set_size(nx, ny, nz);
	ap2ri();

	float *d = dat->get_data();
	int ndim = get_ndim();
	
	/* Do inplace IFT on a image copy, because the complex to real transform of 
	 * nd will destroy its input array even for out-of-place transforms.
	 */
	memcpy((char *) d, (char *) rdata, nx * ny * nz * sizeof(float));

	int offset = is_fftodd() ? 1 : 2;
	//cout << "Sending offset " << offset << " " << nx-offset << endl;
	if (ndim == 1) {
		EMfft::complex_to_real_nd(d, d, nx - offset, ny, nz);
	} else {
		EMfft::complex_to_real_nd(d, d, nx - offset, ny, nz);

#ifndef CUDA_FFT
		size_t row_size = (nx - offset) * sizeof(float);
		for (int i = 1; i < ny * nz; i++) {
			memmove((char *) &d[i * (nx - offset)], (char *) &d[i * nx], row_size);
		}
#endif
	}

	dat->set_size(nx - offset, ny, nz);	//remove the padding
#if defined	FFTW2 || defined FFTW3	|| defined CUDA_FFT //native fft and ACML already done normalization
	// SCALE the inverse FFT
	float scale = 1.0f / ((nx - offset) * ny * nz);
	dat->mult(scale);
#endif	//FFTW2 || FFTW3
	dat->set_fftpad(false);
	dat->set_complex(false);
	if(dat->get_ysize()==1 && dat->get_zsize()==1)  dat->set_complex_x(false);
	dat->set_ri(false);
	dat->update();
	
	
	EXITFUNC;
	return dat;
}

/*
   FFT in place does not depad, return real x-extended image
   use 
*/
EMData *EMData::do_ift_inplace()
{
	ENTERFUNC;

	if (!is_complex()) {
		LOGERR("complex image expected. Input image is real image.");
		throw ImageFormatException("complex image expected. Input image is real image.");
	}

	if (!is_ri()) {
		LOGWARN("run IFT on AP data, only RI should be used. ");
	}
	ap2ri();

	int offset = is_fftodd() ? 1 : 2;
	float* data = get_data();
	EMfft::complex_to_real_nd(data, data, nx - offset, ny, nz);

#if defined	FFTW2 || defined FFTW3 || defined CUDA_FFT	//native fft and ACML already done normalization
	// SCALE the inverse FFT
	int nxo = nx - offset;
	float scale = 1.0f / (nxo * ny * nz);
	mult(scale);
#endif //FFTW2 || FFTW3

#ifndef CUDA_FFT
	set_fftpad(true);
#else
	set_size(nx - offset, ny, nz);
#endif 
	set_complex(false);
	if(ny==1 && nz==1) set_complex_x(false);
	set_ri(false);
	update();


	EXITFUNC;
	return this;
}
#undef rdata

std::string EMData::render_amp8(int x0, int y0, int ixsize, int iysize,
						 int bpl, float scale, int mingray, int maxgray,
						 float render_min, float render_max,float gamma,int flags)
{
	ENTERFUNC;

	int asrgb;
	int hist=(flags&2)/2;
	int invy=(flags&4)?1:0;

	if (get_ndim() > 2) {
		throw ImageDimensionException("1D/2D only");
	}

	if (is_complex()) {
		ri2ap();
	}

	if (render_max <= render_min) {
		render_max = render_min + 0.01f;
	}

	if (gamma<=0) gamma=1.0;

	// Calculating a full floating point gamma for
	// each pixel in the image slows rendering unacceptably
	// however, applying a gamma-mapping to an 8 bit colorspace
	// has unacceptable coarse accuracy. So, we oversample the 8 bit colorspace
	// as a 12 bit colorspace and apply the gamma mapping to that
	// This should produce good accuracy for gamma values
	// larger than 0.5 (and a high upper limit)
	static int smg0=0,smg1=0;	// while this destroys threadsafety in the rendering process
	static float sgam=0;		// it is necessary for speed when rendering large numbers of small images
	static unsigned char gammamap[4096];
	if (gamma!=1.0 && (smg0!=mingray || smg1!=maxgray || sgam!=gamma)) {
		for (int i=0; i<4096; i++) {
			if (mingray<maxgray) gammamap[i]=(unsigned char)(mingray+(maxgray-mingray+0.999)*pow(((float)i/4096.0f),gamma));
			else gammamap[4095-i]=(unsigned char)(mingray+(maxgray-mingray+0.999)*pow(((float)i/4096.0f),gamma));
		}
	}
	smg0=mingray;	// so we don't recompute the map unless something changes
	smg1=maxgray;
	sgam=gamma;

	if (flags&8) asrgb=4;
	else if (flags&1) asrgb=3;
	else asrgb=1;

	std::string ret=std::string();
//	ret.resize(iysize*bpl);
	ret.assign(iysize*bpl+hist*1024,char(mingray));
	unsigned char *data=(unsigned char *)ret.data();
	unsigned int *histd=(unsigned int *)(data+iysize*bpl);
	if (hist) {
		for (int i=0; i<256; i++) histd[i]=0;
	}

	float rm = render_min;
	float inv_scale = 1.0f / scale;
	int ysize = iysize;
	int xsize = ixsize;

	int ymin = 0;
	if (iysize * inv_scale > ny) {
		ymin = (int) (iysize - ny / inv_scale);
	}

	float gs = (maxgray - mingray) / (render_max - render_min);
	float gs2 = 4095.999f / (render_max - render_min);
//	float gs2 = 1.0 / (render_max - render_min);
	if (render_max < render_min) {
		gs = 0;
		rm = FLT_MAX;
	}

	int dsx = -1;
	int dsy = 0;
	int remx = 0;
	int remy = 0;
	const int scale_n = 100000;

	int addi = 0;
	int addr = 0;
	if (inv_scale == floor(inv_scale)) {
		dsx = (int) inv_scale;
		dsy = (int) (inv_scale * nx);
	}
	else {
		addi = (int) floor(inv_scale);
		addr = (int) (scale_n * (inv_scale - floor(inv_scale)));
	}

	int xmin = 0;
	if (x0 < 0) {
		xmin = (int) (-x0 / inv_scale);
		xsize -= (int) floor(x0 / inv_scale);
		x0 = 0;
	}

	if ((xsize - xmin) * inv_scale > (nx - x0)) {
		xsize = (int) ((nx - x0) / inv_scale + xmin);
	}
	int ymax = ysize - 1;
	if (y0 < 0) {
		ymax = (int) (ysize + y0 / inv_scale - 1);
		ymin += (int) floor(y0 / inv_scale);
		y0 = 0;
	}

	if (xmin < 0) xmin = 0;
	if (ymin < 0) ymin = 0;
	if (xsize > ixsize) xsize = ixsize;
	if (ymax > iysize) ymax = iysize;

	int lmax = nx * ny - 1;

	int mid=nx*ny/2;
	float * image_data = get_data();
	if (is_complex()) {
		if (dsx != -1) {
			int l = y0 * nx;
			for (int j = ymax; j >= ymin; j--) {
				int ll = x0;
				for (int i = xmin; i < xsize; i++) {
					if (l + ll > lmax || ll >= nx - 2) break;

					int k = 0;
					unsigned char p;
					if (ll >= nx / 2) {
						if (l >= (ny - inv_scale) * nx) k = 2 * (ll - nx / 2) + 2;
						else k = 2 * (ll - nx / 2) + l + 2 + nx;
					}
					else k = nx * ny - (l + 2 * ll) - 2;
					if (k>=mid) k-=mid;		// These 2 lines handle the Fourier origin being in the corner, not the middle
					else k+=mid; 
					float t = image_data[k];
					if (t <= rm)  p = mingray;
					else if (t >= render_max) p = maxgray;
					else if (gamma!=1.0) {
						k=(int)(gs2 * (t-render_min));		// map float value to 0-4096 range
						p = gammamap[k];					// apply gamma using precomputed gamma map
//						p = (unsigned char) (maxgray-mingray)*pow((gs2 * (t - render_min)),gamma);
//						p += mingray;
//						k = static_cast<int>( (maxgray-mingray)*pow((gs2 * (t - render_min)),gamma) );
//						k += mingray;
					}
					else {
						p = (unsigned char) (gs * (t - render_min));
						p += mingray;
					}
					data[i * asrgb + j * bpl] = p;
					if (hist) histd[p]++;
					ll += dsx;
				}
				l += dsy;
			}
		}
		else {
			remy = 10;
			int l = y0 * nx;
			for (int j = ymax; j >= ymin; j--) {
				int br = l;
				remx = 10;
				int ll = x0;
				for (int i = xmin; i < xsize - 1; i++) {
					if (l + ll > lmax || ll >= nx - 2) {
						break;
					}
					int k = 0;
					unsigned char p;
					if (ll >= nx / 2) {
						if (l >= (ny * nx - nx)) k = 2 * (ll - nx / 2) + 2;
						else k = 2 * (ll - nx / 2) + l + 2 + nx;
					}
					else k = nx * ny - (l + 2 * ll) - 2;
					if (k>=mid) k-=mid;		// These 2 lines handle the Fourier origin being in the corner, not the middle
					else k+=mid; 

					float t = image_data[k];
					if (t <= rm)
						p = mingray;
					else if (t >= render_max) {
						p = maxgray;
					}
					else if (gamma!=1.0) {
						k=(int)(gs2 * (t-render_min));		// map float value to 0-4096 range
						p = gammamap[k];					// apply gamma using precomputed gamma map
//						p = (unsigned char) (maxgray-mingray)*pow((gs2 * (t - render_min)),gamma);
//						p += mingray;
//						k = static_cast<int>( (maxgray-mingray)*pow((gs2 * (t - render_min)),gamma) );
//						k += mingray;
					}
					else {
						p = (unsigned char) (gs * (t - render_min));
						p += mingray;
					}
					data[i * asrgb + j * bpl] = p;
					if (hist) histd[p]++;
					ll += addi;
					remx += addr;
					if (remx > scale_n) {
						remx -= scale_n;
						ll++;
					}
				}
				l = br + addi * nx;
				remy += addr;
				if (remy > scale_n) {
					remy -= scale_n;
					l += nx;
				}
			}
		}
	}
	else {
		if (dsx != -1) {
			int l = x0 + y0 * nx;
			for (int j = ymax; j >= ymin; j--) {
				int br = l;
				for (int i = xmin; i < xsize; i++) {
					if (l > lmax) {
						break;
					}
					int k = 0;
					unsigned char p;
					float t;
					if (dsx==1) t=image_data[l];
					else {						// This block does local pixel averaging for nicer reduced views
						t=0;
						for (int iii=0; iii<dsx; iii++) {
							for (int jjj=0; jjj<dsy; jjj+=nx) {
								t+=image_data[l+iii+jjj];
							}
						}
						t/=dsx*(dsy/nx);
					}

					if (t <= rm) p = mingray;
					else if (t >= render_max) p = maxgray;
					else if (gamma!=1.0) {
						k=(int)(gs2 * (t-render_min));		// map float value to 0-4096 range
						p = gammamap[k];					// apply gamma using precomputed gamma map
//						k = (int) (maxgray-mingray)*pow((gs2 * (t - render_min)),gamma);
//						k += mingray;
//						k = static_cast<int>( (maxgray-mingray)*pow((gs2 * (t - render_min)),gamma) );
//						k += mingray;
					}
					else {
						p = (unsigned char) (gs * (t - render_min));
						p += mingray;
					}
					data[i * asrgb + j * bpl] = p;
					if (hist) histd[p]++;
					l += dsx;
				}
				l = br + dsy;
			}
		}
		else {
			remy = 10;
			int l = x0 + y0 * nx;
			for (int j = ymax; j >= ymin; j--) {
				int addj = addi;
				// There seems to be some overflow issue happening
				// where the statement if (l > lmax) break (below) doesn't work
				// because the loop that iterates jjj can inadvertantly go out of bounds
				if (( l + addi*nx ) >= nxy ) {
					addj = (nxy-l)/nx;
					if (addj <= 0) continue;
				}
				int br = l;
				remx = 10;
				for (int i = xmin; i < xsize; i++) {
					if (l > lmax) break;
					int k = 0;
					unsigned char p;
					float t;
					if (addi<=1) t = image_data[l];
					else {						// This block does local pixel averaging for nicer reduced views
						t=0;
						for (int jjj=0; jjj<addj; jjj++) {
							for (int iii=0; iii<addi; iii++) {
								t+=image_data[l+iii+jjj*nx];
							}
						}
						t/=addi*addi;
					}
					if (t <= rm) p = mingray;
					else if (t >= render_max) p = maxgray;
					else if (gamma!=1.0) {
						k=(int)(gs2 * (t-render_min));		// map float value to 0-4096 range
						p = gammamap[k];					// apply gamma using precomputed gamma map
//						k = (int) (maxgray-mingray)*pow((gs2 * (t - render_min)),gamma);
//						k += mingray;
//						k = static_cast<int>( (maxgray-mingray)*pow((gs2 * (t - render_min)),gamma) );
//						k += mingray;
					}
					else {
						p = (unsigned char) (gs * (t - render_min));
						p += mingray;
					}
					data[i * asrgb + j * bpl] = p;
					if (hist) histd[p]++;
					l += addi;
					remx += addr;
					if (remx > scale_n) {
						remx -= scale_n;
						l++;
					}
				}
				l = br + addi * nx;
				remy += addr;
				if (remy > scale_n) {
					remy -= scale_n;
					l += nx;
				}
			}
		}
	}

	// this replicates r -> g,b
	if (asrgb==3) {
		for (int j=ymin*bpl; j<=ymax*bpl; j+=bpl) {
			for (int i=xmin; i<xsize*3; i+=3) {
				data[i+j+1]=data[i+j+2]=data[i+j];
			}
		}
	}
	if (asrgb==4) {
		for (int j=ymin*bpl; j<=ymax*bpl; j+=bpl) {
			for (int i=xmin; i<xsize*4; i+=4) {
				data[i+j+1]=data[i+j+2]=data[i+j+3]=data[i+j];
				data[i+j+3]=255;
			}
		}
	}

	EXITFUNC;

	// ok, ok, not the most efficient place to do this, but it works
	if (invy) {
		int x,y;
		char swp;
		for (y=0; y<iysize/2; y++) {
			for (x=0; x<ixsize; x++) {
				swp=ret[y*bpl+x];
				ret[y*bpl+x]=ret[(iysize-y-1)*bpl+x];
				ret[(iysize-y-1)*bpl+x]=swp;
			}
		}
	}

    //	return PyString_FromStringAndSize((const char*) data,iysize*bpl);
	if (flags&16) {
#ifdef EMAN2_USING_OPENGL
		glDrawPixels(ixsize,iysize,GL_LUMINANCE,GL_UNSIGNED_BYTE,(const GLvoid *)ret.data());
#endif //EMAN2_USING_OPENGL
	}

	return ret;
}

std::string EMData::render_ap24(int x0, int y0, int ixsize, int iysize,
						 int bpl, float scale, int mingray, int maxgray,
						 float render_min, float render_max,float gamma,int flags)
{
	ENTERFUNC;

	int asrgb;
	int hist=(flags&2)/2;
	int invy=(flags&4)?1:0;

	if (!is_complex()) throw ImageDimensionException("complex only");

	if (get_ndim() != 2) {
		throw ImageDimensionException("2D only");
	}

	if (is_complex()) ri2ap();

	if (render_max <= render_min) {
		render_max = render_min + 0.01f;
	}

	if (gamma<=0) gamma=1.0;

	// Calculating a full floating point gamma for
	// each pixel in the image slows rendering unacceptably
	// however, applying a gamma-mapping to an 8 bit colorspace
	// has unaccepable accuracy. So, we oversample the 8 bit colorspace
	// as a 12 bit colorspace and apply the gamma mapping to that
	// This should produce good accuracy for gamma values
	// larger than 0.5 (and a high upper limit)
	static int smg0=0,smg1=0;	// while this destroys threadsafety in the rendering process
	static float sgam=0;		// it is necessary for speed when rendering large numbers of small images
	static unsigned char gammamap[4096];
	if (gamma!=1.0 && (smg0!=mingray || smg1!=maxgray || sgam!=gamma)) {
		for (int i=0; i<4096; i++) {
			if (mingray<maxgray) gammamap[i]=(unsigned char)(mingray+(maxgray-mingray+0.999)*pow(((float)i/4096.0f),gamma));
			else gammamap[4095-i]=(unsigned char)(mingray+(maxgray-mingray+0.999)*pow(((float)i/4096.0f),gamma));
		}
	}
	smg0=mingray;	// so we don't recompute the map unless something changes
	smg1=maxgray;
	sgam=gamma;

	if (flags&8) asrgb=4;
	else if (flags&1) asrgb=3;
	else throw ImageDimensionException("must set flag 1 or 8");

	std::string ret=std::string();
//	ret.resize(iysize*bpl);
	ret.assign(iysize*bpl+hist*1024,char(mingray));
	unsigned char *data=(unsigned char *)ret.data();
	unsigned int *histd=(unsigned int *)(data+iysize*bpl);
	if (hist) {
		for (int i=0; i<256; i++) histd[i]=0;
	}

	float rm = render_min;
	float inv_scale = 1.0f / scale;
	int ysize = iysize;
	int xsize = ixsize;

	int ymin = 0;
	if (iysize * inv_scale > ny) {
		ymin = (int) (iysize - ny / inv_scale);
	}

	float gs = (maxgray - mingray) / (render_max - render_min);
	float gs2 = 4095.999f / (render_max - render_min);
//	float gs2 = 1.0 / (render_max - render_min);
	if (render_max < render_min) {
		gs = 0;
		rm = FLT_MAX;
	}

	int dsx = -1;
	int dsy = 0;
	int remx = 0;
	int remy = 0;
	const int scale_n = 100000;

	int addi = 0;
	int addr = 0;
	if (inv_scale == floor(inv_scale)) {
		dsx = (int) inv_scale;
		dsy = (int) (inv_scale * nx);
	}
	else {
		addi = (int) floor(inv_scale);
		addr = (int) (scale_n * (inv_scale - floor(inv_scale)));
	}

	int xmin = 0;
	if (x0 < 0) {
		xmin = (int) (-x0 / inv_scale);
		xsize -= (int) floor(x0 / inv_scale);
		x0 = 0;
	}

	if ((xsize - xmin) * inv_scale > (nx - x0)) {
		xsize = (int) ((nx - x0) / inv_scale + xmin);
	}
	int ymax = ysize - 1;
	if (y0 < 0) {
		ymax = (int) (ysize + y0 / inv_scale - 1);
		ymin += (int) floor(y0 / inv_scale);
		y0 = 0;
	}

	if (xmin < 0) xmin = 0;
	if (ymin < 0) ymin = 0;
	if (xsize > ixsize) xsize = ixsize;
	if (ymax > iysize) ymax = iysize;

	int lmax = nx * ny - 1;

	int mid=nx*ny/2;
	float* image_data = get_data();
	if (dsx != -1) {
		int l = y0 * nx;
		for (int j = ymax; j >= ymin; j--) {
			int ll = x0;
			for (int i = xmin; i < xsize; i++) {
				if (l + ll > lmax || ll >= nx - 2) break;

				int k = 0;
				unsigned char p;
				int ph;
				if (ll >= nx / 2) {
					if (l >= (ny - inv_scale) * nx) k = 2 * (ll - nx / 2) + 2;
					else k = 2 * (ll - nx / 2) + l + 2 + nx;
					ph = (int)(image_data[k+1]*768/(2.0*M_PI))+384;	// complex phase as integer 0-767
				}
				else {
					k = nx * ny - (l + 2 * ll) - 2;
					ph = (int)(-image_data[k+1]*768/(2.0*M_PI))+384;	// complex phase as integer 0-767
				}
				if (k>=mid) k-=mid;		// These 2 lines handle the Fourier origin being in the corner, not the middle
				else k+=mid; 
				float t = image_data[k];
				if (t <= rm)  p = mingray;
				else if (t >= render_max) p = maxgray;
				else if (gamma!=1.0) {
					k=(int)(gs2 * (t-render_min));		// map float value to 0-4096 range
					p = gammamap[k];					// apply gamma using precomputed gamma map
				}
				else {
					p = (unsigned char) (gs * (t - render_min));
					p += mingray;
				}
				if (ph<256) {
					data[i * asrgb + j * bpl] = p*(255-ph)/256;
					data[i * asrgb + j * bpl+1] = p*ph/256;
					data[i * asrgb + j * bpl+2] = 0;
				}
				else if (ph<512) {
					data[i * asrgb + j * bpl+1] = p*(511-ph)/256;
					data[i * asrgb + j * bpl+2] = p*(ph-256)/256;
					data[i * asrgb + j * bpl] = 0;
				}
				else {
					data[i * asrgb + j * bpl+2] = p*(767-ph)/256;
					data[i * asrgb + j * bpl] = p*(ph-512)/256;
					data[i * asrgb + j * bpl+1] = 0;
				}
				if (hist) histd[p]++;
				ll += dsx;
			}
			l += dsy;
		}
	}
	else {
		remy = 10;
		int l = y0 * nx;
		for (int j = ymax; j >= ymin; j--) {
			int br = l;
			remx = 10;
			int ll = x0;
			for (int i = xmin; i < xsize - 1; i++) {
				if (l + ll > lmax || ll >= nx - 2) {
					break;
				}
				int k = 0;
				unsigned char p;
				int ph;
				if (ll >= nx / 2) {
					if (l >= (ny * nx - nx)) k = 2 * (ll - nx / 2) + 2;
					else k = 2 * (ll - nx / 2) + l + 2 + nx;
					ph = (int)(image_data[k+1]*768/(2.0*M_PI))+384;	// complex phase as integer 0-767
				}
				else {
					k = nx * ny - (l + 2 * ll) - 2;
					ph = (int)(-image_data[k+1]*768/(2.0*M_PI))+384;	// complex phase as integer 0-767
				}
				if (k>=mid) k-=mid;		// These 2 lines handle the Fourier origin being in the corner, not the middle
				else k+=mid; 

				float t = image_data[k];
				if (t <= rm)
					p = mingray;
				else if (t >= render_max) {
					p = maxgray;
				}
				else if (gamma!=1.0) {
					k=(int)(gs2 * (t-render_min));		// map float value to 0-4096 range
					p = gammamap[k];					// apply gamma using precomputed gamma map
				}
				else {
					p = (unsigned char) (gs * (t - render_min));
					p += mingray;
				}
				if (ph<256) {
					data[i * asrgb + j * bpl] = p*(255-ph)/256;
					data[i * asrgb + j * bpl+1] = p*ph/256;
					data[i * asrgb + j * bpl+2] = 0;
				}
				else if (ph<512) {
					data[i * asrgb + j * bpl+1] = p*(511-ph)/256;
					data[i * asrgb + j * bpl+2] = p*(ph-256)/256;
					data[i * asrgb + j * bpl] = 0;
				}
				else {
					data[i * asrgb + j * bpl+2] = p*(767-ph)/256;
					data[i * asrgb + j * bpl] = p*(ph-512)/256;
					data[i * asrgb + j * bpl+1] = 0;
				}
				if (hist) histd[p]++;
				ll += addi;
				remx += addr;
				if (remx > scale_n) {
					remx -= scale_n;
					ll++;
				}
			}
			l = br + addi * nx;
			remy += addr;
			if (remy > scale_n) {
				remy -= scale_n;
				l += nx;
			}
		}
	}

	// this replicates r -> g,b
	if (asrgb==4) {
		for (int j=ymin*bpl; j<=ymax*bpl; j+=bpl) {
			for (int i=xmin; i<xsize*4; i+=4) {
				data[i+j+3]=255;
			}
		}
	}

	EXITFUNC;

	// ok, ok, not the most efficient place to do this, but it works
	if (invy) {
		int x,y;
		char swp;
		for (y=0; y<iysize/2; y++) {
			for (x=0; x<ixsize; x++) {
				swp=ret[y*bpl+x];
				ret[y*bpl+x]=ret[(iysize-y-1)*bpl+x];
				ret[(iysize-y-1)*bpl+x]=swp;
			}
		}
	}

    //	return PyString_FromStringAndSize((const char*) data,iysize*bpl);
	return ret;
}


void EMData::render_amp24( int x0, int y0, int ixsize, int iysize,
						  int bpl, float scale, int mingray, int maxgray,
						  float render_min, float render_max, void *ref,
						  void cmap(void *, int coord, unsigned char *tri))
{
	ENTERFUNC;

	if (get_ndim() != 2) {
		throw ImageDimensionException("2D only");
	}

	if (is_complex()) {
		ri2ap();
	}

	if (render_max <= render_min) {
		render_max = render_min + 0.01f;
	}

	std::string ret=std::string();
	ret.resize(iysize*bpl);
	unsigned char *data=(unsigned char *)ret.data();

	float rm = render_min;
	float inv_scale = 1.0f / scale;
	int ysize = iysize;
	int xsize = ixsize;
	const int scale_n = 100000;

	int ymin = 0;
	if ( iysize * inv_scale > ny) {
		ymin = (int) (iysize - ny / inv_scale);
	}
	float gs = (maxgray - mingray) / (render_max - render_min);
	if (render_max < render_min) {
		gs = 0;
		rm = FLT_MAX;
	}
	int dsx = -1;
	int dsy = 0;
	if (inv_scale == floor(inv_scale)) {
		dsx = (int) inv_scale;
		dsy = (int) (inv_scale * nx);
	}
	int addi = 0;
	int addr = 0;

	if (dsx == -1) {
		addi = (int) floor(inv_scale);
		addr = (int) (scale_n * (inv_scale - floor(inv_scale)));
	}

	int remx = 0;
	int remy = 0;
	int xmin = 0;
	if (x0 < 0) {
		xmin = (int) (-x0 / inv_scale);
		xsize -= (int) floor(x0 / inv_scale);
		x0 = 0;
	}

	if ((xsize - xmin) * inv_scale > (nx - x0)) {
		xsize = (int) ((nx - x0) / inv_scale + xmin);
	}
	int ymax = ysize - 1;
	if (y0 < 0) {
		ymax = (int) (ysize + y0 / inv_scale - 1);
		ymin += (int) floor(y0 / inv_scale);
		y0 = 0;
	}


	if (xmin < 0) {
		xmin = 0;
	}

	if (ymin < 0) {
		ymin = 0;
	}
	if (xsize > ixsize) {
		xsize = ixsize;
	}
	if (ymax > iysize) {
		ymax = iysize;
	}

	int lmax = nx * ny - 1;
	unsigned char tri[3];
	float* image_data = get_data();
	if (is_complex()) {
		if (dsx != -1) {
			int l = y0 * nx;
			for (int j = ymax; j >= ymin; j--) {
				int ll = x0;
				for (int i = xmin; i < xsize; i++, ll += dsx) {
					if (l + ll > lmax || ll >= nx - 2) {
						break;
					}
					int kk = 0;
					if (ll >= nx / 2) {
						if (l >= (ny - inv_scale) * nx) {
							kk = 2 * (ll - nx / 2) + 2;
						}
						else {
							kk = 2 * (ll - nx / 2) + l + 2 + nx;
						}
					}
					else {
						kk = nx * ny - (l + 2 * ll) - 2;
					}
					int k = 0;
					float t = image_data[kk];
					if (t <= rm) {
						k = mingray;
					}
					else if (t >= render_max) {
						k = maxgray;
					}
					else {
						k = (int) (gs * (t - render_min));
						k += mingray;
					}
					tri[0] = static_cast < unsigned char >(k);
					cmap(ref, kk, tri);
					data[i * 3 + j * bpl] = tri[0];
					data[i * 3 + 1 + j * bpl] = tri[1];
					data[i * 3 + 2 + j * bpl] = tri[2];
				}
				l += dsy;
			}
		}
		else {
			remy = 10;
			for (int j = ymax, l = y0 * nx; j >= ymin; j--) {
				int br = l;
				remx = 10;
				for (int i = xmin, ll = x0; i < xsize - 1; i++) {
					if (l + ll > lmax || ll >= nx - 2) {
						break;
					}
					int kk = 0;
					if (ll >= nx / 2) {
						if (l >= (ny * nx - nx)) {
							kk = 2 * (ll - nx / 2) + 2;
						}
						else {
							kk = 2 * (ll - nx / 2) + l + 2 + nx;
						}
					}
					else {
						kk = nx * ny - (l + 2 * ll) - 2;
					}
					int k = 0;
					float t = image_data[kk];
					if (t <= rm) {
						k = mingray;
					}
					else if (t >= render_max) {
						k = maxgray;
					}
					else {
						k = (int) (gs * (t - render_min));
						k += mingray;
					}
					tri[0] = static_cast < unsigned char >(k);
					cmap(ref, kk, tri);
					data[i * 3 + j * bpl] = tri[0];
					data[i * 3 + 1 + j * bpl] = tri[1];
					data[i * 3 + 2 + j * bpl] = tri[2];
					ll += addi;
					remx += addr;
					if (remx > scale_n) {
						remx -= scale_n;
						ll++;
					}
				}
				l = br + addi * nx;
				remy += addr;
				if (remy > scale_n) {
					remy -= scale_n;
					l += nx;
				}
			}
		}
	}
	else {
		if (dsx != -1) {
			for (int j = ymax, l = x0 + y0 * nx; j >= ymin; j--) {
				int br = l;
				for (int i = xmin; i < xsize; i++, l += dsx) {
					if (l > lmax) {
						break;
					}
					float t = image_data[l];
					int k = 0;
					if (t <= rm) {
						k = mingray;
					}
					else if (t >= render_max) {
						k = maxgray;
					}
					else {
						k = (int) (gs * (t - render_min));
						k += mingray;
					}
					tri[0] = static_cast < unsigned char >(k);
					cmap(ref, l, tri);
					data[i * 3 + j * bpl] = tri[0];
					data[i * 3 + 1 + j * bpl] = tri[1];
					data[i * 3 + 2 + j * bpl] = tri[2];
				}
				l = br + dsy;
			}
		}
		else {
			remy = 10;
			for (int j = ymax, l = x0 + y0 * nx; j >= ymin; j--) {
				int br = l;
				remx = 10;
				for (int i = xmin; i < xsize; i++) {
					if (l > lmax) {
						break;
					}
					float t = image_data[l];
					int k = 0;
					if (t <= rm) {
						k = mingray;
					}
					else if (t >= render_max) {
						k = maxgray;
					}
					else {
						k = (int) (gs * (t - render_min));
						k += mingray;
					}
					tri[0] = static_cast < unsigned char >(k);
					cmap(ref, l, tri);
					data[i * 3 + j * bpl] = tri[0];
					data[i * 3 + 1 + j * bpl] = tri[1];
					data[i * 3 + 2 + j * bpl] = tri[2];
					l += addi;
					remx += addr;
					if (remx > scale_n) {
						remx -= scale_n;
						l++;
					}
				}
				l = br + addi * nx;
				remy += addr;
				if (remy > scale_n) {
					remy -= scale_n;
					l += nx;
				}
			}
		}
	}

	EXITFUNC;
}

void EMData::ri2inten()
{
	ENTERFUNC;

	if (!is_complex()) return;
	if (!is_ri()) ap2ri();

	float * data = get_data();
	int size = nx * ny * nz;
	for (int i = 0; i < size; i += 2) {
		data[i]=data[i]*data[i]+data[i+1]*data[i+1];
		data[i+1]=0;
	}

	set_attr("is_intensity", int(1));
	update();
	EXITFUNC;
}


void EMData::ri2ap()
{
	ENTERFUNC;

	if (!is_complex() || !is_ri()) {
		return;
	}
	float * data = get_data();

	int size = nx * ny * nz;
	for (int i = 0; i < size; i += 2) {
#ifdef	_WIN32
		float f = (float)_hypot(data[i], data[i + 1]);
#else
		float f = (float)hypot(data[i], data[i + 1]);
#endif	
		if (data[i] == 0 && data[i + 1] == 0) {
			data[i + 1] = 0;
		}
		else {
			data[i + 1] = atan2(data[i + 1], data[i]);
		}
		data[i] = f;
	}

	set_ri(false);
	update();
	EXITFUNC;
}


void EMData::ap2ri()
{
	ENTERFUNC;

	if (!is_complex() || is_ri()) {
		return;
	}

	Util::ap2ri(get_data(), nx * ny * nz);
	set_ri(true);
	update();
	EXITFUNC;
}


void EMData::insert_clip(EMData * block, const IntPoint &origin)
{
	ENTERFUNC;
	int nx1 = block->get_xsize();
	int ny1 = block->get_ysize();
	int nz1 = block->get_zsize();

	Region area(origin[0], origin[1], origin[2],nx1, ny1, nz1);

	if (area.inside_region((float)nx, (float)ny, (float)nz)) {
		throw ImageFormatException("outside of destination image not supported.");
	}

	int x0 = origin[0];
	int y0 = origin[1];
	int y1 = origin[1] + ny1;
	int z0 = origin[2];
	int z1 = origin[2] + nz1;

	size_t inserted_row_size = nx1 * sizeof(float);
	float *inserted_data = block->get_data();
	float *src = inserted_data;
	float *dst = get_data() + z0 * nx * ny + y0 * nx + x0;

	for (int i = z0; i < z1; i++) {

		for (int j = y0; j < y1; j++) {
			memcpy(dst, src, inserted_row_size);
			src += nx1;
			dst += nx;
		}
		dst += nx * (ny - ny1);
	}

	update();
	EXITFUNC;
}


void EMData::insert_scaled_sum(EMData *block, const FloatPoint &center,
						   float scale, float)
{
	ENTERFUNC;
	float * data = get_data();
	if (get_ndim()==3) {
		// Start by determining the region to operate on
		int xs=(int)floor(block->get_xsize()*scale/2.0);
		int ys=(int)floor(block->get_ysize()*scale/2.0);
		int zs=(int)floor(block->get_zsize()*scale/2.0);
		int x0=(int)center[0]-xs;
		int x1=(int)center[0]+xs;
		int y0=(int)center[1]-ys;
		int y1=(int)center[1]+ys;
		int z0=(int)center[2]-zs;
		int z1=(int)center[2]+zs;

		if (x1<0||y1<0||z1<0||x0>get_xsize()||y0>get_ysize()||z0>get_zsize()) return;	// object is completely outside the target volume

		// make sure we stay inside the volume
		if (x0<0) x0=0;
		if (y0<0) y0=0;
		if (z0<0) z0=0;
		if (x1>=get_xsize()) x1=get_xsize()-1;
		if (y1>=get_ysize()) y1=get_ysize()-1;
		if (z1>=get_zsize()) z1=get_zsize()-1;

		float bx=block->get_xsize()/2.0f;
		float by=block->get_ysize()/2.0f;
		float bz=block->get_zsize()/2.0f;

		for (int x=x0; x<=x1; x++) {
			for (int y=y0; y<=y1; y++) {
				for (int z=z0; z<=z1; z++) {
					data[x + y * nx + z * nx * ny] +=
						block->sget_value_at_interp((x-center[0])/scale+bx,(y-center[1])/scale+by,(z-center[2])/scale+bz);
				}
			}
		}
		update();
	}
	else if (get_ndim()==2) {
		// Start by determining the region to operate on
		int xs=(int)floor(block->get_xsize()*scale/2.0);
		int ys=(int)floor(block->get_ysize()*scale/2.0);
		int x0=(int)center[0]-xs;
		int x1=(int)center[0]+xs;
		int y0=(int)center[1]-ys;
		int y1=(int)center[1]+ys;

		if (x1<0||y1<0||x0>get_xsize()||y0>get_ysize()) return;	// object is completely outside the target volume

		// make sure we stay inside the volume
		if (x0<0) x0=0;
		if (y0<0) y0=0;
		if (x1>=get_xsize()) x1=get_xsize()-1;
		if (y1>=get_ysize()) y1=get_ysize()-1;

		float bx=block->get_xsize()/2.0f;
		float by=block->get_ysize()/2.0f;

		for (int x=x0; x<=x1; x++) {
			for (int y=y0; y<=y1; y++) {
				data[x + y * nx] +=
					block->sget_value_at_interp((x-center[0])/scale+bx,(y-center[1])/scale+by);
			}
		}
		update();
	}
	else {
		LOGERR("insert_scaled_sum supports only 2D and 3D data");
		throw ImageDimensionException("2D/3D only");
	}

	EXITFUNC;
}
// 			else if ( m == 0 )
// 			{
// 				if ( n_f == -ny/2 )
// 				{
// 					t2++;
// // 					continue;
// 					for (int y = 0; y < return_slice->get_ysize(); ++y) {
// 						for (int x = 0; x < return_slice->get_xsize(); ++x) {
// 							double cur_val = return_slice->get_value_at(x,y);
// 							return_slice->set_value_at(x,y,cur_val+dat[idx]*std::pow(-1.0f,y));
// 						}
// 					}
// 					if (phase > 0.01 ) cout << "foo 2 " << phase << " " << amp << " " << dat[idx] << endl;
// 				}
// 				else
// 				{
// 					if ( n_f < 1 ) continue;
// 					t3++;
// 					for (int y = 0; y < return_slice->get_ysize(); ++y) {
// 						for (int x = 0; x < return_slice->get_xsize(); ++x) {
// 							double cur_val = return_slice->get_value_at(x,y);
// 							return_slice->set_value_at(x,y,cur_val+2*amp*cos(ndash*y+phase));
// 						}
// 					}
// 				}
// 			}
// 			else if ( n_f == -ny/2 )
// 			{
// 				if ( m == ((nx-2)/2) )
// 				{
// 					t4++;
// 					for (int y = 0; y < return_slice->get_ysize(); ++y) {
// 						for (int x = 0; x < return_slice->get_xsize(); ++x) {
// 							double cur_val = return_slice->get_value_at(x,y);
// 							return_slice->set_value_at(x,y,cur_val+dat[idx]*std::pow(-1.0f,x+y));
// 						}
// 					}
// 					if (phase > 0.01 ) cout << "foo 4 " << phase << " " << amp << " " << dat[idx] << endl;
// 				}
// 				else
// 				{
// 					t5++;
// 					for (int y = 0; y < return_slice->get_ysize(); ++y) {
// 						for (int x = 0; x < return_slice->get_xsize(); ++x) {
// 							double cur_val = return_slice->get_value_at(x,y);
// 							return_slice->set_value_at(x,y,cur_val+2*amp*cos(mdash*x+phase));
// 						}
// 					}
// 				}
// 			}
// 			else if ( n_f == 0 )
// 			{
// 				if ( m == ((nx-2)/2) )
// 				{
// 					t6++;
// 					for (int y = 0; y < return_slice->get_ysize(); ++y) {
// 						for (int x = 0; x < return_slice->get_xsize(); ++x) {
// 							double cur_val = return_slice->get_value_at(x,y);
// 							return_slice->set_value_at(x,y,cur_val+dat[idx]*std::pow(-1.0f,x));
// 						}
// 					}
// 					if (phase > 0.01 ) cout << "foo 3 " << phase << " " << amp << " " << dat[idx] << endl;
// 				}
// 				else
// 				{
// 					t7++;
// 					for (int y = 0; y < return_slice->get_ysize(); ++y) {
// 						for (int x = 0; x < return_slice->get_xsize(); ++x) {
// 							double cur_val = return_slice->get_value_at(x,y);
// 							return_slice->set_value_at(x,y,cur_val+2*amp*cos(mdash*x+M_PI*y + phase));
// 						}
// 					}
// 				}
// 			}
// 			else if ( m == ((nx-2)/2) )
// 			{
// 				if ( n_f < 1 ) continue;
// 				t8++;
// 				for (int y = 0; y < return_slice->get_ysize(); ++y) {
// 					for (int x = 0; x < return_slice->get_xsize(); ++x) {
// 						double cur_val = return_slice->get_value_at(x,y);
// 						return_slice->set_value_at(x,y,cur_val+2*amp*cos(ndash*y+M_PI*x+phase)); 
// 					}
// 				}
// 			}
// }
