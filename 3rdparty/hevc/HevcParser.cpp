#include "stdafx.h"
#include "HevcParser.h"

#define SLICE_B 0
#define SLICE_P 1
#define SLICE_I 2

HevcParser::HevcParser()
{
	m_vps = NULL;
	m_sps = NULL;
	m_pps = NULL;
	m_lastSlice = NULL;

	Reset();
}
HevcParser::~HevcParser()
{
	SAFE_DELETE(m_vps);
	SAFE_DELETE(m_sps);
	SAFE_DELETE(m_pps);
	SAFE_DELETE(m_lastSlice);
}

void HevcParser::Reset()
{
	SAFE_DELETE(m_vps);
	SAFE_DELETE(m_sps);
	SAFE_DELETE(m_pps);
	SAFE_DELETE(m_lastSlice);

	m_ptr = NULL;
	m_size = 0;
	m_NumTile = 0;
	m_CountTile = 0;
	m_PacketLength = 0;

	m_b_slice = false;
}
uint32 HevcParser::GetTileNum()
{
	return m_NumTile;
}
uint32 HevcParser::GetResolution()
{
	if(m_sps)
	{
		return (uint32)(
			(m_sps->pic_width_in_luma_samples << 16) |
			m_sps->pic_height_in_luma_samples
			);
	}
	return 0;
}

bool HevcParser::IsTile(uint8* pInput, size_t size, bool b_slice)
{
	if(process(pInput, size) < HEVC_TYPE_PPS)
		return false;
	if(!m_pps) return false;
	if(!m_pps->tiles_enabled_flag) return false;

	if(!m_NumTile || !m_b_slice)
	{
		m_NumTile = (m_pps->num_tile_columns_minus1 + 1) * (m_pps->num_tile_rows_minus1 + 1);
		if(m_NumTile > 1)
		{
			if(b_slice)
			{
				if(m_NumTile == GetSliceNumber(pInput, size, m_NumTile))
					m_NumTile = 1;
				m_b_slice = true;
			}
		}
	}
	if(m_NumTile > 1) return true;

	return false;
}
bool HevcParser::GetData(uint8* pOutput, size_t maxsize)
{
	bool bpkt = false;
	if(m_ptr && m_size && (m_NumTile > 1))
	{
		if(m_lastSlice->first_slice_segment_in_pic_flag)
		{
			if(pOutput)
				memcpy(pOutput, m_ptr, m_size);
			m_PacketLength = m_size;
			m_CountTile = 1;
		}
		else
		{
			if(m_PacketLength + m_size < maxsize)
			{
				if(pOutput)
					memcpy((uint8*)(pOutput + m_PacketLength), m_ptr, m_size);
				m_PacketLength += m_size;
				m_CountTile++;
			
				if(m_CountTile >= m_NumTile)
				{
					m_CountTile = 0;
					bpkt = true;
				}
			}
			else
			{
				m_CountTile = 0;
				m_PacketLength = maxsize;
			}
		}
	}
	m_ptr = NULL; m_size = 0;
	return bpkt;
}
int HevcParser::GetSliceNumber(const uint8 *pdata, size_t size, int max_num)
{
	uint8* buf = (uint8*)pdata;
	int slice_num = 0;

	uint8 start_code[4] = {0x00, 0x00, 0x00, 0x01};
	uint8 start_cod2[3] = {0x00, 0x00, 0x01};

	size_t max_pos = size - 64;
	for(size_t pos = 0; pos < max_pos;)
	{
		bool b_nal_type = false;
		if(!memcmp(start_code, buf, 4)){
			b_nal_type = true;
			buf+=4; pos+=4;
		}
		else if(!memcmp(start_cod2, buf, 3)){
			b_nal_type = true;
			buf+=3; pos+=3;
		}
		else{buf++; pos++;}

		if(b_nal_type)
		{
			uint8 nal_type = ((*buf) >> 1) & 0x3f;
			if(nal_type < NAL_VPS)
			{
				slice_num++;
				if(max_num)
				{
					if(slice_num >= max_num) return slice_num;
				}
			}
		}
	}
	return slice_num;
}

UINT HevcParser::process(const uint8 *pdata, size_t size)
{
	m_ptr = (uint8*)pdata;
	m_size = size;

	size_t max = size < 1024 ? size : 1024;

	UINT uiType = HEVC_TYPE_NONE;
	for(size_t pos = 0; pos + 3 < max;)
	{
		size_t startOffset = 3;
		bool naluFinded = pdata[pos] == 0 && pdata[pos+1] == 0 && pdata[pos+2] == 1;
		
		if(!naluFinded)
		{
			if(size - pos >= 4 &&pdata[pos] == 0 && pdata[pos+1] == 0 && pdata[pos+2] == 0 && pdata[pos+3] == 1)
			{
				naluFinded = true;
				startOffset = 4;
			}
		}
		if(naluFinded)
		{
			uiType = processNALUnit(pdata + pos + startOffset, size - pos - startOffset);
			if(uiType == HEVC_TYPE_SLICE)
			{
				if(!m_lastSlice->first_slice_segment_in_pic_flag)
				{
					if(m_lastSlice->slice_type == 2)
					{
						m_ptr += pos;
						m_size -= pos;
					}
				}
				return uiType;
			}
			pos += 3;
		}
		else pos++;
	}
	return uiType;
}

UINT HevcParser::processNALUnit(const uint8 *pdata, size_t size)
{
	BitstreamReader bs(pdata, size);

	NALUnitType type = processNALUnitHeader(bs);
	
	switch(type)
	{
	case NAL_VPS:
		{
			SAFE_DELETE(m_vps);
			
			m_vps = new VPS;
			processVPS(m_vps, bs);

			return HEVC_TYPE_VPS;
		}
		break;
	case NAL_SPS:
		{
			SAFE_DELETE(m_sps);
			
			m_sps = new SPS;
			processSPS(m_sps, bs);

			return HEVC_TYPE_SPS;
		}
		break;
	case NAL_PPS:
		{
			SAFE_DELETE(m_pps);
			
			m_pps = new PPS;
			processPPS(m_pps, bs);

			return HEVC_TYPE_PPS;
		}
		break;
	case NAL_TRAIL_R:
	case NAL_TRAIL_N:
	case NAL_TSA_N:
	case NAL_TSA_R:
	case NAL_STSA_N:
	case NAL_STSA_R:
	case NAL_BLA_W_LP:
	case NAL_BLA_W_RADL:
	case NAL_BLA_N_LP:
	case NAL_IDR_W_RADL:
	case NAL_IDR_N_LP:
	case NAL_CRA_NUT:
	case NAL_RADL_N:
	case NAL_RADL_R:
	case NAL_RASL_N:
	case NAL_RASL_R:
		{
			SAFE_DELETE(m_lastSlice);
			
			m_lastSlice = new Slice(type);
			processSlice(m_lastSlice, bs);

			return HEVC_TYPE_SLICE;
		}
		break;
	}

	return HEVC_TYPE_NONE;
}

NALUnitType HevcParser::processNALUnitHeader(BitstreamReader &bs)
{
	//forbidden_zero_bit
	bs.getBit();
	
	NALUnitType type = (NALUnitType)bs.getBits(6);
	
	//nuh_layer_id
	bs.getBits(6);
	
	//nuh_temporal_id_plus1
	bs.getBits(3);
	return type;
}

void HevcParser::processVPS(VPS *pvps, BitstreamReader &bs)
{
	pvps -> vps_video_parameter_set_id = bs.getBits(4);
	bs.getBits(2);
	pvps -> vps_max_layers_minus1 = bs.getBits(6);
	pvps -> vps_max_sub_layers_minus1 = bs.getBits(3);
	pvps -> vps_temporal_id_nesting_flag = bs.getBits(1);
	bs.getBits(16);
	
	pvps -> profile_tier_level = processProfileTierLevel(pvps -> vps_max_sub_layers_minus1, bs);
	
	pvps -> vps_sub_layer_ordering_info_present_flag = bs.getBits(1);
	
	pvps -> vps_max_dec_pic_buffering_minus1.resize(pvps -> vps_max_sub_layers_minus1 + 1, 0);
	pvps -> vps_max_num_reorder_pics.resize(pvps -> vps_max_sub_layers_minus1 + 1, 0);
	pvps -> vps_max_latency_increase_plus1.resize(pvps -> vps_max_sub_layers_minus1 + 1, 0);
	
	for(size_t i = (pvps -> vps_sub_layer_ordering_info_present_flag ? 0 : pvps -> vps_max_sub_layers_minus1); i <= pvps -> vps_max_sub_layers_minus1; i++)
	{
		pvps -> vps_max_dec_pic_buffering_minus1[i] = bs.getGolombU();
		pvps -> vps_max_num_reorder_pics[i] = bs.getGolombU();
		pvps -> vps_max_latency_increase_plus1[i] = bs.getGolombU();
	}
	
	pvps -> vps_max_layer_id = bs.getBits(6);
	pvps -> vps_num_layer_sets_minus1 = bs.getGolombU();
	
	pvps -> layer_id_included_flag.resize(pvps -> vps_num_layer_sets_minus1+1);
	
	for(size_t i=1; i<=pvps -> vps_num_layer_sets_minus1; i++)
	{
		pvps -> layer_id_included_flag[i].resize(pvps -> vps_max_layer_id+1);
		for(size_t j=0; j<=pvps -> vps_max_layer_id; j++)
		{
			(pvps -> layer_id_included_flag[i])[j] = bs.getBits(1);
		}
	}
	
	pvps -> vps_timing_info_present_flag = bs.getBits(1);
	if(pvps -> vps_timing_info_present_flag)
	{
		pvps -> vps_num_units_in_tick = bs.getBits(32);
		pvps -> vps_time_scale = bs.getBits(32);
		pvps -> vps_poc_proportional_to_timing_flag = bs.getBits(1);
		
		if(pvps -> vps_poc_proportional_to_timing_flag)
		{
			pvps -> vps_num_ticks_poc_diff_one_minus1 = bs.getBits(1);
		}
		pvps -> vps_num_hrd_parameters = bs.getGolombU();
		
		if(pvps -> vps_num_hrd_parameters > 0)
		{
			pvps -> hrd_layer_set_idx.resize(pvps -> vps_num_hrd_parameters);
			pvps -> cprms_present_flag.resize(pvps -> vps_num_hrd_parameters);
			pvps -> cprms_present_flag[0] = 1;
			
			for(size_t i=0; i<pvps -> vps_num_hrd_parameters; i++)
			{
				pvps -> hrd_layer_set_idx[i] = bs.getGolombU();
				
				if(i > 0)
					pvps -> cprms_present_flag[i] = bs.getBits(1);
				pvps -> hrd_parameters.push_back(processHrdParameters(pvps -> cprms_present_flag[i], pvps -> vps_max_sub_layers_minus1, bs));
			}
		}
	}

	pvps -> vps_extension_flag = bs.getBits(1);
}

void HevcParser::processSPS(SPS *psps, BitstreamReader &bs)
{
	psps -> sps_video_parameter_set_id = bs.getBits(4);
	psps -> sps_max_sub_layers_minus1 = bs.getBits(3);
	psps -> sps_temporal_id_nesting_flag = bs.getBits(1);
	psps -> profile_tier_level = processProfileTierLevel(psps -> sps_max_sub_layers_minus1, bs);

	psps -> sps_seq_parameter_set_id = bs.getGolombU();
	psps -> chroma_format_idc = bs.getGolombU();

	if(psps -> chroma_format_idc == 3)
		psps -> separate_colour_plane_flag = bs.getBits(1);
	else
		psps -> separate_colour_plane_flag = 0;

	psps -> pic_width_in_luma_samples = bs.getGolombU();
	psps -> pic_height_in_luma_samples = bs.getGolombU();
	psps -> conformance_window_flag = bs.getBits(1);

	if(psps -> conformance_window_flag)
	{
		psps -> conf_win_left_offset = bs.getGolombU();
		psps -> conf_win_right_offset = bs.getGolombU();
		psps -> conf_win_top_offset = bs.getGolombU();
		psps -> conf_win_bottom_offset = bs.getGolombU();
	}

	psps -> bit_depth_luma_minus8 = bs.getGolombU();
	psps -> bit_depth_chroma_minus8 = bs.getGolombU();
	psps -> log2_max_pic_order_cnt_lsb_minus4 = bs.getGolombU();
	psps -> sps_sub_layer_ordering_info_present_flag = bs.getBits(1);

	psps -> sps_max_dec_pic_buffering_minus1.resize(psps -> sps_max_sub_layers_minus1 + 1, 0);
	psps -> sps_max_num_reorder_pics.resize(psps -> sps_max_sub_layers_minus1 + 1, 0);
	psps -> sps_max_latency_increase_plus1.resize(psps -> sps_max_sub_layers_minus1 + 1, 0);

	for(size_t i=(psps -> sps_sub_layer_ordering_info_present_flag ? 0 : psps -> sps_max_sub_layers_minus1);
		i<=psps -> sps_max_sub_layers_minus1;
		i++)
	{
		psps -> sps_max_dec_pic_buffering_minus1[i] = bs.getGolombU();
		psps -> sps_max_num_reorder_pics[i] = bs.getGolombU();
		psps -> sps_max_latency_increase_plus1[i] = bs.getGolombU();
	}

	psps -> log2_min_luma_coding_block_size_minus3 = bs.getGolombU();
	psps -> log2_diff_max_min_luma_coding_block_size = bs.getGolombU();
	psps -> log2_min_transform_block_size_minus2 = bs.getGolombU();
	psps -> log2_diff_max_min_transform_block_size = bs.getGolombU();
	psps -> max_transform_hierarchy_depth_inter = bs.getGolombU();
	psps -> max_transform_hierarchy_depth_intra = bs.getGolombU();

	psps -> scaling_list_enabled_flag = bs.getBits(1);
	if(psps -> scaling_list_enabled_flag)
	{
		psps -> sps_scaling_list_data_present_flag = bs.getBits(1);
		if(psps -> sps_scaling_list_data_present_flag)
		{
			psps -> scaling_list_data = processScalingListData(bs);
		}
	}

	psps -> amp_enabled_flag = bs.getBits(1);
	psps -> sample_adaptive_offset_enabled_flag = bs.getBits(1);
	psps -> pcm_enabled_flag = bs.getBits(1);

	if(psps -> pcm_enabled_flag)
	{
		psps -> pcm_sample_bit_depth_luma_minus1 = bs.getBits(4);
		psps -> pcm_sample_bit_depth_chroma_minus1 = bs.getBits(4);
		psps -> log2_min_pcm_luma_coding_block_size_minus3 = bs.getGolombU();
		psps -> log2_diff_max_min_pcm_luma_coding_block_size = bs.getGolombU();
		psps -> pcm_loop_filter_disabled_flag = bs.getBits(1);
	}

	psps -> num_short_term_ref_pic_sets = bs.getGolombU();

	psps -> short_term_ref_pic_set.resize(psps -> num_short_term_ref_pic_sets);
	for(size_t i=0; i<psps -> num_short_term_ref_pic_sets; i++)
		psps -> short_term_ref_pic_set[i] = processShortTermRefPicSet(i, psps -> num_short_term_ref_pic_sets, psps -> short_term_ref_pic_set, psps, bs);

	psps -> long_term_ref_pics_present_flag = bs.getBits(1);
	if(psps -> long_term_ref_pics_present_flag)
	{
		psps -> num_long_term_ref_pics_sps = bs.getGolombU();
		psps -> lt_ref_pic_poc_lsb_sps.resize(psps -> num_long_term_ref_pics_sps);
		psps -> used_by_curr_pic_lt_sps_flag.resize(psps -> num_long_term_ref_pics_sps);
		
		for(size_t i = 0; i<psps -> num_long_term_ref_pics_sps; i++)
		{
			psps -> lt_ref_pic_poc_lsb_sps[i] = bs.getBits(psps -> log2_max_pic_order_cnt_lsb_minus4 + 4);
			psps -> used_by_curr_pic_lt_sps_flag[i] = bs.getBits(1);
		}
	}

	psps -> sps_temporal_mvp_enabled_flag = bs.getBits(1);
	psps -> strong_intra_smoothing_enabled_flag = bs.getBits(1);
	psps -> vui_parameters_present_flag = bs.getBits(1);

	if(psps -> vui_parameters_present_flag)
	{
		psps -> vui_parameters = processVuiParameters(psps -> sps_max_sub_layers_minus1, bs);
	}

	psps -> sps_extension_flag = bs.getBits(1);
}

void HevcParser::processPPS(PPS *ppps, BitstreamReader &bs)
{
	ppps -> pps_pic_parameter_set_id = bs.getGolombU();
	ppps -> pps_seq_parameter_set_id  = bs.getGolombU();
	ppps -> dependent_slice_segments_enabled_flag = bs.getBits(1);
	
	ppps -> output_flag_present_flag = bs.getBits(1);
	ppps -> num_extra_slice_header_bits = bs.getBits(3);
	ppps -> sign_data_hiding_flag = bs.getBits(1);
	ppps -> cabac_init_present_flag = bs.getBits(1);
	ppps -> num_ref_idx_l0_default_active_minus1 = bs.getGolombU();
	ppps -> num_ref_idx_l1_default_active_minus1 = bs.getGolombU();
	ppps -> init_qp_minus26  = bs.getGolombS();
	ppps -> constrained_intra_pred_flag = bs.getBits(1);
	ppps -> transform_skip_enabled_flag = bs.getBits(1);
	ppps -> cu_qp_delta_enabled_flag = bs.getBits(1);
	
	if(ppps -> cu_qp_delta_enabled_flag)
		ppps -> diff_cu_qp_delta_depth = bs.getGolombU();
	else
		ppps -> diff_cu_qp_delta_depth = 0;
	
	ppps -> pps_cb_qp_offset = bs.getGolombS();
	ppps -> pps_cr_qp_offset = bs.getGolombS();
	ppps -> pps_slice_chroma_qp_offsets_present_flag = bs.getBits(1);
	ppps -> weighted_pred_flag = bs.getBits(1);
	ppps -> weighted_bipred_flag = bs.getBits(1);
	ppps -> transquant_bypass_enabled_flag = bs.getBits(1);
	ppps -> tiles_enabled_flag = bs.getBits(1);
	ppps -> entropy_coding_sync_enabled_flag = bs.getBits(1);
	
	if(ppps -> tiles_enabled_flag)
	{
		ppps -> num_tile_columns_minus1 = bs.getGolombU();
		ppps -> num_tile_rows_minus1 = bs.getGolombU();
		ppps -> uniform_spacing_flag = bs.getBits(1);
		
		if(!ppps -> uniform_spacing_flag)
		{
			ppps -> column_width_minus1.resize(ppps -> num_tile_columns_minus1);
			for(size_t i=0; i<ppps -> num_tile_columns_minus1; i++)
				ppps -> column_width_minus1[i] = bs.getGolombU();
			
			ppps -> row_height_minus1.resize(ppps -> num_tile_rows_minus1);
			for(size_t i=0; i<ppps -> num_tile_rows_minus1; i++)
				ppps -> row_height_minus1[i] = bs.getGolombU();
		}
		ppps -> loop_filter_across_tiles_enabled_flag = bs.getBits(1);
	}
	else
	{
		ppps -> num_tile_columns_minus1 = 0;
		ppps -> num_tile_rows_minus1 = 0;
		ppps -> uniform_spacing_flag = 1;
		ppps -> loop_filter_across_tiles_enabled_flag = 1;
	}
	
	ppps -> pps_loop_filter_across_slices_enabled_flag = bs.getBits(1);
	ppps -> deblocking_filter_control_present_flag = bs.getBits(1);
	
	if(ppps -> deblocking_filter_control_present_flag)
	{
		ppps -> deblocking_filter_override_enabled_flag = bs.getBits(1);
		ppps -> pps_deblocking_filter_disabled_flag = bs.getBits(1);
		
		if(!ppps -> pps_deblocking_filter_disabled_flag)
		{
			ppps -> pps_beta_offset_div2 = bs.getGolombS();
			ppps -> pps_tc_offset_div2 = bs.getGolombS();
		}
		else
		{
			ppps -> pps_beta_offset_div2 = 0;
			ppps -> pps_tc_offset_div2 = 0;
		}
	}
	else
	{
		ppps -> deblocking_filter_override_enabled_flag = 0;
		ppps -> pps_deblocking_filter_disabled_flag = 0;
	}
	
	ppps -> pps_scaling_list_data_present_flag = bs.getBits(1);
	if(ppps -> pps_scaling_list_data_present_flag)
	{
		ppps -> scaling_list_data = processScalingListData(bs);
	}
	
	ppps -> lists_modification_present_flag = bs.getBits(1);
	ppps -> log2_parallel_merge_level_minus2 = bs.getGolombU();
	ppps -> slice_segment_header_extension_present_flag = bs.getBits(1);
	ppps -> pps_extension_flag = bs.getBits(1);
}

static const uint8 log2_tab[256]={
	0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};
uint32 HEVC_log2(uint32 k)
{
	uint32 res = 0;
	if (k & 0xffff0000) {
		k >>= 16;
		res += 16;
	}
	if (k & 0xff00) {
		k >>= 8;
		res += 8;
	}
	res += log2_tab[k];
	return res;
}
size_t HEVC_calcNumPocTotalCurr(Slice *pslice, SPS *psps)
{
	size_t NumPocTotalCurr = 0;
	size_t currRpsIdx;
	
	bool UsedByCurrPicLt[16];
	size_t num_long_term = pslice -> num_long_term_sps + pslice -> num_long_term_pics;
	
	for(size_t i=0; i < num_long_term; i++)
	{
		if (i < pslice -> num_long_term_sps)
			UsedByCurrPicLt[i] = psps -> used_by_curr_pic_lt_sps_flag[pslice -> lt_idx_sps[i]] ? true : false;
		else
			UsedByCurrPicLt[i] = pslice -> used_by_curr_pic_lt_flag[i] ? true : false;
	}
	
	if(pslice -> short_term_ref_pic_set_sps_flag)
		currRpsIdx = pslice -> short_term_ref_pic_set_idx;
	else
		currRpsIdx = psps -> num_short_term_ref_pic_sets;
	
	if(psps -> short_term_ref_pic_set.size() <= currRpsIdx)
	{
		if(currRpsIdx != 0 || pslice->short_term_ref_pic_set_sps_flag)
			return 0;
	}
	
	ShortTermRefPicSet strps;
	
	if(currRpsIdx < psps -> short_term_ref_pic_set.size())
		strps = psps -> short_term_ref_pic_set[currRpsIdx];
	else
		strps = pslice -> short_term_ref_pic_set;
	
	for(size_t i = 0; i < strps.num_negative_pics; i++)
		if (strps.used_by_curr_pic_s0_flag[i])
			NumPocTotalCurr++;
	
	for(size_t i = 0; i < strps.num_positive_pics; i++)
		if(strps.used_by_curr_pic_s1_flag[i])
			NumPocTotalCurr++;
	
	for(size_t i = 0;i < (pslice -> num_long_term_sps + pslice -> num_long_term_pics); i++)
		if(UsedByCurrPicLt[i])
			NumPocTotalCurr++;
	
	return NumPocTotalCurr;
}
void HevcParser::processSlice(Slice *pslice, BitstreamReader &bs)
{
	if(!m_vps || !m_pps)
	{
		pslice -> m_processFailed = true;
		return;
	}

	pslice -> first_slice_segment_in_pic_flag = bs.getBits(1);
	
	if(pslice -> m_nalUnitType >= NAL_BLA_W_LP && pslice -> m_nalUnitType <= NAL_IRAP_VCL23)
		pslice -> no_output_of_prior_pics_flag = bs.getBits(1);
	
	pslice -> slice_pic_parameter_set_id = bs.getGolombU();
	
	int32 spsId = m_pps -> pps_seq_parameter_set_id;
	
	pslice -> dependent_slice_segment_flag = 0;
	if(!pslice -> first_slice_segment_in_pic_flag)
	{
		if(m_pps -> dependent_slice_segments_enabled_flag)
			pslice -> dependent_slice_segment_flag = bs.getBits(1);
		else
			pslice -> dependent_slice_segment_flag = 0;
		int32 CtbLog2SizeY = m_sps -> log2_min_luma_coding_block_size_minus3 + 3 + m_sps -> log2_diff_max_min_luma_coding_block_size;
		uint32 CtbSizeY = 1 << CtbLog2SizeY;
		uint32 PicWidthInCtbsY = m_sps -> pic_width_in_luma_samples / CtbSizeY;
		if(m_sps -> pic_width_in_luma_samples % CtbSizeY)
			PicWidthInCtbsY++;
		
		uint32 PicHeightInCtbsY = m_sps -> pic_height_in_luma_samples / CtbSizeY;
		if(m_sps -> pic_height_in_luma_samples % CtbSizeY)
			PicHeightInCtbsY++;
		
		int32 sliceAddrLength = HEVC_log2(PicHeightInCtbsY * PicWidthInCtbsY);
		if((uint32)(1 << sliceAddrLength) < PicHeightInCtbsY * PicWidthInCtbsY)
			sliceAddrLength++;
		
		pslice -> slice_segment_address = bs.getBits(sliceAddrLength);
	}
	
	if(!pslice -> dependent_slice_segment_flag)
	{
		uint32 num_extra_slice_header_bits = m_pps -> num_extra_slice_header_bits;
		pslice -> slice_reserved_undetermined_flag.resize(num_extra_slice_header_bits, 0);
		for(size_t i=0; i<num_extra_slice_header_bits; i++)
			pslice -> slice_reserved_undetermined_flag[i] = bs.getBits(1);
		
		pslice -> slice_type = bs.getGolombU();
		
		if(m_pps -> output_flag_present_flag)
			pslice -> pic_output_flag = bs.getBits(1);
		
		if(m_sps -> separate_colour_plane_flag)
			pslice -> colour_plane_id = bs.getBits(2);
		
		bool IdrPicFlag = pslice -> m_nalUnitType == NAL_IDR_W_RADL || pslice -> m_nalUnitType == NAL_IDR_N_LP;
		if(!IdrPicFlag)
		{
			if(m_sps -> log2_max_pic_order_cnt_lsb_minus4 + 4 >= 32)
			{
				pslice -> m_processFailed = true;
				return;
			}
			
			pslice -> pic_order_cnt_lsb = bs.getBits(m_sps -> log2_max_pic_order_cnt_lsb_minus4 + 4);
			pslice -> short_term_ref_pic_set_sps_flag = bs.getBits(1);
			
			if(!pslice -> short_term_ref_pic_set_sps_flag)
			{
				pslice -> short_term_ref_pic_set = processShortTermRefPicSet(m_sps -> num_short_term_ref_pic_sets, m_sps -> num_short_term_ref_pic_sets, m_sps -> short_term_ref_pic_set, m_sps, bs);
			}
			else if(m_sps -> num_short_term_ref_pic_sets > 1)
			{
				size_t numBits = HEVC_log2(m_sps -> num_short_term_ref_pic_sets);
				if((uint32)(1 << numBits) < m_sps -> num_short_term_ref_pic_sets)
					numBits++;
				
				if(numBits > 0)
					pslice -> short_term_ref_pic_set_idx = bs.getBits(numBits);
				else
					pslice -> short_term_ref_pic_set_idx = 0;
			}
			
			if(m_sps -> long_term_ref_pics_present_flag)
			{
				pslice -> num_long_term_sps = 0;
				if(m_sps -> num_long_term_ref_pics_sps > 0)
					pslice -> num_long_term_sps = bs.getGolombU();
				
				pslice -> num_long_term_pics = bs.getGolombU();
				
				size_t num_long_term = pslice -> num_long_term_sps + pslice -> num_long_term_pics;

				pslice -> lt_idx_sps.resize(num_long_term);
				pslice -> poc_lsb_lt.resize(num_long_term);
				pslice -> used_by_curr_pic_lt_flag.resize(num_long_term);
				pslice -> delta_poc_msb_present_flag.resize(num_long_term);
				pslice -> delta_poc_msb_cycle_lt.resize(num_long_term);

				for(size_t i=0; i < num_long_term; i++)
				{
					if(i < pslice -> num_long_term_sps)
					{
						if(m_sps -> num_long_term_ref_pics_sps > 1)
						{
							int32 ltIdxSpsLength = HEVC_log2(m_sps -> num_long_term_ref_pics_sps);
							pslice -> lt_idx_sps[i] = bs.getBits(ltIdxSpsLength);
						}
					}
					else
					{
						pslice -> poc_lsb_lt[i] = bs.getBits(m_sps -> log2_max_pic_order_cnt_lsb_minus4 + 4);
						pslice -> used_by_curr_pic_lt_flag[i] = bs.getBits(1);
					}
					
					pslice -> delta_poc_msb_present_flag[i] = bs.getBits(1);
					if(pslice -> delta_poc_msb_present_flag[i])
						pslice -> delta_poc_msb_cycle_lt[i] = bs.getGolombU();
				}
			}
			
			if(m_sps -> sps_temporal_mvp_enabled_flag)
				pslice -> slice_temporal_mvp_enabled_flag = bs.getBits(1);
		}
		
		if(m_sps -> sample_adaptive_offset_enabled_flag)
		{
			pslice -> slice_sao_luma_flag = bs.getBits(1);
			pslice -> slice_sao_chroma_flag = bs.getBits(1);
		}
		
		pslice -> num_ref_idx_l0_active_minus1 = m_pps -> num_ref_idx_l0_default_active_minus1;
		pslice -> num_ref_idx_l1_active_minus1 = m_pps -> num_ref_idx_l1_default_active_minus1;
		
		if(pslice -> slice_type == SLICE_B || pslice -> slice_type == SLICE_P)
		{
			pslice -> num_ref_idx_active_override_flag = bs.getBits(1);
			if(pslice -> num_ref_idx_active_override_flag)
			{
				pslice -> num_ref_idx_l0_active_minus1 = bs.getGolombU();
				
				if(pslice -> slice_type == SLICE_B)
					pslice -> num_ref_idx_l1_active_minus1 = bs.getGolombU();
			}
			
			if(m_pps -> lists_modification_present_flag)
			{
				size_t NumPocTotalCurr = HEVC_calcNumPocTotalCurr(pslice, m_sps);
				if(NumPocTotalCurr > 1)
					pslice -> ref_pic_lists_modification = processRefPicListModification(bs, pslice);
			}
			
			if(pslice -> slice_type == SLICE_B)
				pslice -> mvd_l1_zero_flag = bs.getBits(1);
			
			if(m_pps -> cabac_init_present_flag)
				pslice -> cabac_init_flag = bs.getBits(1);
			
			if(pslice -> slice_temporal_mvp_enabled_flag)
			{
				if(pslice -> slice_type == SLICE_B)
					pslice -> collocated_from_l0_flag = bs.getBits(1);
				
				if(pslice -> collocated_from_l0_flag && pslice -> num_ref_idx_l0_active_minus1 ||
					!pslice -> collocated_from_l0_flag && pslice -> num_ref_idx_l1_active_minus1)
				{
					pslice -> collocated_ref_idx = bs.getGolombU();
				}
			}
			
			if(m_pps -> weighted_pred_flag && pslice -> slice_type == SLICE_P ||
				m_pps -> weighted_bipred_flag && pslice -> slice_type == SLICE_B)
			{
				pslice -> pred_weight_table = processPredWeightTable(bs, pslice);
				
				if(pslice -> pred_weight_table.luma_log2_weight_denom > 7)
				{
					pslice -> m_processFailed = true;
					return;
				}
			}
			
			pslice -> five_minus_max_num_merge_cand = bs.getGolombU();
		}
		pslice -> slice_qp_delta = bs.getGolombS();
		
		if(m_pps -> pps_slice_chroma_qp_offsets_present_flag)
		{
			pslice -> slice_cb_qp_offset = bs.getGolombS();
			pslice -> slice_cr_qp_offset = bs.getGolombS();
		}
		
		if(m_pps -> deblocking_filter_override_enabled_flag)
			pslice -> deblocking_filter_override_flag = bs.getBits(1);
		
		if(pslice -> deblocking_filter_override_flag)
		{
			pslice -> slice_deblocking_filter_disabled_flag = bs.getBits(1);
			if(!pslice -> slice_deblocking_filter_disabled_flag)
			{
				pslice -> slice_beta_offset_div2 = bs.getGolombS();
				pslice -> slice_tc_offset_div2 = bs.getGolombS();
			}
		}
		else
		{
			pslice -> slice_deblocking_filter_disabled_flag = m_pps -> pps_deblocking_filter_disabled_flag;
		}
		
		if(m_pps -> pps_loop_filter_across_slices_enabled_flag && 
			(pslice -> slice_sao_luma_flag || pslice -> slice_sao_chroma_flag || !pslice -> slice_deblocking_filter_disabled_flag))
		{
			pslice -> slice_loop_filter_across_slices_enabled_flag = bs.getBits(1);
		}
	}
	if(m_pps -> tiles_enabled_flag || m_pps -> entropy_coding_sync_enabled_flag)
	{
		pslice -> num_entry_point_offsets = bs.getGolombU();
		if(pslice -> num_entry_point_offsets > 0)
		{
			pslice -> offset_len_minus1 = bs.getGolombU();
			pslice -> entry_point_offset_minus1.resize(pslice -> num_entry_point_offsets);
			
			if(pslice -> offset_len_minus1 > 31)
			{
				pslice -> m_processFailed = true;
				return;
			}
			for(size_t i=0; i<pslice -> num_entry_point_offsets; i++)
				pslice -> entry_point_offset_minus1[i] = bs.getBits(pslice -> offset_len_minus1 + 1);
		}
	}
	
	if(m_pps -> slice_segment_header_extension_present_flag)
	{
		pslice -> slice_segment_header_extension_length = bs.getGolombU();
		pslice -> slice_segment_header_extension_data_byte.resize(pslice -> slice_segment_header_extension_length);
		for(size_t i=0; i<pslice -> slice_segment_header_extension_length; i++)
			pslice -> slice_segment_header_extension_data_byte[i] = bs.getBits(8);
	}
}

ProfileTierLevel HevcParser::processProfileTierLevel(size_t max_sub_layers_minus1, BitstreamReader &bs)
{
	ProfileTierLevel ptl;
	
	ptl.Default();
	
	ptl.general_profile_space = bs.getBits(2);
	ptl.general_tier_flag = bs.getBits(1);
	ptl.general_profile_idc = bs.getBits(5);
	
	for(size_t i=0; i<32; i++)
		ptl.general_profile_compatibility_flag[i] = bs.getBits(1);
	
	ptl.general_progressive_source_flag = bs.getBits(1);
	ptl.general_interlaced_source_flag = bs.getBits(1);
	ptl.general_non_packed_constraint_flag = bs.getBits(1);
	ptl.general_frame_only_constraint_flag = bs.getBits(1);
	bs.getBits(32);
	bs.getBits(12);
	ptl.general_level_idc = bs.getBits(8);
	
	ptl.sub_layer_profile_present_flag.resize(max_sub_layers_minus1);
	ptl.sub_layer_level_present_flag.resize(max_sub_layers_minus1);
	
	for(size_t i=0; i<max_sub_layers_minus1; i++)
	{
		ptl.sub_layer_profile_present_flag[i] = bs.getBits(1);
		ptl.sub_layer_level_present_flag[i] = bs.getBits(1);
	}
	
	if(max_sub_layers_minus1 > 0)
	{
		for(size_t i=max_sub_layers_minus1; i<8; i++)
			bs.getBits(2);
	}
	
	ptl.sub_layer_profile_space.resize(max_sub_layers_minus1);
	ptl.sub_layer_tier_flag.resize(max_sub_layers_minus1);
	ptl.sub_layer_profile_idc.resize(max_sub_layers_minus1);
	ptl.sub_layer_profile_compatibility_flag.resize(max_sub_layers_minus1);
	ptl.sub_layer_progressive_source_flag.resize(max_sub_layers_minus1);
	ptl.sub_layer_interlaced_source_flag.resize(max_sub_layers_minus1);
	ptl.sub_layer_non_packed_constraint_flag.resize(max_sub_layers_minus1);
	ptl.sub_layer_frame_only_constraint_flag.resize(max_sub_layers_minus1);
	ptl.sub_layer_level_idc.resize(max_sub_layers_minus1);
	
	for(size_t i=0; i<max_sub_layers_minus1; i++)
	{
		if(ptl.sub_layer_profile_present_flag[i])
		{
			ptl.sub_layer_profile_space[i] = bs.getBits(2);
			ptl.sub_layer_tier_flag[i] = bs.getBits(1);
			ptl.sub_layer_profile_idc[i] = bs.getBits(5);
			ptl.sub_layer_profile_compatibility_flag[i].resize(32);
			
			for(size_t j=0; j<32; j++)
				ptl.sub_layer_profile_compatibility_flag[i][j] = bs.getBits(1);
			
			ptl.sub_layer_progressive_source_flag[i] = bs.getBits(1);
			ptl.sub_layer_interlaced_source_flag[i] = bs.getBits(1);
			ptl.sub_layer_non_packed_constraint_flag[i] = bs.getBits(1);
			ptl.sub_layer_frame_only_constraint_flag[i] = bs.getBits(1);
			bs.getBits(32);
			bs.getBits(12);
		}
		
		if(ptl.sub_layer_level_present_flag[i])
		{
			ptl.sub_layer_level_idc[i] = bs.getBits(8);
		}
		else
			ptl.sub_layer_level_idc[i] = 1;
	}
	
	return ptl;
}

HrdParameters HevcParser::processHrdParameters(uint8 commonInfPresentFlag, size_t maxNumSubLayersMinus1, BitstreamReader &bs)
{
	HrdParameters hrd;

	hrd.Default();

	hrd.nal_hrd_parameters_present_flag = 0;
	hrd.vcl_hrd_parameters_present_flag = 0;
	hrd.sub_pic_hrd_params_present_flag = 0;
	hrd.sub_pic_cpb_params_in_pic_timing_sei_flag = 0;
	if(commonInfPresentFlag)
	{
		hrd.nal_hrd_parameters_present_flag = bs.getBits(1);
		hrd.vcl_hrd_parameters_present_flag = bs.getBits(1);
		
		if(hrd.nal_hrd_parameters_present_flag || hrd.vcl_hrd_parameters_present_flag)
		{
			hrd.sub_pic_hrd_params_present_flag = bs.getBits(1);
			if(hrd.sub_pic_hrd_params_present_flag)
			{
				hrd.tick_divisor_minus2 = bs.getBits(8);
				hrd.du_cpb_removal_delay_increment_length_minus1 = bs.getBits(5);
				hrd.sub_pic_cpb_params_in_pic_timing_sei_flag = bs.getBits(1);
				hrd.dpb_output_delay_du_length_minus1 = bs.getBits(5);
			}
			hrd.bit_rate_scale = bs.getBits(4);
			hrd.cpb_size_scale = bs.getBits(4);
			
			if(hrd.sub_pic_hrd_params_present_flag)
				hrd.cpb_size_du_scale = bs.getBits(4);
			
			hrd.initial_cpb_removal_delay_length_minus1 = bs.getBits(5);
			hrd.au_cpb_removal_delay_length_minus1 = bs.getBits(5);
			hrd.dpb_output_delay_length_minus1 = bs.getBits(5);
		}
	}

	hrd.fixed_pic_rate_general_flag.resize(maxNumSubLayersMinus1 + 1);
	hrd.fixed_pic_rate_within_cvs_flag.resize(maxNumSubLayersMinus1 + 1);
	hrd.elemental_duration_in_tc_minus1.resize(maxNumSubLayersMinus1 + 1);
	hrd.low_delay_hrd_flag.resize(maxNumSubLayersMinus1 + 1, 0);
	hrd.cpb_cnt_minus1.resize(maxNumSubLayersMinus1 + 1, 0);

	if(hrd.nal_hrd_parameters_present_flag)
		hrd.nal_sub_layer_hrd_parameters.resize(maxNumSubLayersMinus1 + 1);
	if(hrd.vcl_hrd_parameters_present_flag)
		hrd.vcl_sub_layer_hrd_parameters.resize(maxNumSubLayersMinus1 + 1);

	for(size_t i = 0; i <= maxNumSubLayersMinus1; i++ )
	{
		hrd.fixed_pic_rate_general_flag[i] = bs.getBits(1);
		
		if(hrd.fixed_pic_rate_general_flag[i])
			hrd.fixed_pic_rate_within_cvs_flag[i] = 1;
		
		if(!hrd.fixed_pic_rate_general_flag[i])
			hrd.fixed_pic_rate_within_cvs_flag[i] = bs.getBits(1);
		
		if(hrd.fixed_pic_rate_within_cvs_flag[i])
			hrd.elemental_duration_in_tc_minus1[i] = bs.getGolombU();
		else
			hrd.low_delay_hrd_flag[i] = bs.getBits(1);
		
		if(!hrd.low_delay_hrd_flag[i])
			hrd.cpb_cnt_minus1[i] = bs.getGolombU();
		
		if(hrd.nal_hrd_parameters_present_flag)
			hrd.nal_sub_layer_hrd_parameters[i] = processSubLayerHrdParameters(hrd.sub_pic_hrd_params_present_flag, hrd.cpb_cnt_minus1[i], bs);
		if(hrd.vcl_hrd_parameters_present_flag)
			hrd.vcl_sub_layer_hrd_parameters[i] = processSubLayerHrdParameters(hrd.sub_pic_hrd_params_present_flag, hrd.cpb_cnt_minus1[i], bs);
	}

	return hrd;
}

SubLayerHrdParameters HevcParser::processSubLayerHrdParameters(uint8 sub_pic_hrd_params_present_flag, size_t CpbCnt, BitstreamReader &bs)
{
	SubLayerHrdParameters slhrd;
	
	slhrd.Default();
	slhrd.bit_rate_value_minus1.resize(CpbCnt + 1);
	slhrd.cpb_size_value_minus1.resize(CpbCnt + 1);
	slhrd.cpb_size_du_value_minus1.resize(CpbCnt + 1);
	slhrd.bit_rate_du_value_minus1.resize(CpbCnt + 1);
	slhrd.cbr_flag.resize(CpbCnt + 1);
	
	for(size_t i=0; i<=CpbCnt; i++)
	{
		slhrd.bit_rate_value_minus1[i] = bs.getGolombU();
		slhrd.cpb_size_value_minus1[i] = bs.getGolombU();
		
		if(sub_pic_hrd_params_present_flag)
		{
			slhrd.cpb_size_du_value_minus1[i] = bs.getGolombU();
			slhrd.bit_rate_du_value_minus1[i] = bs.getGolombU();
		}
		
		slhrd.cbr_flag[i] = bs.getBits(1);
	}
	
	return slhrd;
}

ShortTermRefPicSet HevcParser::processShortTermRefPicSet(size_t stRpsIdx, size_t num_short_term_ref_pic_sets, const std::vector<ShortTermRefPicSet> &refPicSets, SPS *psps, BitstreamReader &bs)
{
	ShortTermRefPicSet rpset;
	
	rpset.Default();
	
	rpset.inter_ref_pic_set_prediction_flag = 0;
	rpset.delta_idx_minus1 = 0;
	if(stRpsIdx)
	{
		rpset.inter_ref_pic_set_prediction_flag = bs.getBits(1);
	}
	
	if(rpset.inter_ref_pic_set_prediction_flag)
	{
		if(stRpsIdx == num_short_term_ref_pic_sets)
			rpset.delta_idx_minus1 = bs.getGolombU();
		
		rpset.delta_rps_sign = bs.getBits(1);
		rpset.abs_delta_rps_minus1 = bs.getGolombU();
		
		size_t RefRpsIdx = stRpsIdx - (rpset.delta_idx_minus1 + 1);
		size_t NumDeltaPocs = 0;
		
		if(refPicSets[RefRpsIdx].inter_ref_pic_set_prediction_flag)
		{
			for(size_t i=0; i<refPicSets[RefRpsIdx].used_by_curr_pic_flag.size(); i++)
				if(refPicSets[RefRpsIdx].used_by_curr_pic_flag[i] || refPicSets[RefRpsIdx].use_delta_flag[i])
					NumDeltaPocs++;
		}
		else
			NumDeltaPocs = refPicSets[RefRpsIdx].num_negative_pics + refPicSets[RefRpsIdx].num_positive_pics;
		
		rpset.used_by_curr_pic_flag.resize(NumDeltaPocs + 1);
		rpset.use_delta_flag.resize(NumDeltaPocs + 1, 1);
		
		for(size_t i=0; i<=NumDeltaPocs; i++ )
		{
			rpset.used_by_curr_pic_flag[i] = bs.getBits(1);
			if(!rpset.used_by_curr_pic_flag[i])
				rpset.use_delta_flag[i] = bs.getBits(1);
		}
	}
	else
	{
		rpset.num_negative_pics = bs.getGolombU();
		rpset.num_positive_pics = bs.getGolombU();
		
		if(rpset.num_negative_pics > psps -> sps_max_dec_pic_buffering_minus1[psps -> sps_max_sub_layers_minus1])
		{
			// "ShortTermRefPicSet: num_negative_pics > sps_max_dec_pic_buffering_minus1"
			return rpset;
		}
		
		if(rpset.num_positive_pics > psps -> sps_max_dec_pic_buffering_minus1[psps -> sps_max_sub_layers_minus1])
		{
			// "ShortTermRefPicSet: num_positive_pics > sps_max_dec_pic_buffering_minus1"
			return rpset;
		}
		
		rpset.delta_poc_s0_minus1.resize(rpset.num_negative_pics);
		rpset.used_by_curr_pic_s0_flag.resize(rpset.num_negative_pics);
		
		for(size_t i=0; i<rpset.num_negative_pics; i++)
		{
			rpset.delta_poc_s0_minus1[i] = bs.getGolombU();
			rpset.used_by_curr_pic_s0_flag[i] = bs.getBits(1);
		}
		
		rpset.delta_poc_s1_minus1.resize(rpset.num_positive_pics);
		rpset.used_by_curr_pic_s1_flag.resize(rpset.num_positive_pics);
		for(size_t i=0; i<rpset.num_positive_pics; i++)
		{
			rpset.delta_poc_s1_minus1[i] = bs.getGolombU();
			rpset.used_by_curr_pic_s1_flag[i] = bs.getBits(1);
		}
	}
	
	return rpset;
}

VuiParameters HevcParser::processVuiParameters(size_t sps_max_sub_layers_minus1, BitstreamReader &bs)
{
	VuiParameters vui;
	
	vui.Default();
	
	vui.aspect_ratio_idc = 0;
	vui.sar_width = 0;
	vui.sar_height = 0;
	
	vui.aspect_ratio_info_present_flag = bs.getBits(1);
	
	if(vui.aspect_ratio_info_present_flag)
	{
		vui.aspect_ratio_idc = bs.getBits(8);
		
		if(vui.aspect_ratio_idc == 255) //EXTENDED_SAR
		{
			vui.sar_width = bs.getBits(16);
			vui.sar_height = bs.getBits(16);
		}
	}
	
	vui.overscan_info_present_flag = bs.getBits(1);
	if(vui.overscan_info_present_flag)
		vui.overscan_appropriate_flag = bs.getBits(1);
	
	vui.video_format = 5;
	vui.video_full_range_flag = 0;
	vui.colour_primaries = 2;
	vui.transfer_characteristics = 2;
	vui.matrix_coeffs = 2;
	
	vui.video_signal_type_present_flag = bs.getBits(1);
	
	if(vui.video_signal_type_present_flag)
	{
		vui.video_format = bs.getBits(3);
		vui.video_full_range_flag = bs.getBits(1);
		vui.colour_description_present_flag = bs.getBits(1);
		
		if(vui.colour_description_present_flag)
		{
			vui.colour_primaries = bs.getBits(8);
			vui.transfer_characteristics = bs.getBits(8);
			vui.matrix_coeffs = bs.getBits(8);
		}
	}
	
	vui.chroma_sample_loc_type_top_field = 0;
	vui.chroma_sample_loc_type_bottom_field = 0;
	
	vui.chroma_loc_info_present_flag = bs.getBits(1);
	if(vui.chroma_loc_info_present_flag)
	{
		vui.chroma_sample_loc_type_top_field = bs.getGolombU();
		vui.chroma_sample_loc_type_bottom_field = bs.getGolombU();
	}
	
	vui.neutral_chroma_indication_flag = bs.getBits(1);
	vui.field_seq_flag = bs.getBits(1);
	vui.frame_field_info_present_flag = bs.getBits(1);
	vui.default_display_window_flag = bs.getBits(1);
	
	vui.def_disp_win_left_offset = 0;
	vui.def_disp_win_right_offset = 0;
	vui.def_disp_win_right_offset = 0;
	vui.def_disp_win_bottom_offset = 0;
	
	if(vui.default_display_window_flag)
	{
		vui.def_disp_win_left_offset = bs.getGolombU();
		vui.def_disp_win_right_offset = bs.getGolombU();
		vui.def_disp_win_top_offset = bs.getGolombU();
		vui.def_disp_win_bottom_offset = bs.getGolombU();
	}
	
	vui.vui_timing_info_present_flag = bs.getBits(1);
	
	if(vui.vui_timing_info_present_flag)
	{
		vui.vui_num_units_in_tick = bs.getBits(32);
		vui.vui_time_scale = bs.getBits(32);
		vui.vui_poc_proportional_to_timing_flag = bs.getBits(1);
	
		if(vui.vui_poc_proportional_to_timing_flag)
			vui.vui_num_ticks_poc_diff_one_minus1 = bs.getGolombU();
	
		vui.vui_hrd_parameters_present_flag = bs.getBits(1);
	
		if(vui.vui_hrd_parameters_present_flag)
			vui.hrd_parameters = processHrdParameters(1, sps_max_sub_layers_minus1, bs);
	}
	
	vui.bitstream_restriction_flag = bs.getBits(1);
	
	if(vui.bitstream_restriction_flag)
	{
		vui.tiles_fixed_structure_flag = bs.getBits(1);
		vui.motion_vectors_over_pic_boundaries_flag = bs.getBits(1);
		vui.restricted_ref_pic_lists_flag = bs.getBits(1);
		
		vui.min_spatial_segmentation_idc = bs.getGolombU();
		vui.max_bytes_per_pic_denom = bs.getGolombU();
		vui.max_bits_per_min_cu_denom = bs.getGolombU();
		vui.log2_max_mv_length_horizontal = bs.getGolombU();
		vui.log2_max_mv_length_vertical = bs.getGolombU();
	}
	
	return vui;
}

ScalingListData HevcParser::processScalingListData(BitstreamReader &bs)
{
	ScalingListData sc;
	
	sc.scaling_list_pred_mode_flag.resize(4);
	sc.scaling_list_pred_matrix_id_delta.resize(4);
	sc.scaling_list_dc_coef_minus8.resize(2);
	sc.scaling_list_delta_coef.resize(4);
	
	for(size_t sizeId = 0; sizeId < 4; sizeId++)
	{
		if(sizeId == 3)
		{
			sc.scaling_list_pred_mode_flag[sizeId].resize(2);
			sc.scaling_list_pred_matrix_id_delta[sizeId].resize(2);
			sc.scaling_list_dc_coef_minus8[sizeId-2].resize(2);
			sc.scaling_list_delta_coef[sizeId].resize(2);
		}
		else
		{
			sc.scaling_list_pred_mode_flag[sizeId].resize(6);
			sc.scaling_list_pred_matrix_id_delta[sizeId].resize(6);
			sc.scaling_list_delta_coef[sizeId].resize(6);
			if(sizeId >= 2)
				sc.scaling_list_dc_coef_minus8[sizeId-2].resize(6);
		}
		
		for(size_t matrixId = 0; matrixId<(size_t)((sizeId == 3)?2:6); matrixId++)
		{
			sc.scaling_list_pred_mode_flag[sizeId][matrixId] = bs.getBits(1);
			if(!sc.scaling_list_pred_mode_flag[sizeId][matrixId])
				sc.scaling_list_pred_matrix_id_delta[sizeId][matrixId] = bs.getGolombU();
			else
			{
				size_t nextCoef = 8;
				size_t coefNum = ((size_t)1 << (4 + (sizeId << 1)));
				if(coefNum > 64) coefNum = 64;
				if(sizeId > 1)
					sc.scaling_list_dc_coef_minus8[sizeId-2][matrixId] = bs.getGolombS();
				
				sc.scaling_list_delta_coef[sizeId][matrixId].resize(coefNum);
				for(size_t i = 0; i < coefNum; i++)
					sc.scaling_list_delta_coef[sizeId][matrixId][i] = bs.getGolombS();
			}
		}
	}
	
	return sc;
}

RefPicListModification HevcParser::processRefPicListModification(BitstreamReader &bs, Slice *pslice)
{
	RefPicListModification res;
	res.Default();
	
	if(!m_sps || !m_pps)
	{
		return res;
	}
	
	int32 spsId = m_pps -> pps_seq_parameter_set_id;
	
	size_t numPocTotalCurr = HEVC_calcNumPocTotalCurr(pslice, m_sps);
	size_t listSize = HEVC_log2((uint32)numPocTotalCurr);
	
	if(((size_t)1 << listSize) < numPocTotalCurr)
		listSize++;
	
	res.ref_pic_list_modification_flag_l0 = bs.getBits(1);
	
	if(res.ref_pic_list_modification_flag_l0)
	{
		res.list_entry_l0.resize(pslice -> num_ref_idx_l0_active_minus1);
		
		for(std::size_t i=0; i<pslice -> num_ref_idx_l0_active_minus1; i++)
			res.list_entry_l0[i] = bs.getBits(listSize);
	}
	
	res.ref_pic_list_modification_flag_l1 = bs.getBits(1);
	
	if(res.ref_pic_list_modification_flag_l1)
	{
		res.list_entry_l1.resize(pslice -> num_ref_idx_l1_active_minus1);
		
		for(size_t i=0; i<pslice -> num_ref_idx_l1_active_minus1; i++)
			res.list_entry_l1[i] = bs.getBits(listSize);
	}
	
	return res;
}

PredWeightTable HevcParser::processPredWeightTable(BitstreamReader &bs, Slice *pslice)
{
	PredWeightTable pwt;
	pwt.Default();

	if(!m_sps || !m_pps)
	{
		return pwt;
	}
	
	pwt.luma_log2_weight_denom = bs.getGolombU();
	if(m_sps -> chroma_format_idc != 0)
		pwt.delta_chroma_log2_weight_denom = bs.getGolombS();
	
	pwt.luma_weight_l0_flag.resize(pslice -> num_ref_idx_l0_active_minus1 + 1);
	
	for(size_t i=0; i<=pslice -> num_ref_idx_l0_active_minus1; i++)
		pwt.luma_weight_l0_flag[i] = bs.getBits(1);
	
	pwt.chroma_weight_l0_flag.resize(pslice -> num_ref_idx_l0_active_minus1 + 1, 0);
	
	if(m_sps -> chroma_format_idc != 0)
	{
		for(size_t i=0; i<=pslice -> num_ref_idx_l0_active_minus1; i++)
			pwt.chroma_weight_l0_flag[i] = bs.getBits(1);
	}
	
	pwt.delta_luma_weight_l0.resize(pslice -> num_ref_idx_l0_active_minus1 + 1);
	pwt.luma_offset_l0.resize(pslice -> num_ref_idx_l0_active_minus1 + 1);
	
	pwt.delta_chroma_weight_l0_0.resize(pslice -> num_ref_idx_l0_active_minus1 + 1);
	pwt.delta_chroma_weight_l0_1.resize(pslice -> num_ref_idx_l0_active_minus1 + 1);
	pwt.delta_chroma_offset_l0_0.resize(pslice -> num_ref_idx_l0_active_minus1 + 1);
	pwt.delta_chroma_offset_l0_1.resize(pslice -> num_ref_idx_l0_active_minus1 + 1);
	//pwt.delta_chroma_weight_l0.resize(pslice -> num_ref_idx_l0_active_minus1 + 1);
	//pwt.delta_chroma_offset_l0.resize(pslice -> num_ref_idx_l0_active_minus1 + 1);
	
	for(size_t i=0; i<=pslice -> num_ref_idx_l0_active_minus1; i++)
	{
		if(pwt.luma_weight_l0_flag[i])
		{
			pwt.delta_luma_weight_l0[i] = bs.getGolombS();
			pwt.luma_offset_l0[i] = bs.getGolombS();
		}
		if(pwt.chroma_weight_l0_flag[i])
		{
			pwt.delta_chroma_weight_l0_0[i] = bs.getGolombS();
			pwt.delta_chroma_offset_l0_0[i] = bs.getGolombS();
			pwt.delta_chroma_weight_l0_1[i] = bs.getGolombS();
			pwt.delta_chroma_offset_l0_1[i] = bs.getGolombS();
			//for(std::size_t j=0; j<2; j++)
			//{
			//	pwt.delta_chroma_weight_l0[i][j] = bs.getGolombS();
			//	pwt.delta_chroma_offset_l0[i][j] = bs.getGolombS();
			//}
		}
	}
	
	if(pslice -> slice_type == SLICE_B)
	{
		pwt.luma_weight_l1_flag.resize(pslice -> num_ref_idx_l1_active_minus1 + 1);
		
		for(size_t i=0; i<=pslice -> num_ref_idx_l1_active_minus1; i++)
			pwt.luma_weight_l1_flag[i] = bs.getBits(1);
		
		pwt.chroma_weight_l1_flag.resize(pslice -> num_ref_idx_l1_active_minus1 + 1, 0);
		
		if(m_sps -> chroma_format_idc != 0)
		{
			for(size_t i=0; i<=pslice -> num_ref_idx_l1_active_minus1; i++)
				pwt.chroma_weight_l1_flag[i] = bs.getBits(1);
		}
		
		pwt.delta_luma_weight_l1.resize(pslice -> num_ref_idx_l1_active_minus1 + 1);
		pwt.luma_offset_l1.resize(pslice -> num_ref_idx_l1_active_minus1 + 1);

		pwt.delta_chroma_weight_l1_0.resize(pslice -> num_ref_idx_l1_active_minus1 + 1);
		pwt.delta_chroma_weight_l1_1.resize(pslice -> num_ref_idx_l1_active_minus1 + 1);
		pwt.delta_chroma_offset_l1_0.resize(pslice -> num_ref_idx_l1_active_minus1 + 1);
		pwt.delta_chroma_offset_l1_1.resize(pslice -> num_ref_idx_l1_active_minus1 + 1);
		//pwt.delta_chroma_weight_l1.resize(pslice -> num_ref_idx_l1_active_minus1 + 1);
		//pwt.delta_chroma_offset_l1.resize(pslice -> num_ref_idx_l1_active_minus1 + 1);
		
		for(size_t i=0; i<=pslice -> num_ref_idx_l1_active_minus1; i++)
		{
			if(pwt.luma_weight_l1_flag[i])
			{
				pwt.delta_luma_weight_l1[i] = bs.getGolombS();
				pwt.luma_offset_l1[i] = bs.getGolombS();
			}
			if(pwt.chroma_weight_l1_flag[i])
			{
				pwt.delta_chroma_weight_l1_0[i] = bs.getGolombS();
				pwt.delta_chroma_offset_l1_0[i] = bs.getGolombS();
				pwt.delta_chroma_weight_l1_1[i] = bs.getGolombS();
				pwt.delta_chroma_offset_l1_1[i] = bs.getGolombS();
				//for(size_t j=0; j<2; j++)
				//{
				//	pwt.delta_chroma_weight_l1[i][j] = bs.getGolombS();
				//	pwt.delta_chroma_offset_l1[i][j] = bs.getGolombS();
				//}
			}
		}
	}

	return pwt;
}
