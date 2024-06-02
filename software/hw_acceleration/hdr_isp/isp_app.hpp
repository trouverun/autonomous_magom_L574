#ifndef ISP_APP
#define ISP_APP

#include "common/xf_headers.hpp"
#include "xcl2/xcl2.hpp"
#include "isp_kernel.hpp"

static constexpr size_t image_in_size_bytes = IMG_HEIGHT * IMG_WIDTH * (2 * sizeof(unsigned char));
static constexpr size_t image_out_size_bytes = IMG_HEIGHT * IMG_WIDTH * 3 * sizeof(unsigned char);

class ISPApp {
    public:
        ISPApp(const std::string& xclbin);
        ~ISPApp();
        void processImage(const cv::Mat& in_image, cv::Mat& out_image);
    private:
        cl::Kernel kernel;
        cl::Buffer buffer_in;
        cl::Buffer buffer_out;
        cl::CommandQueue command_queue;
};

extern "C" {
    ISPApp* ISPApp_new(const char* xclbin);
    void ISPApp_delete(ISPApp* ip);
    void ISPApp_processImage(ISPApp* ip, const cv::Mat& in_image, cv::Mat& out_image);
}

#endif