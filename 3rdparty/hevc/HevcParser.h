#ifndef _HEVC_PARSER_H_
#define _HEVC_PARSER_H_

#include "HevcDefs.h"
#include "BitstreamReader.h"

enum
{
	HEVC_TYPE_NONE = 0,
	HEVC_TYPE_VPS,
	HEVC_TYPE_SPS,
	HEVC_TYPE_PPS,
	HEVC_TYPE_SLICE,
};
class HevcParser
{
public:
	uint8* m_ptr;
	size_t m_size;

	uint32 m_NumTile;
	uint32 m_CountTile;
	size_t m_PacketLength;

	bool m_b_slice;

	void Reset();
	uint32 GetTileNum();
	uint32 GetResolution();

	bool IsTile(uint8* pInput, size_t size, bool b_slice = true);
	bool GetData(uint8* pOutput, size_t maxsize);
	size_t GetPacketLength(){return m_PacketLength;}
	int GetSliceNumber(const uint8 *pdata, size_t size, int max_num);

public:
	UINT process(const uint8 *pdata, size_t size);

	VPS* GetVPS(){return m_vps;}
	SPS* GetSPS(){return m_sps;}
	PPS* GetPPS(){return m_pps;}
	Slice* GetSlice(){return m_lastSlice;}

private:
	UINT processNALUnit(const uint8 *pdata, size_t size);
	NALUnitType processNALUnitHeader(BitstreamReader &bs);
	void processVPS(VPS *pvps, BitstreamReader &bs);
	void processSPS(SPS *psps, BitstreamReader &bs);
	void processPPS(PPS *ppps, BitstreamReader &bs);
	void processSlice(Slice *pslice, BitstreamReader &bs);

	ProfileTierLevel processProfileTierLevel(size_t max_sub_layers_minus1, BitstreamReader &bs);
	HrdParameters processHrdParameters(uint8 commonInfPresentFlag, size_t maxNumSubLayersMinus1, BitstreamReader &bs);
	SubLayerHrdParameters processSubLayerHrdParameters(uint8 sub_pic_hrd_params_present_flag, size_t CpbCnt, BitstreamReader &bs);
	ShortTermRefPicSet processShortTermRefPicSet(size_t stRpsIdx, size_t num_short_term_ref_pic_sets, const std::vector<ShortTermRefPicSet> &refPicSets, SPS *psps, BitstreamReader &bs);
	VuiParameters processVuiParameters(size_t sps_max_sub_layers_minus1, BitstreamReader &bs);
	ScalingListData processScalingListData(BitstreamReader &bs);
	RefPicListModification processRefPicListModification(BitstreamReader &bs, Slice *pslice);
	PredWeightTable processPredWeightTable(BitstreamReader &bs, Slice *pslice);

	VPS *m_vps;
	SPS *m_sps;
	PPS *m_pps;
	Slice *m_lastSlice;

public:
	HevcParser();
	virtual ~HevcParser();
};

#endif
