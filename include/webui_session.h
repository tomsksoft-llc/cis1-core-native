#pragma once

#include <thread>
#include <memory>

#include <boost/asio.hpp>

#include <cis1_proto_utils/transaction.h>
#include <cis1_cwu_transport/ccwu_tcp_client.h>

#include "context.h"

class webui_session
{
public:
    webui_session();
    ~webui_session();
    void connect(
            const boost::asio::ip::tcp::endpoint& ep,
            boost::system::error_code& ec);
    void run();
    cis1::proto_utils::transaction make_transaction(uint32_t id = 0);
private:
    std::thread working_thread_;
    boost::asio::io_context ctx_;
    cis1::cwu::tcp_client client_;
    bool running_ = false;
};

std::shared_ptr<webui_session> init_webui_session(const cis1::context& ctx);
