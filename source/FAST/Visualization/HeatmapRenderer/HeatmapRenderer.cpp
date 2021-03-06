#include "HeatmapRenderer.hpp"
#include "FAST/Data/Image.hpp"

namespace fast {

HeatmapRenderer::HeatmapRenderer() {
    createInputPort<Image>(0, false);
    mColors[0] = Color::Red();
    createOpenCLProgram(Config::getKernelSourcePath() + "/Visualization/HeatmapRenderer/HeatmapRenderer.cl");
    mIsModified = false;
}

void HeatmapRenderer::addInputConnection(ProcessObjectPort port, Color color) {
    uint nr = getNrOfInputData();
    if(nr > 0)
        createInputPort<Image>(nr);
    releaseInputAfterExecute(nr, false);
    setInputConnection(nr, port);
    mColors[nr] = color;
}

void HeatmapRenderer::execute() {
    std::lock_guard<std::mutex> lock(mMutex);

    // This simply gets the input data for each connection and puts it into a data structure
    for(uint inputNr = 0; inputNr < getNrOfInputData(); inputNr++) {
        Image::pointer input = getStaticInputData<Image>(inputNr);
        if(input->getDataType() != TYPE_FLOAT) {
            throw Exception("Data type of image given to HeatmapRenderer must be FLOAT");
        }

        std::cout << "HEATMAP recieved input" << std::endl;
        mImagesToRender[inputNr] = input;
    }
}

void HeatmapRenderer::draw() {

}

void HeatmapRenderer::draw2D(cl::Buffer PBO, uint width, uint height,
                             Eigen::Transform<float, 3, Eigen::Affine> pixelToViewportTransform, float PBOspacing,
                             Vector2f translation) {

    std::lock_guard<std::mutex> lock(mMutex);
    OpenCLDevice::pointer device = getMainDevice();

    cl::CommandQueue queue = device->getCommandQueue();
    std::vector<cl::Memory> v;
    if(DeviceManager::isGLInteropEnabled()) {
        v.push_back(PBO);
        queue.enqueueAcquireGLObjects(&v);
    }

    // Create an aux PBO
    cl::Buffer PBO2(
            device->getContext(),
            CL_MEM_READ_WRITE,
            sizeof(float)*width*height*4
    );

    std::unordered_map<uint, Image::pointer>::iterator it;
    for(it = mImagesToRender.begin(); it != mImagesToRender.end(); it++) {
        Image::pointer input = it->second;


        if(input->getDimensions() == 2) {
            std::string kernelName = "render2D";
            cl::Kernel kernel(getOpenCLProgram(device), kernelName.c_str());
            // Run kernel to fill the texture

            OpenCLImageAccess::pointer access = input->getOpenCLImageAccess(ACCESS_READ, device);
            cl::Image2D *clImage = access->get2DImage();
            kernel.setArg(0, *clImage);
            kernel.setArg(1, PBO); // Read from this
            kernel.setArg(2, PBO2); // Write to this
            kernel.setArg(3, input->getSpacing().x());
            kernel.setArg(4, input->getSpacing().y());
            kernel.setArg(5, PBOspacing);
            kernel.setArg(6, mColors[it->first].getRedValue());
            kernel.setArg(7, mColors[it->first].getGreenValue());
            kernel.setArg(8, mColors[it->first].getBlueValue());

            // Run the draw 2D kernel
            queue.enqueueNDRangeKernel(
                    kernel,
                    cl::NullRange,
                    cl::NDRange(width, height),
                    cl::NullRange
            );
        } else {
            throw Exception("HeatmapRenderer not implemented for 3D images");
        }

        // Copy PBO2 to PBO
        queue.enqueueCopyBuffer(PBO2, PBO, 0, 0, sizeof(float)*width*height*4);
    }
    if(DeviceManager::isGLInteropEnabled()) {
        queue.enqueueReleaseGLObjects(&v);
    }
    queue.finish();

}

BoundingBox HeatmapRenderer::getBoundingBox() {
    std::vector<Vector3f> coordinates;

    std::unordered_map<uint, Image::pointer>::iterator it;
    for(it = mImagesToRender.begin(); it != mImagesToRender.end(); it++) {
        BoundingBox transformedBoundingBox;
        transformedBoundingBox = it->second->getTransformedBoundingBox();

        MatrixXf corners = transformedBoundingBox.getCorners();
        for(uint j = 0; j < 8; j++) {
            coordinates.push_back((Vector3f)corners.row(j));
        }
    }
    return BoundingBox(coordinates);
}

}