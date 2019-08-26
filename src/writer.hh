#pragma once

#include <stdint.h>

#include "conn.hh"
#include "frame.hh"
#include "queue.hh"
#include "socket.hh"

namespace kvz_rtp {
    class writer : public connection {

    public:
        writer(std::string dst_addr, int dst_port);
        writer(std::string dst_addr, int dst_port, int src_port);
        ~writer();

        // open socket for sending frames
        rtp_error_t start();

        /* TODO: */
        rtp_error_t push_frame(uint8_t *data, uint32_t data_len, int flags);

        /* TODO: remove */
        sockaddr_in get_out_address();

    private:
        std::string dst_addr_;
        int dst_port_;
        int src_port_;
        sockaddr_in addr_out_;
    };
};
