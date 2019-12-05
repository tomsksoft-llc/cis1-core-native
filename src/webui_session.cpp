/*
 *    TomskSoft CIS1 Core
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Mokin Innokentiy [mia@tomsksoft.com]
 *
 */

#include "webui_session.h"

#include <cis1_cwu_protocol/protocol.h>

std::string get_address(const boost::process::environment& env)
{
    if(auto addr = env.find("webui_internal_address"); addr != env.end())
    {
        return addr->to_string();
    }

    return {};
}

uint16_t get_port(const boost::process::environment& env)
{
    if(auto port = env.find("webui_internal_port"); port != env.end())
    {
        try
        {
            return std::stoul(port->to_string());
        }
        catch(...)
        {
            return {};
        }
    }

    return {};
}

std::shared_ptr<webui_session> init_webui_session(const cis1::context& ctx)
{
        auto& env = ctx.env();

        std::string address = get_address(env);
        uint16_t port = get_port(env);

        boost::system::error_code ec;

        auto real_address = boost::asio::ip::make_address(address, ec);
        if(ec)
        {
            return nullptr;
        }

        auto session = std::make_shared<webui_session>();

        session->connect(
                {real_address, port},
                ec);
        if(ec)
        {
            return nullptr;
        }

        session->run();

        return session;
}

webui_session::webui_session()
    : client_(ctx_)
{}

webui_session::~webui_session()
{
    if(running_)
    {
        client_.disconnect();
    }

    if(working_thread_.joinable())
    {
        working_thread_.join();
    }
}

void webui_session::connect(
        const boost::asio::ip::tcp::endpoint& ep,
        boost::system::error_code& ec)
{
    client_.connect(ep, ec);
}

void webui_session::auth(
        const cis1::session& session)
{
    cis1::cwu::session_auth dto;
    dto.session_id = session.session_id();

    make_transaction().send(dto);
}

void webui_session::run()
{
    working_thread_ = std::thread(
            [&]()
            {
                ctx_.run();
                running_ = false;
            });
    running_ = true;
}

cis1::proto_utils::transaction webui_session::make_transaction(uint32_t id)
{
    return cis1::proto_utils::transaction(client_.get_queue(), id);
}
