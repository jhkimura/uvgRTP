#pragma once

#include <mutex>
#include <unordered_map>

#include "frame.hh"
#include "runner.hh"
#include "socket.hh"
#include "util.hh"

namespace uvg_rtp {

    typedef rtp_error_t (*packet_handler)(ssize_t, void *, int, uvg_rtp::frame::rtp_frame **);

    class pkt_dispatcher : public runner {
        public:
            pkt_dispatcher();
            ~pkt_dispatcher();

            /* Install a generic handler for an incoming packet
             *
             * Return RTP_OK on successfully
             * Return RTP_INVALID_VALUE if "handler" is nullptr */
            rtp_error_t install_handler(packet_handler handler);

            /* Install receive hook for the RTP packet dispatcher
             *
             * Return RTP_OK on success
             * Return RTP_INVALID_VALUE if "hook" is nullptr */
            rtp_error_t install_receive_hook(void *arg, void (*hook)(void *, uvg_rtp::frame::rtp_frame *));

            /* Start the RTP packet dispatcher
             *
             * Return RTP_OK on success
             * Return RTP_MEMORY_ERROR if allocation of a thread object fails */
            rtp_error_t start(uvg_rtp::socket *socket, int flags);

            /* Stop the RTP packet dispatcher and wait until the receive loop is exited
             * to make sure that destroying the object in media_stream.cc is safe
             *
             * Return RTP_OK on success */
            rtp_error_t stop();

            /* Fetch frame from the frame queue that contains all received frame.
             * pull_frame() will block until there is a frame that can be returned.
             * If "timeout" is given, pull_frame() will block only for however long
             * that value tells it to.
             * If no frame is received within that time period, pull_frame() returns nullptr
             *
             * Return pointer to RTP frame on success
             * Return nullptr if operation timed out or an error occurred */
            uvg_rtp::frame::rtp_frame *pull_frame();
            uvg_rtp::frame::rtp_frame *pull_frame(size_t ms);

            /* Return reference to the vector that holds all installed handlers */
            std::vector<uvg_rtp::packet_handler>& get_handlers();

            /* Return a processed RTP frame to user either through frame queue or receive hook */
            void return_frame(uvg_rtp::frame::rtp_frame *frame);

            /* RTP packet dispatcher thread */
            static void runner(
                uvg_rtp::pkt_dispatcher *dispatcher,
                uvg_rtp::socket *socket,
                int flags,
                std::mutex *exit_mtx
            );

        private:
            std::vector<packet_handler> packet_handlers_;

            /* If receive hook has not been installed, frames are pushed to "frames_"
             * and they can be retrieved using pull_frame() */
            std::vector<uvg_rtp::frame::rtp_frame *> frames_;
            std::mutex frames_mtx_;
            std::mutex exit_mtx_;

            void *recv_hook_arg_;
            void (*recv_hook_)(void *arg, uvg_rtp::frame::rtp_frame *frame);
    };
}
