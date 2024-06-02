
#ifndef _XF_ISP_KERNEL_CONFIG_H_
#define _XF_ISP_KERNEL_CONFIG_H_

#include "imgproc/xf_demosaicing.hpp"
#include "imgproc/xf_ltm.hpp"

static constexpr int INPUT_PTR_WIDTH = 512;
static constexpr int OUTPUT_PTR_WIDTH = 512;
static constexpr int IMG_WIDTH = 3840;
static constexpr int IMG_HEIGHT = 2160;
static constexpr int XF_CV_DEPTH = 2;
static constexpr int XF_CV_NPPC = 1;

static constexpr int BPATTERN = XF_BAYER_BG;
static constexpr int XF_USE_URAM = 0;
static constexpr int BAYER_IN_TYPE = XF_16UC1;

static constexpr int LTM_IN_TYPE = XF_16UC3;
static constexpr int LTM_OUT_TYPE = XF_8UC3;
static constexpr int LTM_BLOCK_HEIGHT = 64;
static constexpr int LTM_BLOCK_WIDTH = 64;

#endif