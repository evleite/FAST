fast_add_sources(
    Streamer.cpp
    Streamer.hpp
    ImageFileStreamer.cpp
    ImageFileStreamer.hpp
    ManualImageStreamer.cpp
    ManualImageStreamer.hpp
    AffineTransformationFileStreamer.cpp
    AffineTransformationFileStreamer.hpp
)
if(FAST_MODULE_OpenIGTLink)
    fast_add_sources(
        IGTLinkStreamer.hpp
        IGTLinkStreamer.cpp
    )
    fast_add_test_sources(
        Tests/IGTLinkStreamerTests.cpp
        Tests/DummyIGTLServer.cpp
        Tests/DummyIGTLServer.hpp
    )
endif()
if(FAST_MODULE_Kinect)
    fast_add_sources(
            KinectStreamer.cpp
            KinectStreamer.hpp
    )
endif()
fast_add_test_sources(
    Tests/ImageFileStreamerTests.cpp
)
fast_add_python_interfaces(
	ImageFileStreamer.i
)