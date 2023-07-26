#include "stdafx.h"
#include "HevcDefs.h"

NALUnit::NALUnit(NALUnitType type)
	: m_nalUnitType(type)
	, m_processFailed(false)
{
}
NALUnit::~NALUnit()
{
}
NALUnitType NALUnit::getType() const
{
	return m_nalUnitType;
}

VPS::VPS(): NALUnit(NAL_VPS) 
{
	Default();
}
void VPS::Default()
{
	vps_video_parameter_set_id = 0;
	vps_max_layers_minus1 = 0;
	vps_max_sub_layers_minus1 = 0;
	vps_temporal_id_nesting_flag = 0;
	profile_tier_level.Default();
	vps_sub_layer_ordering_info_present_flag = 0;
	vps_max_dec_pic_buffering_minus1.clear();
	vps_max_num_reorder_pics.clear();
	vps_max_latency_increase_plus1.clear();
	vps_max_layer_id = 0;
	vps_num_layer_sets_minus1 = 0;
	layer_id_included_flag.clear();
	vps_timing_info_present_flag = 0;
	vps_num_units_in_tick = 0;
	vps_time_scale = 0;
	vps_poc_proportional_to_timing_flag = 0;
	vps_num_ticks_poc_diff_one_minus1 = 0;
	vps_num_hrd_parameters = 0;
	hrd_layer_set_idx.clear();
	cprms_present_flag.clear();
	hrd_parameters.clear();
	vps_extension_flag = 0;
}
bool VPS::operator == (const VPS &vps) const
{
	if(this == &vps)
		return true;	
	if(vps_video_parameter_set_id != vps.vps_video_parameter_set_id)
		return false;
	if(vps_max_layers_minus1 != vps.vps_max_layers_minus1)
		return false;
	if(vps_max_sub_layers_minus1 != vps.vps_max_sub_layers_minus1)
		return false;
	if(vps_temporal_id_nesting_flag != vps.vps_temporal_id_nesting_flag)
		return false;
	if(!(profile_tier_level == vps.profile_tier_level))
		return false;
	if(vps_sub_layer_ordering_info_present_flag != vps.vps_sub_layer_ordering_info_present_flag)
		return false;
	if(vps_max_dec_pic_buffering_minus1 != vps.vps_max_dec_pic_buffering_minus1)
		return false;
	if(vps_max_num_reorder_pics != vps.vps_max_num_reorder_pics)
		return false;
	if(vps_max_latency_increase_plus1 != vps.vps_max_latency_increase_plus1)
		return false;
	if(vps_max_layer_id != vps.vps_max_layer_id)
		return false;
	if(vps_num_layer_sets_minus1 != vps.vps_num_layer_sets_minus1)
		return false;
	if(layer_id_included_flag != vps.layer_id_included_flag)
		return false;
	if(vps_timing_info_present_flag != vps.vps_timing_info_present_flag)
		return false;
	if(vps_num_units_in_tick != vps.vps_num_units_in_tick)
		return false;
	if(vps_time_scale != vps.vps_time_scale)
		return false;
	if(vps_poc_proportional_to_timing_flag != vps.vps_poc_proportional_to_timing_flag)
		return false;
	if(vps_num_ticks_poc_diff_one_minus1 != vps.vps_num_ticks_poc_diff_one_minus1)
		return false;
	if(vps_num_hrd_parameters != vps.vps_num_hrd_parameters)
		return false;
	if(hrd_layer_set_idx!= vps.hrd_layer_set_idx)
		return false;
	if(cprms_present_flag != vps.cprms_present_flag)
		return false;
	if(!(hrd_parameters == vps.hrd_parameters))
		return false;
	if(vps_extension_flag != vps.vps_extension_flag)
		return false;
	return true;
}

SPS::SPS(): NALUnit(NAL_SPS)
{
	Default();
}
void SPS::Default()
{
	sps_video_parameter_set_id = 0;
	sps_max_sub_layers_minus1 = 0;
	sps_temporal_id_nesting_flag = 0;
	profile_tier_level.Default();
	sps_seq_parameter_set_id = 0;
	chroma_format_idc = 0;
	separate_colour_plane_flag = 0;
	pic_width_in_luma_samples = 0;
	pic_height_in_luma_samples = 0;
	conformance_window_flag = 0;
	conf_win_left_offset = 0;
	conf_win_right_offset = 0;
	conf_win_top_offset = 0;
	conf_win_bottom_offset = 0;
	bit_depth_luma_minus8 = 0;
	bit_depth_chroma_minus8 = 0;
	log2_max_pic_order_cnt_lsb_minus4 = 0;
	sps_sub_layer_ordering_info_present_flag = 0;
	sps_max_dec_pic_buffering_minus1.clear();
	sps_max_num_reorder_pics.clear();
	sps_max_latency_increase_plus1.clear();
	log2_min_luma_coding_block_size_minus3 = 0;
	log2_diff_max_min_luma_coding_block_size = 0;
	log2_min_transform_block_size_minus2 = 0;
	log2_diff_max_min_transform_block_size = 0;
	max_transform_hierarchy_depth_inter = 0;
	max_transform_hierarchy_depth_intra = 0;
	scaling_list_enabled_flag = 0;
	scaling_list_data.Default();
	sps_scaling_list_data_present_flag = 0;
	amp_enabled_flag = 0;
	sample_adaptive_offset_enabled_flag = 0;
	pcm_enabled_flag = 0;
	pcm_sample_bit_depth_luma_minus1 = 0;
	pcm_sample_bit_depth_chroma_minus1 = 0;
	log2_min_pcm_luma_coding_block_size_minus3 = 0;
	log2_diff_max_min_pcm_luma_coding_block_size = 0;
	pcm_loop_filter_disabled_flag = 0;
	num_short_term_ref_pic_sets = 0;
	short_term_ref_pic_set.clear();
	long_term_ref_pics_present_flag = 0;
	num_long_term_ref_pics_sps = 0;
	lt_ref_pic_poc_lsb_sps.clear();
	used_by_curr_pic_lt_sps_flag.clear();
	sps_temporal_mvp_enabled_flag = 0;
	strong_intra_smoothing_enabled_flag = 0;
	vui_parameters_present_flag = 0;
	vui_parameters.Default();
	sps_extension_flag = 0;
}
bool SPS::operator == (const SPS &sps) const
{
	if(this == &sps)
		return true;
	if(sps_video_parameter_set_id != sps.sps_video_parameter_set_id)
		return false;
	if(sps_max_sub_layers_minus1 != sps.sps_max_sub_layers_minus1)
		return false;
	if(sps_temporal_id_nesting_flag != sps.sps_temporal_id_nesting_flag)
		return false;
	if(!(profile_tier_level == sps.profile_tier_level))
		return false;
	if(sps_seq_parameter_set_id != sps.sps_seq_parameter_set_id)
		return false;
	if(chroma_format_idc != sps.chroma_format_idc)
		return false;
	if(separate_colour_plane_flag != sps.separate_colour_plane_flag)
		return false;
	if(pic_width_in_luma_samples != sps.pic_width_in_luma_samples)
		return false;
	if(pic_height_in_luma_samples != sps.pic_height_in_luma_samples)
		return false;
	if(conformance_window_flag != sps.conformance_window_flag)
		return false;
	if(conf_win_left_offset != sps.conf_win_left_offset)
		return false;
	if(conf_win_right_offset != sps.conf_win_right_offset)
		return false;
	if(conf_win_top_offset != sps.conf_win_top_offset)
		return false;
	if(conf_win_bottom_offset != sps.conf_win_bottom_offset)
		return false;
	if(bit_depth_luma_minus8 != sps.bit_depth_luma_minus8)
		return false;
	if(bit_depth_chroma_minus8 != sps.bit_depth_chroma_minus8)
		return false;
	if(log2_max_pic_order_cnt_lsb_minus4 != sps.log2_max_pic_order_cnt_lsb_minus4)
		return false;
	if(sps_sub_layer_ordering_info_present_flag != sps.sps_sub_layer_ordering_info_present_flag)
		return false;
	if(sps_max_dec_pic_buffering_minus1 != sps.sps_max_dec_pic_buffering_minus1)
		return false;
	if(sps_max_num_reorder_pics != sps.sps_max_num_reorder_pics)
		return false;
	if(sps_max_latency_increase_plus1 != sps.sps_max_latency_increase_plus1)
		return false;
	if(log2_min_luma_coding_block_size_minus3 != sps.log2_min_luma_coding_block_size_minus3)
		return false;
	if(log2_diff_max_min_luma_coding_block_size != sps.log2_diff_max_min_luma_coding_block_size)
		return false;
	if(log2_min_transform_block_size_minus2 != sps.log2_min_transform_block_size_minus2)
		return false;
	if(log2_diff_max_min_transform_block_size != sps.log2_diff_max_min_transform_block_size)
		return false;
	if(max_transform_hierarchy_depth_inter != sps.max_transform_hierarchy_depth_inter)
		return false;
	if(max_transform_hierarchy_depth_intra != sps.max_transform_hierarchy_depth_intra)
		return false;
	if(scaling_list_enabled_flag != sps.scaling_list_enabled_flag)
		return false;
	if(!(scaling_list_data == sps.scaling_list_data))
		return false;
	if(sps_scaling_list_data_present_flag != sps.sps_scaling_list_data_present_flag)
		return false;
	if(amp_enabled_flag != sps.amp_enabled_flag)
		return false;
	if(sample_adaptive_offset_enabled_flag != sps.sample_adaptive_offset_enabled_flag)
		return false;
	if(pcm_enabled_flag != sps.pcm_enabled_flag)
		return false;
	if(num_short_term_ref_pic_sets != sps.num_short_term_ref_pic_sets)
		return false;
	if(short_term_ref_pic_set != sps.short_term_ref_pic_set)
		return false;
	if(long_term_ref_pics_present_flag != sps.long_term_ref_pics_present_flag)
		return false;
	if(num_long_term_ref_pics_sps != sps.num_long_term_ref_pics_sps)
		return false;
	if(lt_ref_pic_poc_lsb_sps != sps.lt_ref_pic_poc_lsb_sps)
		return false;
	if(used_by_curr_pic_lt_sps_flag != sps.used_by_curr_pic_lt_sps_flag)
		return false;
	if(sps_temporal_mvp_enabled_flag != sps.sps_temporal_mvp_enabled_flag)
		return false;
	if(strong_intra_smoothing_enabled_flag != sps.strong_intra_smoothing_enabled_flag)
		return false;
	if(vui_parameters_present_flag != sps.vui_parameters_present_flag)
		return false;
	if(!(vui_parameters == sps.vui_parameters))
		return false;
	if(sps_extension_flag != sps.sps_extension_flag)
		return false;
	return true;
}

PPS::PPS(): NALUnit(NAL_PPS)
{
	Default();
};
void PPS::Default()
{
	pps_pic_parameter_set_id = 0;
	pps_seq_parameter_set_id = 0;
	dependent_slice_segments_enabled_flag = 0;
	output_flag_present_flag = 0;
	num_extra_slice_header_bits = 0;
	sign_data_hiding_flag = 0;
	cabac_init_present_flag = 0;
	num_ref_idx_l0_default_active_minus1 = 0;
	num_ref_idx_l1_default_active_minus1 = 0;
	init_qp_minus26 = 0;
	constrained_intra_pred_flag = 0;
	transform_skip_enabled_flag = 0;
	cu_qp_delta_enabled_flag = 0;
	diff_cu_qp_delta_depth = 0;
	pps_cb_qp_offset = 0;
	pps_cr_qp_offset = 0;
	pps_slice_chroma_qp_offsets_present_flag = 0;
	weighted_pred_flag = 0;
	weighted_bipred_flag = 0;
	transquant_bypass_enabled_flag = 0;
	tiles_enabled_flag = 0;
	entropy_coding_sync_enabled_flag = 0;
	num_tile_columns_minus1 = 0;
	num_tile_rows_minus1 = 0;
	uniform_spacing_flag = 1;
	column_width_minus1.clear();
	row_height_minus1.clear();
	loop_filter_across_tiles_enabled_flag = 0;
	pps_loop_filter_across_slices_enabled_flag = 0;
	deblocking_filter_control_present_flag = 0;
	deblocking_filter_override_enabled_flag = 0;
	pps_deblocking_filter_disabled_flag = 0;
	pps_beta_offset_div2 = 0;
	pps_tc_offset_div2 = 0;
	pps_scaling_list_data_present_flag = 0;
	lists_modification_present_flag = 0;
	log2_parallel_merge_level_minus2 = 0;
	slice_segment_header_extension_present_flag = 0;
	pps_extension_flag = 0;
}
bool PPS::operator == (const PPS &pps) const
{
	if(this == &pps)
		return true;
	if(pps_pic_parameter_set_id != pps.pps_pic_parameter_set_id)
		return false;
	if(pps_seq_parameter_set_id != pps.pps_seq_parameter_set_id)
		return false;
	if(dependent_slice_segments_enabled_flag != pps.dependent_slice_segments_enabled_flag)
		return false;
	if(output_flag_present_flag != pps.output_flag_present_flag)
		return false;
	if(num_extra_slice_header_bits != pps.num_extra_slice_header_bits)
		return false;
	if(sign_data_hiding_flag != pps.sign_data_hiding_flag)
		return false;
	if(cabac_init_present_flag != pps.cabac_init_present_flag)
		return false;
	if(num_ref_idx_l0_default_active_minus1 != pps.num_ref_idx_l0_default_active_minus1)
		return false;
	if(num_ref_idx_l1_default_active_minus1 != pps.num_ref_idx_l1_default_active_minus1)
		return false;
	if(init_qp_minus26 != pps.init_qp_minus26)
		return false;
	if(constrained_intra_pred_flag != pps.constrained_intra_pred_flag)
		return false;
	if(transform_skip_enabled_flag != pps.transform_skip_enabled_flag)
		return false;
	if(cu_qp_delta_enabled_flag != pps.cu_qp_delta_enabled_flag)
		return false;
	if(diff_cu_qp_delta_depth != pps.diff_cu_qp_delta_depth)
		return false;
	if(pps_cb_qp_offset != pps.pps_cb_qp_offset)
		return false;
	if(pps_cr_qp_offset != pps.pps_cr_qp_offset)
		return false;
	if(pps_slice_chroma_qp_offsets_present_flag != pps.pps_slice_chroma_qp_offsets_present_flag)
		return false;
	if(weighted_pred_flag != pps.weighted_pred_flag)
		return false;
	if(weighted_bipred_flag != pps.weighted_bipred_flag)
		return false;
	if(transquant_bypass_enabled_flag != pps.transquant_bypass_enabled_flag)
		return false;
	if(tiles_enabled_flag != pps.tiles_enabled_flag)
		return false;
	if(entropy_coding_sync_enabled_flag != pps.entropy_coding_sync_enabled_flag)
		return false;
	if(num_tile_columns_minus1 != pps.num_tile_columns_minus1)
		return false;
	if(num_tile_rows_minus1 != pps.num_tile_rows_minus1)
		return false;
	if(uniform_spacing_flag != pps.uniform_spacing_flag)
		return false;
	if(column_width_minus1 != pps.column_width_minus1)
		return false;
	if(row_height_minus1 != pps.row_height_minus1)
		return false;
	if(loop_filter_across_tiles_enabled_flag != pps.loop_filter_across_tiles_enabled_flag)
		return false;
	if(pps_loop_filter_across_slices_enabled_flag != pps.pps_loop_filter_across_slices_enabled_flag)
		return false;
	if(deblocking_filter_control_present_flag != pps.deblocking_filter_control_present_flag)
		return false;
	if(deblocking_filter_override_enabled_flag != pps.deblocking_filter_override_enabled_flag)
		return false;
	if(pps_deblocking_filter_disabled_flag != pps.pps_deblocking_filter_disabled_flag)
		return false;
	if(pps_beta_offset_div2 != pps.pps_beta_offset_div2)
		return false;
	if(pps_tc_offset_div2 != pps.pps_tc_offset_div2)
		return false;
	if(pps_scaling_list_data_present_flag != pps_scaling_list_data_present_flag)
		return false;
	if(lists_modification_present_flag != lists_modification_present_flag)
		return false;
	if(log2_parallel_merge_level_minus2 != log2_parallel_merge_level_minus2)
		return false;
	if(slice_segment_header_extension_present_flag != slice_segment_header_extension_present_flag)
		return false;
	if(pps_extension_flag != pps_extension_flag)
		return false;
	return true;
}

Slice::Slice(NALUnitType type): NALUnit(type) 
{
	Default();
};
void Slice::Default()
{
	first_slice_segment_in_pic_flag = 0;
	no_output_of_prior_pics_flag = 0;
	slice_pic_parameter_set_id = 0;
	dependent_slice_segment_flag = 0;
	slice_segment_address = 0;
	slice_reserved_undetermined_flag.clear();
	slice_type = 0;
	pic_output_flag = 1;
	colour_plane_id = 0;
	pic_order_cnt_lsb = 0;
	short_term_ref_pic_set_sps_flag = 0;
	short_term_ref_pic_set_idx = 0;
	num_ref_idx_l0_active_minus1 = 0;
	num_ref_idx_l1_active_minus1 = 0;
	slice_temporal_mvp_enabled_flag = 0;
	collocated_from_l0_flag = 1;
	deblocking_filter_override_flag = 0;
	slice_sao_luma_flag = 0;
	slice_sao_chroma_flag = 0;
	num_long_term_pics = 0;
	num_long_term_sps = 0;
}
