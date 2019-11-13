#pragma once

#include <thread>
#include <memory>

#include <boost/asio.hpp>

#include <cis1_proto_utils/transaction.h>
#include <cis1_cwu_transport/ccwu_tcp_client.h>

#include "context.h"
#include "session.h"

/**
 * \brief Represents TCP session with webui-server
 */
class webui_session
{
public:
    /**
     * \brief Constructs webui_session instance
     */
    webui_session();

    ~webui_session();

    /**
     * \brief Connects to webui-server
     * @param[in] ep Server Endpoint
     * @param[out] ec
     */
    void connect(
            const boost::asio::ip::tcp::endpoint& ep,
            boost::system::error_code& ec);

    /**
     * \brief Authenticate on webui-server
     * @param[in] session
     */
    void auth(const cis1::session& session);

    /**
     * \brief Run webui_session
     */
    void run();

    /**
     * \brief Create new transaction
     */
    cis1::proto_utils::transaction make_transaction(uint32_t id = 0);

private:
    std::thread working_thread_;
    boost::asio::io_context ctx_;
    cis1::cwu::tcp_client client_;
    bool running_ = false;
};

/**
 * \brief Initialize new webui_session
 * \return webui_session or std::nullopt on error
 * @param[in] ctx
 */
std::shared_ptr<webui_session> init_webui_session(const cis1::context& ctx);
