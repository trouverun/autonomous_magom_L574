#include "isp_kernel.hpp"

extern "C" {

static constexpr int __XF_DEPTH_IN = (IMG_HEIGHT * IMG_WIDTH * XF_PIXELWIDTH(LTM_IN_TYPE, XF_CV_NPPC)) / INPUT_PTR_WIDTH;
static constexpr int __XF_DEPTH_OUT = (IMG_HEIGHT * IMG_WIDTH * XF_PIXELWIDTH(LTM_OUT_TYPE, XF_CV_NPPC)) / OUTPUT_PTR_WIDTH;
static constexpr int MinMaxVArrSize = LTMTile<LTM_BLOCK_HEIGHT, LTM_BLOCK_WIDTH, IMG_HEIGHT, IMG_WIDTH, XF_CV_NPPC>::MinMaxVArrSize;
static constexpr int MinMaxHArrSize = LTMTile<LTM_BLOCK_HEIGHT, LTM_BLOCK_WIDTH, IMG_HEIGHT, IMG_WIDTH, XF_CV_NPPC>::MinMaxHArrSize;

static XF_CTUNAME(LTM_IN_TYPE, XF_CV_NPPC) omin[2][MinMaxVArrSize][MinMaxHArrSize];
static XF_CTUNAME(LTM_IN_TYPE, XF_CV_NPPC) omax[2][MinMaxVArrSize][MinMaxHArrSize];

void isp_pipeline(ap_uint<INPUT_PTR_WIDTH>* in_ptr, ap_uint<OUTPUT_PTR_WIDTH>* out_ptr) {
    // clang-format off
    #pragma HLS INTERFACE m_axi      port=in_ptr  offset=slave bundle=gmem_in  depth=__XF_DEPTH_IN
    #pragma HLS INTERFACE m_axi      port=out_ptr offset=slave bundle=gmem_out depth=__XF_DEPTH_OUT
    #pragma HLS INTERFACE s_axilite  port=return

    #pragma HLS ARRAY_PARTITION variable=omin dim=1 complete
    #pragma HLS ARRAY_PARTITION variable=omin dim=2 cyclic factor=2
    #pragma HLS ARRAY_PARTITION variable=omin dim=3 cyclic factor=2
    #pragma HLS ARRAY_PARTITION variable=omax dim=1 complete
    #pragma HLS ARRAY_PARTITION variable=omax dim=2 cyclic factor=2
    #pragma HLS ARRAY_PARTITION variable=omax dim=3 cyclic factor=2
    // clang-format on

    xf::cv::Mat<BAYER_IN_TYPE, IMG_HEIGHT, IMG_WIDTH, XF_CV_NPPC, XF_CV_DEPTH> imgInput(IMG_HEIGHT, IMG_WIDTH);
    xf::cv::Mat<LTM_IN_TYPE, IMG_HEIGHT, IMG_WIDTH, XF_CV_NPPC, XF_CV_DEPTH> imgDebayer(IMG_HEIGHT, IMG_WIDTH);
    xf::cv::Mat<LTM_OUT_TYPE, IMG_HEIGHT, IMG_WIDTH, XF_CV_NPPC, XF_CV_DEPTH> imgOutput(IMG_HEIGHT, IMG_WIDTH);

    // clang-format off
    #pragma HLS DATAFLOW
    // clang-format on

    xf::cv::Array2xfMat<INPUT_PTR_WIDTH, BAYER_IN_TYPE, IMG_HEIGHT, IMG_WIDTH, XF_CV_NPPC, XF_CV_DEPTH>(in_ptr, imgInput);

    xf::cv::demosaicing<BPATTERN, BAYER_IN_TYPE, LTM_IN_TYPE, IMG_HEIGHT, IMG_WIDTH, XF_CV_NPPC, XF_USE_URAM, XF_CV_DEPTH, XF_CV_DEPTH>(imgInput, imgDebayer);

    xf::cv::LTM<LTM_IN_TYPE, LTM_OUT_TYPE, LTM_BLOCK_HEIGHT, LTM_BLOCK_WIDTH, IMG_HEIGHT, IMG_WIDTH, XF_CV_NPPC, XF_CV_DEPTH, XF_CV_DEPTH>::process(
        imgDebayer, LTM_BLOCK_HEIGHT, LTM_BLOCK_WIDTH, omin[1], omax[1], omin[0], omax[0], imgOutput
    );

    xf::cv::xfMat2Array<OUTPUT_PTR_WIDTH, LTM_OUT_TYPE, IMG_HEIGHT, IMG_WIDTH, XF_CV_NPPC, XF_CV_DEPTH>(imgOutput, out_ptr);

    return;
}

}