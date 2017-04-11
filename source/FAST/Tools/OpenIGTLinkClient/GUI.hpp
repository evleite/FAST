#ifndef FAST_OPENIGTLINKCLIENT_GUI_HPP_
#define FAST_OPENIGTLINKCLIENT_GUI_HPP_

#include "FAST/Visualization/Window.hpp"

class QPushButton;
class QLineEdit;
class QLabel;

namespace fast {

class IGTLinkStreamer;
class OpenIGTLinkClient;

class GUI : public Window {
    FAST_OBJECT(GUI)
    public:
        void record();
        void connect();
        void updateMessages();
    private:
        GUI();

        QPushButton* connectButton;
        QLineEdit* address;
        QLineEdit* port;
        bool mConnected;
        SharedPointer<IGTLinkStreamer> mStreamer;
        SharedPointer<OpenIGTLinkClient> mClient;

        QLabel* recordingInformation;
        QPushButton* recordButton;
        QLineEdit* storageDir;
};

} // end namespace fast

#endif