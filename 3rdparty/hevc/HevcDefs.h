#ifndef _HEVC_DEFS_H_
#define _HEVC_DEFS_H_

#include <vector>
#include <memory>

enum NALUnitType
{
	NAL_TRAIL_N    = 0,
	NAL_TRAIL_R    = 1,
	NAL_TSA_N      = 2,
	NAL_TSA_R      = 3,
	NAL_STSA_N     = 4,
	NAL_STSA_R     = 5,
	NAL_RADL_N     = 6,
	NAL_RADL_R     = 7,
	NAL_RASL_N     = 8,
	NAL_RASL_R     = 9,
	NAL_BLA_W_LP   = 16,//<----
	NAL_BLA_W_RADL = 17,
	NAL_BLA_N_LP   = 18,
	NAL_IDR_W_RADL = 19,
	NAL_IDR_N_LP   = 20,
	NAL_CRA_NUT    = 21,
	NAL_IRAP_VCL23 = 23,//<---- 
	NAL_VPS        = 32,
	NAL_SPS        = 33,
	NAL_PPS        = 34,
	NAL_AUD        = 35,
	NAL_EOS_NUT    = 36,
	NAL_EOB_NUT    = 37,
	NAL_FD_NUT     = 38,
	NAL_SEI_PREFIX = 39,
	NAL_SEI_SUFFIX = 40,
};

class ProfileTierLevel
{
public:
	uint8				general_profile_space;
	uint8				general_tier_flag;
	uint8				general_profile_idc;
	uint8				general_profile_compatibility_flag[32];
	uint8				general_progressive_source_flag;
	uint8				general_interlaced_source_flag;
	uint8				general_non_packed_constraint_flag;
	uint8				general_frame_only_constraint_flag;
	uint8				general_level_idc;
	std::vector<uint8>	sub_layer_profile_present_flag;
	std::vector<uint8>	sub_layer_level_present_flag;
	std::vector<uint8>	sub_layer_profile_space;
	std::vector<uint8>	sub_layer_tier_flag;
	std::vector<uint8>	sub_layer_profile_idc;
	std::vector< std::vector< uint8> > sub_layer_profile_compatibility_flag;
	std::vector<uint8>	sub_layer_progressive_source_flag;
	std::vector<uint8>	sub_layer_interlaced_source_flag;
	std::vector<uint8>	sub_layer_non_packed_constraint_flag;
	std::vector<uint8>	sub_layer_frame_only_constraint_flag;
	std::vector<uint8>	sub_layer_level_idc;
	
	inline void Default()
	{
		general_profile_space = 0;  
		general_tier_flag = 0;
		general_profile_idc = 0;
		general_profile_compatibility_flag[32];
		general_progressive_source_flag = 0;
		general_interlaced_source_flag = 0;
		general_non_packed_constraint_flag = 0;
		general_frame_only_constraint_flag = 0;
		general_level_idc = 0;
		sub_layer_profile_present_flag.clear();
		sub_layer_level_present_flag.clear();
		sub_layer_profile_space.clear();
		sub_layer_tier_flag.clear();
		sub_layer_profile_idc.clear();
		sub_layer_profile_compatibility_flag.clear();
		sub_layer_progressive_source_flag.clear();
		sub_layer_interlaced_source_flag.clear();
		sub_layer_non_packed_constraint_flag.clear();
		sub_layer_frame_only_constraint_flag.clear();
		sub_layer_level_idc.clear();
	}
	inline bool operator == (const ProfileTierLevel &ptl) const
	{
		if(this == &ptl)
			return true;
		if(general_profile_space != ptl.general_profile_space)
			return false;
		if(general_tier_flag != ptl.general_tier_flag)
			return false;
		if(general_profile_idc != ptl.general_profile_idc)
			return false;
		if(general_progressive_source_flag != ptl.general_progressive_source_flag)
			return false;
		if(general_interlaced_source_flag != ptl.general_interlaced_source_flag)
			return false;
		if(general_non_packed_constraint_flag != ptl.general_non_packed_constraint_flag)
			return false;
		if(general_frame_only_constraint_flag != ptl.general_frame_only_constraint_flag)
			return false;
		if(general_level_idc != ptl.general_level_idc)
			return false;
		if(sub_layer_profile_present_flag != ptl.sub_layer_profile_present_flag)
			return false;
		if(sub_layer_level_present_flag != ptl.sub_layer_level_present_flag)
			return false;
		if(memcmp(general_profile_compatibility_flag, ptl.general_profile_compatibility_flag, 32) != 0)
			return false;
		if(sub_layer_profile_space != ptl.sub_layer_profile_space)
			return false;
		if(sub_layer_tier_flag != ptl.sub_layer_tier_flag)
			return false;
		if(sub_layer_profile_idc != ptl.sub_layer_profile_idc)
			return false;
		if(sub_layer_profile_compatibility_flag != ptl.sub_layer_profile_compatibility_flag)
			return false;
		if(sub_layer_progressive_source_flag != ptl.sub_layer_progressive_source_flag)
			return false;
		if(sub_layer_interlaced_source_flag != ptl.sub_layer_interlaced_source_flag)
			return false;
		if(sub_layer_non_packed_constraint_flag != ptl.sub_layer_non_packed_constraint_flag)
			return false;
		if(sub_layer_frame_only_constraint_flag != ptl.sub_layer_frame_only_constraint_flag)
			return false;
		if(sub_layer_level_idc != ptl.sub_layer_level_idc)
			return false;
		return true;
	}
};

class SubLayerHrdParameters
{
public:
	std::vector<uint32>	bit_rate_value_minus1;
	std::vector<uint32>	cpb_size_value_minus1;
	std::vector<uint32>	cpb_size_du_value_minus1;
	std::vector<uint32>	bit_rate_du_value_minus1;
	std::vector<uint8>	cbr_flag;
	
	inline void Default()
	{
		bit_rate_value_minus1.clear();
		cpb_size_value_minus1.clear();
		cpb_size_du_value_minus1.clear();
		bit_rate_du_value_minus1.clear();
		cbr_flag.clear();
	}
	inline bool operator == (const SubLayerHrdParameters &slhrd) const
	{
		if(this == &slhrd)
			return true;
		if(bit_rate_value_minus1 != slhrd.bit_rate_value_minus1)
			return false;
		if(cpb_size_value_minus1 != slhrd.cpb_size_value_minus1)
			return false;
		if(cpb_size_du_value_minus1 != slhrd.cpb_size_du_value_minus1)
			return false;
		if(bit_rate_du_value_minus1 != slhrd.bit_rate_du_value_minus1)
			return false;
		if(cbr_flag != slhrd.cbr_flag)
			return false;
		return true;
	}
};

class ScalingListData
{
public:
	std::vector< std::vector< uint8> >	scaling_list_pred_mode_flag;
	std::vector< std::vector< uint32> >	scaling_list_pred_matrix_id_delta;
	std::vector< std::vector< uint32> >	scaling_list_dc_coef_minus8;
	std::vector<std::vector< std::vector< uint32> > > scaling_list_delta_coef;
	
	inline void Default()
	{
		scaling_list_delta_coef.clear();
		scaling_list_pred_mode_flag.clear();
		scaling_list_pred_matrix_id_delta.clear();
		scaling_list_dc_coef_minus8.clear();
	}
	inline bool operator == (const ScalingListData &obj) const
	{
		if(this == &obj)
			return true;
		if(scaling_list_pred_mode_flag != obj.scaling_list_pred_mode_flag)
			return false;
		if(scaling_list_pred_matrix_id_delta != obj.scaling_list_pred_matrix_id_delta)
			return false;
		if(scaling_list_dc_coef_minus8 != obj.scaling_list_dc_coef_minus8)
			return false;
		if(scaling_list_delta_coef != obj.scaling_list_delta_coef)
			return false;
		return true;
	}
};

class HrdParameters
{
public:
	uint8				nal_hrd_parameters_present_flag;
	uint8				vcl_hrd_parameters_present_flag;
	uint8				sub_pic_hrd_params_present_flag;
	uint8				tick_divisor_minus2;
	uint8				du_cpb_removal_delay_increment_length_minus1;
	uint8				sub_pic_cpb_params_in_pic_timing_sei_flag;
	uint8				dpb_output_delay_du_length_minus1;
	uint8				bit_rate_scale;
	uint8				cpb_size_scale;
	uint8				cpb_size_du_scale;
	uint8				initial_cpb_removal_delay_length_minus1;
	uint8				au_cpb_removal_delay_length_minus1;
	uint8				dpb_output_delay_length_minus1;
	std::vector<uint8>	fixed_pic_rate_general_flag;
	std::vector<uint8>	fixed_pic_rate_within_cvs_flag;
	std::vector<uint32>	elemental_duration_in_tc_minus1;
	std::vector<uint8>	low_delay_hrd_flag;
	std::vector<uint32>	cpb_cnt_minus1;
	std::vector<SubLayerHrdParameters> nal_sub_layer_hrd_parameters;
	std::vector<SubLayerHrdParameters> vcl_sub_layer_hrd_parameters;
	
	inline void Default()
	{
		nal_hrd_parameters_present_flag = 0;
		vcl_hrd_parameters_present_flag = 0;
		sub_pic_hrd_params_present_flag = 0;
		tick_divisor_minus2 = 0;
		du_cpb_removal_delay_increment_length_minus1 = 0;
		sub_pic_cpb_params_in_pic_timing_sei_flag = 0;
		dpb_output_delay_du_length_minus1 = 0;
		bit_rate_scale = 0;
		cpb_size_scale = 0;
		cpb_size_du_scale = 0;
		initial_cpb_removal_delay_length_minus1 = 23;
		au_cpb_removal_delay_length_minus1 = 23;
		dpb_output_delay_length_minus1 = 23;
		fixed_pic_rate_general_flag.clear();
		fixed_pic_rate_within_cvs_flag.clear();
		elemental_duration_in_tc_minus1.clear();
		low_delay_hrd_flag.clear();
		cpb_cnt_minus1.clear();
		nal_sub_layer_hrd_parameters.clear();
		vcl_sub_layer_hrd_parameters.clear();
	}
	inline bool operator == (const HrdParameters &hrd) const
	{
		if(this == &hrd)
			return true;
		if(nal_hrd_parameters_present_flag != hrd.nal_hrd_parameters_present_flag)
			return false;
		if(vcl_hrd_parameters_present_flag != hrd.vcl_hrd_parameters_present_flag)
			return false;
		if(sub_pic_hrd_params_present_flag != hrd.sub_pic_hrd_params_present_flag)
			return false;
		if(tick_divisor_minus2 != hrd.tick_divisor_minus2)
			return false;
		if(du_cpb_removal_delay_increment_length_minus1 != hrd.du_cpb_removal_delay_increment_length_minus1)
			return false;
		if(sub_pic_cpb_params_in_pic_timing_sei_flag != hrd.sub_pic_cpb_params_in_pic_timing_sei_flag)
			return false;
		if(dpb_output_delay_du_length_minus1 != hrd.dpb_output_delay_du_length_minus1)
			return false;
		if(bit_rate_scale != hrd.bit_rate_scale)
			return false;
		if(cpb_size_scale != hrd.cpb_size_scale)
			return false;
		if(cpb_size_du_scale != hrd.cpb_size_du_scale)
			return false;
		if(initial_cpb_removal_delay_length_minus1 != hrd.initial_cpb_removal_delay_length_minus1)
			return false;
		if(au_cpb_removal_delay_length_minus1 != hrd.au_cpb_removal_delay_length_minus1)
			return false;
		if(dpb_output_delay_length_minus1 != hrd.dpb_output_delay_length_minus1)
			return false;
		if(fixed_pic_rate_general_flag != hrd.fixed_pic_rate_general_flag)
			return false;
		if(fixed_pic_rate_within_cvs_flag != hrd.fixed_pic_rate_within_cvs_flag)
			return false;
		if(elemental_duration_in_tc_minus1 != hrd.elemental_duration_in_tc_minus1)
			return false;
		if(low_delay_hrd_flag != hrd.low_delay_hrd_flag)
			return false;
		if(cpb_cnt_minus1 != hrd.cpb_cnt_minus1)
			return false;
		if(!(nal_sub_layer_hrd_parameters == hrd.nal_sub_layer_hrd_parameters))
			return false;
		if(!(vcl_sub_layer_hrd_parameters == hrd.vcl_sub_layer_hrd_parameters))
			return false;
		return true;
	}
};

class ShortTermRefPicSet
{
public:
	uint8					inter_ref_pic_set_prediction_flag;
	uint32					delta_idx_minus1;
	uint8					delta_rps_sign;
	uint32					abs_delta_rps_minus1;
	std::vector<uint8>		used_by_curr_pic_flag;
	std::vector<uint8>		use_delta_flag;
	uint32					num_negative_pics;
	uint32					num_positive_pics;
	std::vector<uint32>		delta_poc_s0_minus1;
	std::vector<uint8>		used_by_curr_pic_s0_flag;
	std::vector<uint32>		delta_poc_s1_minus1;
	std::vector<uint8>		used_by_curr_pic_s1_flag;
	
	inline void Default()
	{
		inter_ref_pic_set_prediction_flag = 0;
		delta_idx_minus1 = 0;
		delta_rps_sign = 0;
		abs_delta_rps_minus1 = 0;
		used_by_curr_pic_flag.clear();
		use_delta_flag.clear();
		num_negative_pics = 0;
		num_positive_pics = 0;
		delta_poc_s0_minus1.clear();
		used_by_curr_pic_s0_flag.clear();
		delta_poc_s1_minus1.clear();
		used_by_curr_pic_s1_flag.clear();
	}
	inline bool operator == (const ShortTermRefPicSet &strps) const
	{
		if(this == &strps)
			return true;
		if(inter_ref_pic_set_prediction_flag != strps.inter_ref_pic_set_prediction_flag)
			return false;
		if(delta_idx_minus1 != strps.delta_idx_minus1)
			return false;
		if(delta_rps_sign != strps.delta_rps_sign)
			return false;
		if(abs_delta_rps_minus1 != strps.abs_delta_rps_minus1)
			return false;
		if(used_by_curr_pic_flag != strps.used_by_curr_pic_flag)
			return false;
		if(use_delta_flag != strps.use_delta_flag)
			return false;
		if(num_negative_pics != strps.num_negative_pics)
			return false;
		if(num_positive_pics != strps.num_positive_pics)
			return false;
		if(delta_poc_s0_minus1 != strps.delta_poc_s0_minus1)
			return false;
		if(used_by_curr_pic_s0_flag != strps.used_by_curr_pic_s0_flag)
			return false;
		if(delta_poc_s1_minus1 != strps.delta_poc_s1_minus1)
			return false;
		if(used_by_curr_pic_s1_flag != strps.used_by_curr_pic_s1_flag)
			return false;
		return true;
	}
};

class RefPicListModification
{
public:
	uint8				ref_pic_list_modification_flag_l0;
	std::vector<uint32>	list_entry_l0;
	uint8				ref_pic_list_modification_flag_l1;
	std::vector<uint32>	list_entry_l1;
	
	inline void Default()
	{
		ref_pic_list_modification_flag_l0 = 0;
		list_entry_l0.clear();
		ref_pic_list_modification_flag_l1 = 0;
		list_entry_l1.clear();
	}
	inline bool operator == (const RefPicListModification &obj) const
	{
		if(this == &obj)
			return true;
		if(ref_pic_list_modification_flag_l0 != obj.ref_pic_list_modification_flag_l0)
			return false;
		if(list_entry_l0 != obj.list_entry_l0)
			return false;
		if(ref_pic_list_modification_flag_l1 != obj.ref_pic_list_modification_flag_l1)
			return false;
		if(list_entry_l1 != obj.list_entry_l1)
			return false;
		return true;
	}
};

class VuiParameters
{
public:
	uint8			aspect_ratio_info_present_flag;
	uint8			aspect_ratio_idc;
	uint16			sar_width;
	uint16			sar_height;
	uint8			overscan_info_present_flag;
	uint8			overscan_appropriate_flag;
	uint8			video_signal_type_present_flag;
	uint8			video_format;
	uint8			video_full_range_flag;
	uint8			colour_description_present_flag;
	uint8			colour_primaries;
	uint8			transfer_characteristics;
	uint8			matrix_coeffs;
	uint8			chroma_loc_info_present_flag;
	uint32			chroma_sample_loc_type_top_field;
	uint32			chroma_sample_loc_type_bottom_field;
	uint8			neutral_chroma_indication_flag;
	uint8			field_seq_flag;
	uint8			frame_field_info_present_flag;
	uint8			default_display_window_flag;
	uint32			def_disp_win_left_offset;
	uint32			def_disp_win_right_offset;
	uint32			def_disp_win_top_offset;
	uint32			def_disp_win_bottom_offset;
	uint8			vui_timing_info_present_flag;
	uint32			vui_num_units_in_tick;
	uint32			vui_time_scale;
	uint8			vui_poc_proportional_to_timing_flag;
	uint32			vui_num_ticks_poc_diff_one_minus1;
	uint8			vui_hrd_parameters_present_flag;
	HrdParameters	hrd_parameters;
	uint8			bitstream_restriction_flag;
	uint8			tiles_fixed_structure_flag;
	uint8			motion_vectors_over_pic_boundaries_flag;
	uint8			restricted_ref_pic_lists_flag;
	uint32			min_spatial_segmentation_idc;
	uint32			max_bytes_per_pic_denom;
	uint32			max_bits_per_min_cu_denom;
	uint32			log2_max_mv_length_horizontal;
	uint32			log2_max_mv_length_vertical;
	
	inline void Default()
	{
		aspect_ratio_info_present_flag = 0;
		aspect_ratio_idc = 0;
		sar_width = 0;
		sar_height = 0;
		overscan_info_present_flag = 0;
		overscan_appropriate_flag = 0;
		video_signal_type_present_flag = 0;
		video_format = 5;
		video_full_range_flag = 0;
		colour_description_present_flag = 0;
		colour_primaries = 2;
		transfer_characteristics = 2;
		matrix_coeffs = 2;
		chroma_loc_info_present_flag = 0;
		chroma_sample_loc_type_top_field = 0;
		chroma_sample_loc_type_bottom_field = 0;
		neutral_chroma_indication_flag = 0;
		field_seq_flag = 0;
		frame_field_info_present_flag = 0;
		default_display_window_flag = 0;
		def_disp_win_left_offset = 0;
		def_disp_win_right_offset = 0;
		def_disp_win_top_offset = 0;
		def_disp_win_bottom_offset = 0;
		vui_timing_info_present_flag = 0;
		vui_num_units_in_tick = 0;
		vui_time_scale = 0;
		vui_poc_proportional_to_timing_flag = 0;
		vui_num_ticks_poc_diff_one_minus1 = 0;
		vui_hrd_parameters_present_flag = 0;
		hrd_parameters.Default();
		bitstream_restriction_flag = 0;
		tiles_fixed_structure_flag = 0;
		motion_vectors_over_pic_boundaries_flag = 0;
		restricted_ref_pic_lists_flag = 0;
		min_spatial_segmentation_idc = 0;
		max_bytes_per_pic_denom = 2;
		max_bits_per_min_cu_denom = 1;
		log2_max_mv_length_horizontal = 15;
		log2_max_mv_length_vertical = 15;  
	}
	inline bool operator == (const VuiParameters &vui) const
	{
		if(this == &vui)
			return true;
		if(aspect_ratio_info_present_flag != vui.aspect_ratio_info_present_flag)
			return false;
		if(aspect_ratio_idc != vui.aspect_ratio_idc)
			return false;
		if(sar_width != vui.sar_width)
			return false;
		if(sar_height != vui.sar_height)
			return false;
		if(overscan_info_present_flag != vui.overscan_info_present_flag)
			return false;
		if(overscan_appropriate_flag != vui.overscan_appropriate_flag)
			return false;
		if(video_signal_type_present_flag != vui.video_signal_type_present_flag)
			return false;
		if(video_format != vui.video_format)
			return false;
		if(video_full_range_flag != vui.video_full_range_flag)
			return false;
		if(colour_description_present_flag != vui.colour_description_present_flag)
			return false;
		if(colour_primaries != vui.colour_primaries)
			return false;
		if(transfer_characteristics != vui.transfer_characteristics)
			return false;
		if(matrix_coeffs != vui.matrix_coeffs)
			return false;
		if(chroma_loc_info_present_flag != vui.chroma_loc_info_present_flag)
			return false;
		if(chroma_sample_loc_type_top_field != vui.chroma_sample_loc_type_top_field)
			return false;
		if(chroma_sample_loc_type_bottom_field != vui.chroma_sample_loc_type_bottom_field)
			return false;
		if(neutral_chroma_indication_flag != vui.neutral_chroma_indication_flag)
			return false;
		if(field_seq_flag != vui.field_seq_flag)
			return false;
		if(frame_field_info_present_flag != vui.frame_field_info_present_flag)
			return false;
		if(default_display_window_flag != vui.default_display_window_flag)
			return false;
		if(def_disp_win_left_offset != vui.def_disp_win_left_offset)
			return false;
		if(def_disp_win_right_offset != vui.def_disp_win_right_offset)
			return false;
		if(def_disp_win_top_offset != vui.def_disp_win_top_offset)
			return false;
		if(def_disp_win_bottom_offset != vui.def_disp_win_bottom_offset)
			return false;
		if(vui_timing_info_present_flag != vui.vui_timing_info_present_flag)
			return false;
		if(vui_num_units_in_tick != vui.vui_num_units_in_tick)
			return false;
		if(vui_time_scale != vui.vui_time_scale)
			return false;
		if(vui_poc_proportional_to_timing_flag != vui.vui_poc_proportional_to_timing_flag)
			return false;
		if(vui_num_ticks_poc_diff_one_minus1 != vui.vui_num_ticks_poc_diff_one_minus1)
			return false;
		if(vui_hrd_parameters_present_flag != vui.vui_hrd_parameters_present_flag)
			return false;
		if(!(hrd_parameters == vui.hrd_parameters))
			return false;
		if(bitstream_restriction_flag != vui.bitstream_restriction_flag)
			return false;
		if(tiles_fixed_structure_flag != vui.tiles_fixed_structure_flag)
			return false;
		if(motion_vectors_over_pic_boundaries_flag != vui.motion_vectors_over_pic_boundaries_flag)
			return false;
		if(restricted_ref_pic_lists_flag != vui.restricted_ref_pic_lists_flag)
			return false;
		if(min_spatial_segmentation_idc != vui.min_spatial_segmentation_idc)
			return false;
		if(max_bytes_per_pic_denom != vui.max_bytes_per_pic_denom)
			return false;
		if(max_bits_per_min_cu_denom != vui.max_bits_per_min_cu_denom)
			return false;
		if(log2_max_mv_length_horizontal != vui.log2_max_mv_length_horizontal)
			return false;
		if(log2_max_mv_length_vertical != vui.log2_max_mv_length_vertical)
			return false;
		return true;
	}
};

class PredWeightTable
{
public:
	uint32					luma_log2_weight_denom;
	int32					delta_chroma_log2_weight_denom;

	std::vector<uint8>		luma_weight_l0_flag;
	std::vector<uint8>		chroma_weight_l0_flag;
	std::vector<int32>		delta_luma_weight_l0;
	std::vector<int32>		luma_offset_l0;
	std::vector<int32>		delta_chroma_weight_l0_0;
	std::vector<int32>		delta_chroma_weight_l0_1;
	std::vector<int32>		delta_chroma_offset_l0_0;
	std::vector<int32>		delta_chroma_offset_l0_1;
	//std::vector<std::array<int32, 2> > delta_chroma_weight_l0;
	//std::vector<std::array<int32, 2> > delta_chroma_offset_l0;

	std::vector<uint8>		luma_weight_l1_flag;
	std::vector<uint8>		chroma_weight_l1_flag;
	std::vector<int32>		delta_luma_weight_l1;
	std::vector<int32>		luma_offset_l1;
	std::vector<int32>		delta_chroma_weight_l1_0;
	std::vector<int32>		delta_chroma_weight_l1_1;
	std::vector<int32>		delta_chroma_offset_l1_0;
	std::vector<int32>		delta_chroma_offset_l1_1;
	//std::vector<std::array<int32, 2> > delta_chroma_weight_l1;
	//std::vector<std::array<int32, 2> > delta_chroma_offset_l1;
	
	inline void Default()
	{
		luma_log2_weight_denom = 0;
		delta_chroma_log2_weight_denom = 0;

		luma_weight_l0_flag.clear();
		chroma_weight_l0_flag.clear();
		delta_luma_weight_l0.clear();
		luma_offset_l0.clear();
		delta_chroma_weight_l0_0.clear();
		delta_chroma_weight_l0_1.clear();
		delta_chroma_offset_l0_0.clear();
		delta_chroma_offset_l0_1.clear();
		//delta_chroma_weight_l0.clear();
		//delta_chroma_offset_l0.clear();
		luma_weight_l1_flag.clear();
		chroma_weight_l1_flag.clear();
		delta_luma_weight_l1.clear();
		luma_offset_l1.clear();
		delta_chroma_weight_l1_0.clear();
		delta_chroma_weight_l1_1.clear();
		delta_chroma_offset_l1_0.clear();
		delta_chroma_offset_l1_1.clear();
		//delta_chroma_weight_l1.clear();
		//delta_chroma_offset_l1.clear();
	}
};

class NALUnit
{
public:
	NALUnit(NALUnitType type);
	virtual ~NALUnit();
	virtual NALUnitType getType() const;
	
	bool			m_processFailed;
	NALUnitType		m_nalUnitType;
};

class VPS: public NALUnit
{
public:
	VPS();
	uint8						vps_video_parameter_set_id;
	uint8						vps_max_layers_minus1;
	uint8						vps_max_sub_layers_minus1;
	uint8						vps_temporal_id_nesting_flag;
	ProfileTierLevel			profile_tier_level;
	uint8						vps_sub_layer_ordering_info_present_flag;
	std::vector<uint32>			vps_max_dec_pic_buffering_minus1;
	std::vector<uint32>			vps_max_num_reorder_pics;
	std::vector<uint32>			vps_max_latency_increase_plus1;
	uint8						vps_max_layer_id;
	uint32						vps_num_layer_sets_minus1;
	std::vector<std::vector<uint8> > layer_id_included_flag;
	uint8						vps_timing_info_present_flag;
	uint32						vps_num_units_in_tick;
	uint32						vps_time_scale;
	uint8						vps_poc_proportional_to_timing_flag;
	uint32						vps_num_ticks_poc_diff_one_minus1;
	uint32						vps_num_hrd_parameters;
	std::vector<uint32>			hrd_layer_set_idx;
	std::vector<uint8>			cprms_present_flag;
	std::vector<HrdParameters>	hrd_parameters;
	uint8						vps_extension_flag;
	
	void Default();
	bool operator == (const VPS &) const;
};

class SPS: public NALUnit
{
public:
	SPS();
	uint8					sps_video_parameter_set_id;
	uint8					sps_max_sub_layers_minus1;
	uint8					sps_temporal_id_nesting_flag;
	ProfileTierLevel		profile_tier_level;
	uint32					sps_seq_parameter_set_id;
	uint32					chroma_format_idc;
	uint8					separate_colour_plane_flag;
	uint32					pic_width_in_luma_samples;
	uint32					pic_height_in_luma_samples;
	uint8					conformance_window_flag;
	uint32					conf_win_left_offset;
	uint32					conf_win_right_offset;
	uint32					conf_win_top_offset;
	uint32					conf_win_bottom_offset;
	uint32					bit_depth_luma_minus8;
	uint32					bit_depth_chroma_minus8;
	uint32					log2_max_pic_order_cnt_lsb_minus4;
	uint8					sps_sub_layer_ordering_info_present_flag;
	std::vector<uint32>		sps_max_dec_pic_buffering_minus1;
	std::vector<uint32>		sps_max_num_reorder_pics;
	std::vector<uint32>		sps_max_latency_increase_plus1;
	uint32					log2_min_luma_coding_block_size_minus3;
	uint32					log2_diff_max_min_luma_coding_block_size;
	uint32					log2_min_transform_block_size_minus2;
	uint32					log2_diff_max_min_transform_block_size;
	uint32					max_transform_hierarchy_depth_inter;
	uint32					max_transform_hierarchy_depth_intra;
	uint8					scaling_list_enabled_flag;
	uint8					sps_scaling_list_data_present_flag;
	ScalingListData			scaling_list_data;
	uint8					amp_enabled_flag;
	uint8					sample_adaptive_offset_enabled_flag;
	uint8					pcm_enabled_flag;
	uint8					pcm_sample_bit_depth_luma_minus1;
	uint8					pcm_sample_bit_depth_chroma_minus1;
	uint32					log2_min_pcm_luma_coding_block_size_minus3;
	uint32					log2_diff_max_min_pcm_luma_coding_block_size;
	uint8					pcm_loop_filter_disabled_flag;
	uint32					num_short_term_ref_pic_sets;
	std::vector<ShortTermRefPicSet> short_term_ref_pic_set;
	uint8					long_term_ref_pics_present_flag;
	uint32					num_long_term_ref_pics_sps;
	std::vector<uint32>		lt_ref_pic_poc_lsb_sps;
	std::vector<uint8>		used_by_curr_pic_lt_sps_flag;
	uint8					sps_temporal_mvp_enabled_flag;
	uint8					strong_intra_smoothing_enabled_flag;
	uint8					vui_parameters_present_flag;
	VuiParameters			vui_parameters;
	uint8					sps_extension_flag;

	void Default();
	bool operator == (const SPS &) const;
};

class PPS: public NALUnit
{
public:
	PPS();
	uint32		pps_pic_parameter_set_id;
	uint32		pps_seq_parameter_set_id;
	uint8		dependent_slice_segments_enabled_flag;
	uint8		output_flag_present_flag;
	uint8		num_extra_slice_header_bits;
	uint8		sign_data_hiding_flag;
	uint8		cabac_init_present_flag;
	uint32		num_ref_idx_l0_default_active_minus1;
	uint32		num_ref_idx_l1_default_active_minus1;
	int32		init_qp_minus26;   
	uint8		constrained_intra_pred_flag;
	uint8		transform_skip_enabled_flag;
	uint8		cu_qp_delta_enabled_flag;
	uint32		diff_cu_qp_delta_depth;
	int32		pps_cb_qp_offset;
	int32		pps_cr_qp_offset;
	uint8		pps_slice_chroma_qp_offsets_present_flag;
	uint8		weighted_pred_flag;
	uint8		weighted_bipred_flag;
	uint8		transquant_bypass_enabled_flag;
	uint8		tiles_enabled_flag;
	uint8		entropy_coding_sync_enabled_flag;
	uint32		num_tile_columns_minus1;
	uint32		num_tile_rows_minus1;
	uint8		uniform_spacing_flag;
	std::vector<uint32> column_width_minus1;
	std::vector<uint32> row_height_minus1;
	uint8		loop_filter_across_tiles_enabled_flag;
	uint8		pps_loop_filter_across_slices_enabled_flag;
	uint8		deblocking_filter_control_present_flag;
	uint8		deblocking_filter_override_enabled_flag;
	uint8		pps_deblocking_filter_disabled_flag;
	uint32		pps_beta_offset_div2;
	uint32		pps_tc_offset_div2;
	uint8		pps_scaling_list_data_present_flag;
	ScalingListData scaling_list_data;
	uint8		lists_modification_present_flag;
	int32		log2_parallel_merge_level_minus2;
	uint8		slice_segment_header_extension_present_flag;
	uint8		pps_extension_flag;

	void Default();
	bool operator == (const PPS &) const;
};

class Slice: public NALUnit
{
public:
	enum SliceType
	{
		B_SLICE = 0,
		P_SLICE = 1,
		I_SLICE = 2,
		NONE_SLICE = 3
	};
	
	Slice(NALUnitType type);
	uint8					first_slice_segment_in_pic_flag;
	uint8					no_output_of_prior_pics_flag;
	uint32					slice_pic_parameter_set_id;
	uint8					dependent_slice_segment_flag;
	uint32					slice_segment_address;
	std::vector<uint32>		slice_reserved_undetermined_flag;
	uint32					slice_type;
	uint8					pic_output_flag;
	uint8					colour_plane_id;
	uint32					pic_order_cnt_lsb;
	uint8					short_term_ref_pic_set_sps_flag;
	ShortTermRefPicSet		short_term_ref_pic_set;
	uint8					short_term_ref_pic_set_idx;

	uint32					num_long_term_sps;
	uint32					num_long_term_pics;
	std::vector<uint32>		lt_idx_sps;
	std::vector<uint32>		poc_lsb_lt;
	std::vector<uint8>		used_by_curr_pic_lt_flag;
	std::vector<uint8>		delta_poc_msb_present_flag;
	std::vector<uint32>		delta_poc_msb_cycle_lt;
	
	uint8					slice_temporal_mvp_enabled_flag;
	uint8					slice_sao_luma_flag;
	uint8					slice_sao_chroma_flag;
	uint8					num_ref_idx_active_override_flag;
	uint32					num_ref_idx_l0_active_minus1;
	uint32					num_ref_idx_l1_active_minus1;
	RefPicListModification	ref_pic_lists_modification;
	uint8					mvd_l1_zero_flag;
	uint8					cabac_init_flag;
	uint8					collocated_from_l0_flag;
	uint32					collocated_ref_idx;
	PredWeightTable			pred_weight_table;
	uint32					five_minus_max_num_merge_cand;
	int32					slice_qp_delta;
	int32					slice_cb_qp_offset;
	int32					slice_cr_qp_offset;
	uint8					deblocking_filter_override_flag;
	uint8					slice_deblocking_filter_disabled_flag;
	int32					slice_beta_offset_div2;
	int32					slice_tc_offset_div2;
	int32					slice_loop_filter_across_slices_enabled_flag;
	uint32					num_entry_point_offsets;
	uint32					offset_len_minus1;
	std::vector<uint32>		entry_point_offset_minus1;
	uint32					slice_segment_header_extension_length;
	std::vector<uint8>		slice_segment_header_extension_data_byte;

	void Default();
};

#endif // _HEVC_DEFS_H_
