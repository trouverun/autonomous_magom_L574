#include "isp_app.hpp"

ISPApp::ISPApp(const std::string& xclbin) {
    std::cout << "INFO: Initializing ISPApp." << std::endl;
    cl_int err;

    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];
    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, cl::CommandQueue command_queue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err, std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    std::cout << "INFO: Device found - " << device_name << std::endl;
    std::cout << "Input Image Bit Depth:" << XF_DTPIXELDEPTH(BAYER_IN_TYPE, XF_CV_NPPC) << std::endl;
    std::cout << "Input Image Channels:" << XF_CHANNELS(BAYER_IN_TYPE, XF_CV_NPPC) << std::endl;
    std::cout << "Output Image Bit Depth:" << XF_DTPIXELDEPTH(BAYER_IN_TYPE, XF_CV_NPPC) << std::endl;
    std::cout << "Output Image Channels:" << XF_CHANNELS(BAYER_IN_TYPE, XF_CV_NPPC) << std::endl;
    std::cout << "NPPC:" << XF_CV_NPPC << std::endl;

    std::string binaryFile = xcl::find_binary_file(device_name, xclbin);
    cl::Program::Binaries bins = xcl::import_binary_file(binaryFile);
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err)); 
    cl::Kernel kernel(program, "image_pipeline");

    OCL_CHECK(err, cl::Buffer buffer_in(context, CL_MEM_READ_ONLY, image_in_size_bytes, NULL, &err));
    OCL_CHECK(err, cl::Buffer buffer_out(context, CL_MEM_READ_ONLY, image_out_size_bytes, NULL, &err));
    OCL_CHECK(err, err = kernel.setArg(0, buffer_in));
    OCL_CHECK(err, err = kernel.setArg(1, buffer_out));
}

ISPApp::~ISPApp() {
}

void ISPApp::processImage(const cv::Mat& in_image, cv::Mat& out_image){
    cl::Event event;
    cl_int err;

    OCL_CHECK(err, command_queue.enqueueWriteBuffer(buffer_in,        // buffer on the FPGA
                                        CL_TRUE,                      // blocking call
                                        0,                            // buffer offset in bytes
                                        image_in_size_bytes,          // Size in bytes
                                        in_image.data,                // Pointer to the data to copy
                                        nullptr, &event));

    cl::Event event_sp;
    OCL_CHECK(err, err = command_queue.enqueueTask(kernel, NULL, &event_sp));
    clWaitForEvents(1, (const cl_event*)&event_sp);

    command_queue.enqueueReadBuffer(buffer_out,             // This buffers data will be read
                            CL_TRUE,                        // blocking call
                            0,                              // offset
                            image_out_size_bytes,       
                            out_image.data,                 // Data will be stored here
                            nullptr, &event);
    
    command_queue.finish();
}

extern "C" {
    ISPApp* ISPApp_new(const char* xclbin) {
        return new ISPApp(xclbin);
    }

    void ISPApp_delete(ISPApp* ip) {
        delete ip;
    }

    void ISPApp_processImage(ISPApp* ip, const cv::Mat& in_image, cv::Mat& out_image) {
        ip->processImage(in_image, out_image);
    }
}