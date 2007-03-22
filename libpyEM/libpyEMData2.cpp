
// Boost Includes ==============================================================
#include <boost/python.hpp>
#include <boost/cstdint.hpp>

// Includes ====================================================================
#include <aligner.h>
#include <cmp.h>
#include <ctf.h>
#include <emdata_pickle.h>
#include <emdata_wrapitems.h>
#include <emfft.h>
#include <processor.h>
#include <transform.h>
#include <xydata.h>

// Using =======================================================================
using namespace boost::python;

// Declarations ================================================================
namespace  {

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_read_image_overloads_1_5, read_image, 1, 5)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_write_image_overloads_1_7, write_image, 1, 7)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_append_image_overloads_1_3, append_image, 1, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_write_lst_overloads_1_4, write_lst, 1, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_print_image_overloads_0_2, print_image, 0, 2)

BOOST_PYTHON_FUNCTION_OVERLOADS(EMAN_EMData_read_images_overloads_1_3, EMAN::EMData::read_images, 1, 3)

BOOST_PYTHON_FUNCTION_OVERLOADS(EMAN_EMData_read_images_ext_overloads_3_5, EMAN::EMData::read_images_ext, 3, 5)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_set_size_overloads_1_3, set_size, 1, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_set_complex_size_overloads_1_3, set_complex_size, 1, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_get_attr_default_overloads_1_2, get_attr_default, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_process_inplace_overloads_1_2, process_inplace, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_process_overloads_1_2, process, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_cmp_overloads_2_3, cmp, 2, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_align_overloads_2_5, align, 2, 5)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_project_overloads_1_2, project, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_backproject_overloads_1_2, backproject, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_insert_scaled_sum_overloads_2_4, insert_scaled_sum, 2, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_add_overloads_1_2, add, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_set_array_offsets_overloads_0_3, set_array_offsets, 0, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_real2complex_overloads_0_1, real2complex, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_FH2F_overloads_2_3, FH2F, 2, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_FH2Real_overloads_2_3, FH2Real, 2, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_calc_fourier_shell_correlation_overloads_1_2, calc_fourier_shell_correlation, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_nn_overloads_3_4, nn, 3, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_nn_SSNR_overloads_4_5, nn_SSNR, 4, 5)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_rot_scale_trans2D_overloads_1_4, rot_scale_trans2D, 1, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_rot_scale_conv_overloads_4_5, rot_scale_conv, 4, 5)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_getconvpt2d_kbi0_overloads_3_4, getconvpt2d_kbi0, 3, 4)

BOOST_PYTHON_FUNCTION_OVERLOADS(EMAN_EMData_ctf_img_overloads_5_12, EMAN::EMData::ctf_img, 5, 12)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_zeropad_ntimes_overloads_0_1, zeropad_ntimes, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_pad_fft_overloads_0_1, pad_fft, 0, 1)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_FourInterpol_overloads_1_4, FourInterpol, 1, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_get_rotated_clip_overloads_2_3, get_rotated_clip, 2, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_little_big_dot_overloads_1_2, little_big_dot, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_calc_ccf_overloads_1_2, calc_ccf, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_calc_ccfx_overloads_1_4, calc_ccfx, 1, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_make_rotational_footprint_overloads_0_2, make_rotational_footprint, 0, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_calc_mutual_correlation_overloads_1_3, calc_mutual_correlation, 1, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_unwrap_overloads_0_6, unwrap, 0, 6)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_apply_radial_func_overloads_3_4, apply_radial_func, 3, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_calc_hist_overloads_0_3, calc_hist, 0, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_calc_dist_overloads_1_2, calc_dist, 1, 2)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_calc_flcf_overloads_1_3, calc_flcf, 1, 3)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_common_lines_overloads_2_5, common_lines, 2, 5)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_common_lines_real_overloads_2_4, common_lines_real, 2, 4)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_cut_slice_overloads_2_6, cut_slice, 2, 6)

BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EMAN_EMData_uncut_slice_overloads_2_5, uncut_slice, 2, 5)


}// namespace 


// Module ======================================================================
BOOST_PYTHON_MODULE(libpyEMData2)
{
    scope* EMAN_EMData_scope = new scope(
    class_< EMAN::EMData >("EMData", init<  >())
        .def(init< const EMAN::EMData& >())
        .def(init< const std::string&, optional< int > >())
        .def(init< int, int, optional< int, bool > >())
        .def_readwrite("totalalloc", &EMAN::EMData::totalalloc)
        .def("read_image", &EMAN::EMData::read_image, EMAN_EMData_read_image_overloads_1_5())
        .def("write_image", &EMAN::EMData::write_image, EMAN_EMData_write_image_overloads_1_7())
        .def("append_image", &EMAN::EMData::append_image, EMAN_EMData_append_image_overloads_1_3())
        .def("write_lst", &EMAN::EMData::write_lst, EMAN_EMData_write_lst_overloads_1_4())
        .def("print_image", &EMAN::EMData::print_image, EMAN_EMData_print_image_overloads_0_2())
        .def("read_images", &EMAN::EMData::read_images, EMAN_EMData_read_images_overloads_1_3())
        .def("read_images_ext", &EMAN::EMData::read_images_ext, EMAN_EMData_read_images_ext_overloads_3_5())
        .def("get_fft_amplitude", &EMAN::EMData::get_fft_amplitude, return_value_policy< manage_new_object >())
        .def("get_fft_amplitude2D", &EMAN::EMData::get_fft_amplitude2D, return_value_policy< manage_new_object >())
        .def("get_fft_phase", &EMAN::EMData::get_fft_phase, return_value_policy< manage_new_object >())
        .def("done_data", &EMAN::EMData::done_data)
        .def("update", &EMAN::EMData::update)
        .def("has_ctff", &EMAN::EMData::has_ctff)
        .def("calc_center_density", &EMAN::EMData::calc_center_density)
        .def("calc_sigma_diff", &EMAN::EMData::calc_sigma_diff)
        .def("calc_min_location", &EMAN::EMData::calc_min_location)
        .def("calc_max_location", &EMAN::EMData::calc_max_location)
        .def("calc_min_index", &EMAN::EMData::calc_min_index)
        .def("calc_max_index", &EMAN::EMData::calc_max_index)
        .def("calc_highest_locations", &EMAN::EMData::calc_highest_locations)
        .def("get_edge_mean", &EMAN::EMData::get_edge_mean)
        .def("get_circle_mean", &EMAN::EMData::get_circle_mean)
        .def("get_ctf", &EMAN::EMData::get_ctf, return_value_policy< manage_new_object >())
        .def("set_ctf", &EMAN::EMData::set_ctf)
        .def("get_translation", &EMAN::EMData::get_translation)
        .def("set_translation", (void (EMAN::EMData::*)(const EMAN::Vec3f&) )&EMAN::EMData::set_translation)
        .def("set_translation", (void (EMAN::EMData::*)(float, float, float) )&EMAN::EMData::set_translation)
        .def("get_transform", &EMAN::EMData::get_transform)
        .def("set_rotation", &EMAN::EMData::set_rotation)
        .def("set_size", &EMAN::EMData::set_size, EMAN_EMData_set_size_overloads_1_3())
        .def("set_complex_size", &EMAN::EMData::set_complex_size, EMAN_EMData_set_complex_size_overloads_1_3())
        .def("set_path", &EMAN::EMData::set_path)
        .def("set_pathnum", &EMAN::EMData::set_pathnum)
        .def("get_2dview", (EMAN::MArray2D (EMAN::EMData::*)() const)&EMAN::EMData::get_2dview)
        .def("get_3dview", (EMAN::MArray3D (EMAN::EMData::*)() const)&EMAN::EMData::get_3dview)
        .def("get_2dcview", (EMAN::MCArray2D (EMAN::EMData::*)() const)&EMAN::EMData::get_2dcview)
        .def("get_3dcview", (EMAN::MCArray3D (EMAN::EMData::*)() const)&EMAN::EMData::get_3dcview)
        .def("get_3dcviewptr", &EMAN::EMData::get_3dcviewptr, return_value_policy< reference_existing_object >())
        .def("get_2dview", (EMAN::MArray2D (EMAN::EMData::*)(int, int) const)&EMAN::EMData::get_2dview)
        .def("get_3dview", (EMAN::MArray3D (EMAN::EMData::*)(int, int, int) const)&EMAN::EMData::get_3dview)
        .def("get_2dcview", (EMAN::MCArray2D (EMAN::EMData::*)(int, int) const)&EMAN::EMData::get_2dcview)
        .def("get_3dcview", (EMAN::MCArray3D (EMAN::EMData::*)(int, int, int) const)&EMAN::EMData::get_3dcview)
        .def("get_attr", &EMAN::EMData::get_attr)
        .def("get_attr_default", &EMAN::EMData::get_attr_default, EMAN_EMData_get_attr_default_overloads_1_2())
        .def("set_attr", &EMAN::EMData::set_attr)
        .def("get_attr_dict", &EMAN::EMData::get_attr_dict)
        .def("set_attr_dict", &EMAN::EMData::set_attr_dict)
        .def("del_attr", &EMAN::EMData::del_attr)
        .def("del_attr_dict", &EMAN::EMData::del_attr_dict)
        .def("get_xsize", &EMAN::EMData::get_xsize)
        .def("get_ysize", &EMAN::EMData::get_ysize)
        .def("get_zsize", &EMAN::EMData::get_zsize)
        .def("get_ndim", &EMAN::EMData::get_ndim)
        .def("is_shuffled", &EMAN::EMData::is_shuffled)
        .def("is_FH", &EMAN::EMData::is_FH)
        .def("is_complex", &EMAN::EMData::is_complex)
        .def("is_real", &EMAN::EMData::is_real)
        .def("set_shuffled", &EMAN::EMData::set_shuffled)
        .def("set_FH", &EMAN::EMData::set_FH)
        .def("set_complex", &EMAN::EMData::set_complex)
        .def("is_complex_x", &EMAN::EMData::is_complex_x)
        .def("set_complex_x", &EMAN::EMData::set_complex_x)
        .def("is_flipped", &EMAN::EMData::is_flipped)
        .def("set_flipped", &EMAN::EMData::set_flipped)
        .def("is_ri", &EMAN::EMData::is_ri)
        .def("set_ri", &EMAN::EMData::set_ri)
        .def("is_fftpadded", &EMAN::EMData::is_fftpadded)
        .def("set_fftpad", &EMAN::EMData::set_fftpad)
        .def("is_fftodd", &EMAN::EMData::is_fftodd)
        .def("set_fftodd", &EMAN::EMData::set_fftodd)
        .def("set_nxc", &EMAN::EMData::set_nxc)
        .def("get_flags", &EMAN::EMData::get_flags)
        .def("set_flags", &EMAN::EMData::set_flags)
        .def("get_changecount", &EMAN::EMData::get_changecount)
        .def("set_changecount", &EMAN::EMData::set_changecount)
        .def("get_xoff", &EMAN::EMData::get_xoff)
        .def("get_yoff", &EMAN::EMData::get_yoff)
        .def("get_zoff", &EMAN::EMData::get_zoff)
        .def("set_xyzoff", &EMAN::EMData::set_xyzoff)
        .def("get_path", &EMAN::EMData::get_path)
        .def("get_pathnum", &EMAN::EMData::get_pathnum)
        .def("get_data_pickle", &EMAN::EMData::get_data_pickle)
        .def("set_data_pickle", &EMAN::EMData::set_data_pickle)
        .def("get_supp_pickle", &EMAN::EMData::get_supp_pickle)
        .def("set_supp_pickle", &EMAN::EMData::set_supp_pickle)
        .def("process_inplace", (void (EMAN::EMData::*)(const std::string&, const EMAN::Dict&) )&EMAN::EMData::process_inplace, EMAN_EMData_process_inplace_overloads_1_2())
        .def("process_inplace", (void (EMAN::EMData::*)(EMAN::Processor*) )&EMAN::EMData::process_inplace)
        .def("process", (EMAN::EMData* (EMAN::EMData::*)(const std::string&, const EMAN::Dict&) )&EMAN::EMData::process, return_value_policy< manage_new_object >(), EMAN_EMData_process_overloads_1_2())
        .def("process", (EMAN::EMData* (EMAN::EMData::*)(EMAN::Processor*) )&EMAN::EMData::process, return_value_policy< manage_new_object >())
        .def("cmp", &EMAN::EMData::cmp, EMAN_EMData_cmp_overloads_2_3())
        .def("align", &EMAN::EMData::align, return_value_policy< manage_new_object >(), EMAN_EMData_align_overloads_2_5())
        .def("project", &EMAN::EMData::project, return_value_policy< manage_new_object >(), EMAN_EMData_project_overloads_1_2())
        .def("backproject", &EMAN::EMData::backproject, return_value_policy< manage_new_object >(), EMAN_EMData_backproject_overloads_1_2())
        .def("do_fft", &EMAN::EMData::do_fft, return_value_policy< manage_new_object >())
        .def("do_fft_inplace", &EMAN::EMData::do_fft_inplace, return_value_policy< reference_existing_object >())
        .def("do_ift", &EMAN::EMData::do_ift, return_value_policy< manage_new_object >())
        .def("do_ift_inplace", &EMAN::EMData::do_ift_inplace, return_value_policy< reference_existing_object >())
        .def("render_amp8", &EMAN::EMData::render_amp8)
        .def("ri2ap", &EMAN::EMData::ri2ap)
        .def("ap2ri", &EMAN::EMData::ap2ri)
        .def("insert_clip", &EMAN::EMData::insert_clip)
        .def("insert_scaled_sum", &EMAN::EMData::insert_scaled_sum, EMAN_EMData_insert_scaled_sum_overloads_2_4())
        .def("copy", &EMAN::EMData::copy, return_value_policy< manage_new_object >())
        .def("copy_head", &EMAN::EMData::copy_head, return_value_policy< manage_new_object >())
        .def("add", (void (EMAN::EMData::*)(float, int) )&EMAN::EMData::add, EMAN_EMData_add_overloads_1_2())
        .def("add", (void (EMAN::EMData::*)(const EMAN::EMData&) )&EMAN::EMData::add)
        .def("sub", (void (EMAN::EMData::*)(float) )&EMAN::EMData::sub)
        .def("sub", (void (EMAN::EMData::*)(const EMAN::EMData&) )&EMAN::EMData::sub)
        .def("mult", (void (EMAN::EMData::*)(int) )&EMAN::EMData::mult)
        .def("mult", (void (EMAN::EMData::*)(float) )&EMAN::EMData::mult)
        .def("mult", (void (EMAN::EMData::*)(const EMAN::EMData&) )&EMAN::EMData::mult)
        .def("div", (void (EMAN::EMData::*)(float) )&EMAN::EMData::div)
        .def("div", (void (EMAN::EMData::*)(const EMAN::EMData&) )&EMAN::EMData::div)
        .def("to_zero", &EMAN::EMData::to_zero)
        .def("to_one", &EMAN::EMData::to_one)
        .def("dot", &EMAN::EMData::dot)
        .def("get_row", &EMAN::EMData::get_row, return_value_policy< manage_new_object >())
        .def("set_row", &EMAN::EMData::set_row)
        .def("get_col", &EMAN::EMData::get_col, return_value_policy< manage_new_object >())
        .def("set_col", &EMAN::EMData::set_col)
        .def("get_value_at", (float (EMAN::EMData::*)(int, int, int) const)&EMAN::EMData::get_value_at)
        .def("get_value_at", (float (EMAN::EMData::*)(int, int) const)&EMAN::EMData::get_value_at)
        .def("get_value_at", (float (EMAN::EMData::*)(size_t) const)&EMAN::EMData::get_value_at)
        .def("sget_value_at", (float (EMAN::EMData::*)(int, int, int) const)&EMAN::EMData::sget_value_at)
        .def("sget_value_at", (float (EMAN::EMData::*)(int, int) const)&EMAN::EMData::sget_value_at)
        .def("sget_value_at", (float (EMAN::EMData::*)(size_t) const)&EMAN::EMData::sget_value_at)
        .def("sget_value_at_interp", (float (EMAN::EMData::*)(float, float) const)&EMAN::EMData::sget_value_at_interp)
        .def("sget_value_at_interp", (float (EMAN::EMData::*)(float, float, float) const)&EMAN::EMData::sget_value_at_interp)
        .def("set_value_at", (void (EMAN::EMData::*)(int, int, int, float) )&EMAN::EMData::set_value_at)
        .def("set_value_at_fast", (void (EMAN::EMData::*)(int, int, int, float) )&EMAN::EMData::set_value_at_fast)
        .def("set_value_at", (void (EMAN::EMData::*)(int, int, float) )&EMAN::EMData::set_value_at)
        .def("set_value_at_fast", (void (EMAN::EMData::*)(int, int, float) )&EMAN::EMData::set_value_at_fast)
        .def("set_value_at", (void (EMAN::EMData::*)(int, float) )&EMAN::EMData::set_value_at)
        .def("set_array_offsets", (void (EMAN::EMData::*)(const int, const int, const int) )&EMAN::EMData::set_array_offsets, EMAN_EMData_set_array_offsets_overloads_0_3())
        .def("set_array_offsets", (void (EMAN::EMData::*)(std::vector<int,std::allocator<int> >) )&EMAN::EMData::set_array_offsets)
        .def("get_array_offsets", &EMAN::EMData::get_array_offsets)
        .def("power", &EMAN::EMData::power, return_value_policy< manage_new_object >())
        .def("sqrt", &EMAN::EMData::sqrt, return_value_policy< manage_new_object >())
        .def("log", &EMAN::EMData::log, return_value_policy< manage_new_object >())
        .def("log10", &EMAN::EMData::log10, return_value_policy< manage_new_object >())
        .def("real", &EMAN::EMData::real, return_value_policy< manage_new_object >())
        .def("imag", &EMAN::EMData::imag, return_value_policy< manage_new_object >())
        .def("amplitude", &EMAN::EMData::amplitude, return_value_policy< manage_new_object >())
        .def("phase", &EMAN::EMData::phase, return_value_policy< manage_new_object >())
        .def("real2complex", &EMAN::EMData::real2complex, return_value_policy< manage_new_object >(), EMAN_EMData_real2complex_overloads_0_1())
        .def("real2FH", &EMAN::EMData::real2FH, return_value_policy< manage_new_object >())
        .def("FH2F", &EMAN::EMData::FH2F, return_value_policy< manage_new_object >(), EMAN_EMData_FH2F_overloads_2_3())
        .def("FH2Real", &EMAN::EMData::FH2Real, return_value_policy< manage_new_object >(), EMAN_EMData_FH2Real_overloads_2_3())
        .def("rotavg", &EMAN::EMData::rotavg, return_value_policy< manage_new_object >())
        .def("cog", &EMAN::EMData::cog)
        .def("calc_fourier_shell_correlation", &EMAN::EMData::calc_fourier_shell_correlation, EMAN_EMData_calc_fourier_shell_correlation_overloads_1_2())
        .def("average_circ_sub", &EMAN::EMData::average_circ_sub, return_value_policy< manage_new_object >())
        .def("onelinenn", &EMAN::EMData::onelinenn)
        .def("onelinenn_mult", &EMAN::EMData::onelinenn_mult)
        .def("nn", &EMAN::EMData::nn, EMAN_EMData_nn_overloads_3_4())
        .def("nn_SSNR", &EMAN::EMData::nn_SSNR, EMAN_EMData_nn_SSNR_overloads_4_5())
        .def("symplane0", &EMAN::EMData::symplane0)
        .def("symplane1", &EMAN::EMData::symplane1)
        .def("onelinenn_ctf", &EMAN::EMData::onelinenn_ctf)
        .def("nn_ctf", &EMAN::EMData::nn_ctf)
        .def("onelinenn_ctf_applied", &EMAN::EMData::onelinenn_ctf_applied)
        .def("nn_ctf_applied", &EMAN::EMData::nn_ctf_applied)
        .def("symplane0_ctf", &EMAN::EMData::symplane0_ctf)
        .def("symvol", &EMAN::EMData::symvol, return_value_policy< manage_new_object >())
        .def("rot_scale_trans2D", &EMAN::EMData::rot_scale_trans2D, return_value_policy< manage_new_object >(), EMAN_EMData_rot_scale_trans2D_overloads_1_4())
        .def("rot_scale_trans", &EMAN::EMData::rot_scale_trans, return_value_policy< manage_new_object >())
        .def("cm_euc", &EMAN::EMData::cm_euc)
        .def("rot_scale_conv", &EMAN::EMData::rot_scale_conv, return_value_policy< manage_new_object >(), EMAN_EMData_rot_scale_conv_overloads_4_5())
        .def("get_pixel_conv", &EMAN::EMData::get_pixel_conv)
        .def("getconvpt2d_kbi0", &EMAN::EMData::getconvpt2d_kbi0, EMAN_EMData_getconvpt2d_kbi0_overloads_3_4())
        .def("fft_shuffle", &EMAN::EMData::fft_shuffle)
        .def("center_padded", &EMAN::EMData::center_padded)
        .def("extractpoint", &EMAN::EMData::extractpoint)
        .def("extractplane", &EMAN::EMData::extractplane, return_value_policy< manage_new_object >())
        .def("fouriergridrot2d", &EMAN::EMData::fouriergridrot2d, return_value_policy< manage_new_object >())
        .def("divkbsinh", &EMAN::EMData::divkbsinh)
        .def("peak_search", &EMAN::EMData::peak_search)
        .def("phase_cog", &EMAN::EMData::phase_cog)
        .def("find_3d_threshold", &EMAN::EMData::find_3d_threshold)
        .def("peak_ccf", &EMAN::EMData::peak_ccf)
        .def("get_pow", &EMAN::EMData::get_pow, return_value_policy< manage_new_object >())
        .def("extractline", &EMAN::EMData::extractline, return_value_policy< manage_new_object >())
        .def("ctf_img", &EMAN::EMData::ctf_img, return_value_policy< manage_new_object >(), EMAN_EMData_ctf_img_overloads_5_12())
        .def("center_origin", &EMAN::EMData::center_origin)
        .def("center_origin_fft", &EMAN::EMData::center_origin_fft)
        .def("zeropad_ntimes", &EMAN::EMData::zeropad_ntimes, return_value_policy< manage_new_object >(), EMAN_EMData_zeropad_ntimes_overloads_0_1())
        .def("pad_fft", &EMAN::EMData::pad_fft, return_value_policy< manage_new_object >(), EMAN_EMData_pad_fft_overloads_0_1())
        .def("postift_depad_corner_inplace", &EMAN::EMData::postift_depad_corner_inplace)
        .def("FourInterpol", &EMAN::EMData::FourInterpol, return_value_policy< manage_new_object >(), EMAN_EMData_FourInterpol_overloads_1_4())
        .def("get_clip", &EMAN::EMData::get_clip, return_value_policy< manage_new_object >())
        .def("get_top_half", &EMAN::EMData::get_top_half, return_value_policy< manage_new_object >())
        .def("get_rotated_clip", &EMAN::EMData::get_rotated_clip, return_value_policy< manage_new_object >(), EMAN_EMData_get_rotated_clip_overloads_2_3())
        .def("window_center", &EMAN::EMData::window_center, return_value_policy< manage_new_object >())
        .def("scale", &EMAN::EMData::scale)
        .def("translate", (void (EMAN::EMData::*)(float, float, float) )&EMAN::EMData::translate)
        .def("translate", (void (EMAN::EMData::*)(const EMAN::Vec3f&) )&EMAN::EMData::translate)
        .def("translate", (void (EMAN::EMData::*)(int, int, int) )&EMAN::EMData::translate)
        .def("translate", (void (EMAN::EMData::*)(const EMAN::Vec3i&) )&EMAN::EMData::translate)
        .def("rotate", (void (EMAN::EMData::*)(const EMAN::Transform3D&) )&EMAN::EMData::rotate)
        .def("rotate", (void (EMAN::EMData::*)(float, float, float) )&EMAN::EMData::rotate)
        .def("rotate_translate", (void (EMAN::EMData::*)(const EMAN::Transform3D&) )&EMAN::EMData::rotate_translate)
        .def("rotate_translate", (void (EMAN::EMData::*)(float, float, float, float, float, float) )&EMAN::EMData::rotate_translate)
        .def("rotate_translate", (void (EMAN::EMData::*)(float, float, float, float, float, float, float, float, float) )&EMAN::EMData::rotate_translate)
        .def("rotate_x", &EMAN::EMData::rotate_x)
        .def("rotate_180", &EMAN::EMData::rotate_180)
        .def("dot_rotate_translate", &EMAN::EMData::dot_rotate_translate)
        .def("little_big_dot", &EMAN::EMData::little_big_dot, return_value_policy< manage_new_object >(), EMAN_EMData_little_big_dot_overloads_1_2())
        .def("do_radon", &EMAN::EMData::do_radon, return_value_policy< manage_new_object >())
        .def("calc_ccf", &EMAN::EMData::calc_ccf, return_value_policy< manage_new_object >(), EMAN_EMData_calc_ccf_overloads_1_2())
        .def("calc_ccfx", &EMAN::EMData::calc_ccfx, return_value_policy< manage_new_object >(), EMAN_EMData_calc_ccfx_overloads_1_4())
        .def("make_rotational_footprint", &EMAN::EMData::make_rotational_footprint, return_value_policy< manage_new_object >(), EMAN_EMData_make_rotational_footprint_overloads_0_2())
        .def("make_footprint", &EMAN::EMData::make_footprint, return_value_policy< manage_new_object >())
        .def("calc_mutual_correlation", &EMAN::EMData::calc_mutual_correlation, return_value_policy< manage_new_object >(), EMAN_EMData_calc_mutual_correlation_overloads_1_3())
        .def("unwrap", &EMAN::EMData::unwrap, return_value_policy< manage_new_object >(), EMAN_EMData_unwrap_overloads_0_6())
        .def("mean_shrink", &EMAN::EMData::mean_shrink)
        .def("median_shrink", &EMAN::EMData::median_shrink)
        .def("apply_radial_func", &EMAN::EMData::apply_radial_func, EMAN_EMData_apply_radial_func_overloads_3_4())
        .def("calc_radial_dist", (std::vector<float,std::allocator<float> > (EMAN::EMData::*)(int, float, float, bool) )&EMAN::EMData::calc_radial_dist)
        .def("calc_radial_dist", (std::vector<float,std::allocator<float> > (EMAN::EMData::*)(int, float, float, int, bool) )&EMAN::EMData::calc_radial_dist)
        .def("cconj", &EMAN::EMData::cconj)
        .def("add_incoherent", &EMAN::EMData::add_incoherent)
        .def("calc_hist", &EMAN::EMData::calc_hist, EMAN_EMData_calc_hist_overloads_0_3())
        .def("calc_az_dist", &EMAN::EMData::calc_az_dist)
        .def("calc_dist", &EMAN::EMData::calc_dist, EMAN_EMData_calc_dist_overloads_1_2())
        .def("calc_flcf", &EMAN::EMData::calc_flcf, return_value_policy< manage_new_object >(), EMAN_EMData_calc_flcf_overloads_1_3())
        .def("convolute", &EMAN::EMData::convolute, return_value_policy< manage_new_object >())
        .def("common_lines", &EMAN::EMData::common_lines, EMAN_EMData_common_lines_overloads_2_5())
        .def("common_lines_real", &EMAN::EMData::common_lines_real, EMAN_EMData_common_lines_real_overloads_2_4())
        .def("cut_slice", &EMAN::EMData::cut_slice, EMAN_EMData_cut_slice_overloads_2_6())
        .def("uncut_slice", &EMAN::EMData::uncut_slice, EMAN_EMData_uncut_slice_overloads_2_5())
        .def("__getitem__", &emdata_getitem)
        .def("__setitem__", &emdata_setitem)
        .staticmethod("ctf_img")
        .staticmethod("read_images_ext")
        .staticmethod("read_images")
        .def( self / self )
        .def( self - self )
        .def( self + self )
        .def( other< float >() / self )
        .def( self / other< float >() )
        .def( self * other< float >() )
        .def( self - other< float >() )
        .def( self * self )
        .def( other< float >() * self )
        .def( other< float >() - self )
        .def( other< float >() + self )
        .def( self + other< float >() )
        .def( self += other< float >() )
        .def( self -= other< float >() )
        .def( self *= other< float >() )
        .def( self /= other< float >() )
        .def( self += self )
        .def( self -= self )
        .def( self *= self )
        .def( self /= self )
        .def("__call__", (float& (EMAN::EMData::*)(const int, const int, const int) )&EMAN::EMData::operator (), return_value_policy< copy_non_const_reference >())
        .def("__call__", (float& (EMAN::EMData::*)(const int, const int) )&EMAN::EMData::operator (), return_value_policy< copy_non_const_reference >())
        .def("__call__", (float& (EMAN::EMData::*)(const int) )&EMAN::EMData::operator (), return_value_policy< copy_non_const_reference >())
    );

    enum_< EMAN::EMData::FFTPLACE >("FFTPLACE")
        .value("FFT_IN_PLACE", EMAN::EMData::FFT_IN_PLACE)
        .value("FFT_OUT_OF_PLACE", EMAN::EMData::FFT_OUT_OF_PLACE)
    ;


    enum_< EMAN::EMData::WINDOWPLACE >("WINDOWPLACE")
        .value("WINDOW_OUT_OF_PLACE", EMAN::EMData::WINDOW_OUT_OF_PLACE)
        .value("WINDOW_IN_PLACE", EMAN::EMData::WINDOW_IN_PLACE)
    ;

    delete EMAN_EMData_scope;

}

