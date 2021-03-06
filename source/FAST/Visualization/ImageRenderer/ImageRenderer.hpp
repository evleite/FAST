#ifndef IMAGERENDERER_HPP_
#define IMAGERENDERER_HPP_

#include "FAST/Visualization/Renderer.hpp"
#include "FAST/Data/Image.hpp"

namespace fast {

class ImageRenderer : public Renderer {
    FAST_OBJECT(ImageRenderer)
    public:
        void addInputConnection(ProcessObjectPort port);
        BoundingBox getBoundingBox();
    private:
        ImageRenderer();
        void execute();
        void draw();
        void draw2D(
                cl::Buffer PBO,
                uint width,
                uint height,
                Eigen::Transform<float, 3, Eigen::Affine> pixelToViewportTransform,
                float PBOspacing,
                Vector2f translation
        );

        std::unordered_map<uint, Image::pointer> mImagesToRender;
        std::unordered_map<uint, GLuint> mTexturesToRender;
        std::unordered_map<uint, Image::pointer> mImageUsed;

        cl::Kernel mKernel;

        std::mutex mMutex;

};

}




#endif /* IMAGERENDERER_HPP_ */
