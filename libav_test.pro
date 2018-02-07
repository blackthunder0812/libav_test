TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

LIBS += -Wl,-Bstatic -Wl,--start-group -lswscale -lavdevice -lavformat -lavcodec -lavutil -lavfilter -lswresample -Wl,--end-group \
        -Wl,-Bdynamic -Wl,--start-group -lm -lpthread -ldl -lva -lva-x11 -lva-drm -lX11 -ldrm -lwavpack \
                                        -lcrypto -lssl -lz -lvpx -llzma -lbz2 -lopus -lvdpau -lx264 -lx265 \
                                        -lopenh264 -lmp3lame -lfdk-aac -lssh -Wl,--end-group
