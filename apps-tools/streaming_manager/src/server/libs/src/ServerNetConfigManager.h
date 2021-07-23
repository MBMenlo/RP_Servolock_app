#pragma once
#include "common/stream_settings.h"
#include "AsioBroadcastSocket.h"
#include "NetConfigManager.h"
#include "EventHandlers.h"

class ServerNetConfigManager : public  CStreamSettings{
public:
    enum class Errors{
        CANNT_SET_DATA,
        SERVER_INTERNAL,
        BREAK_RECEIVE_SETTINGS,
        BROADCAST_ERROR
    };

    enum class Commands{
        GET_NEW_SETTING,
        STOP_STREAMING,
        START_STREAMING
    };
    ServerNetConfigManager(std::string defualt_file_settings_path, std::string host,std::string port);
    ~ServerNetConfigManager();

    auto startBroadcast(asionet_broadcast::CAsioBroadcastSocket::ABMode mode,std::string host,std::string port) -> void;
    auto isConnected() -> bool;
    auto sendServerStarted() -> bool;
    auto sendServerStopped() -> bool;

    auto addHandlerError(std::function<void(ServerNetConfigManager::Errors)> _func) -> void;
    auto addHandler(ServerNetConfigManager::Commands event,std::function<void()> _func) -> void;

private:
    enum class States{
        NORMAL,
        GET_DATA
    };

    std::shared_ptr<asionet_broadcast::CAsioBroadcastSocket> m_pBroadcast;
    std::shared_ptr<CNetConfigManager> m_pNetConfManager;

    auto receiveCommand(uint32_t command) -> void;
    auto receiveValueStr(std::string key,std::string value) -> void;
    auto receiveValueInt(std::string key,uint32_t value) -> void;
    auto receiveValueDouble(std::string key,double value) -> void;
    auto serverError(std::error_code error) -> void;

    States m_currentState;
    EventList<Errors> m_errorCallback;
    EventList<> m_callbacks;
    std::string m_file_settings;
};