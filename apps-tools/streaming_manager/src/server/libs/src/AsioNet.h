#pragma once

#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <functional>
#include <system_error>
#include <cstdint>
#include <memory>
#include <deque>

#include "neon_asm.h"
#include "asio.hpp"
#include "EventHandlers.h"
#include "AsioSocket.h"

//#define  SOCKET_BUFFER_SIZE 65536
//#define  FIFO_BUFFER_SIZE  SOCKET_BUFFER_SIZE * 3

using  namespace std;
using  namespace asio;

namespace  asionet {

    class CAsioNet {
    public:

        using Ptr = shared_ptr<CAsioNet>;

        static Ptr Create(Mode _mode,Protocol _protocol,string _host , string _port);
        CAsioNet(asionet::Mode _mode,Protocol _protocol,string _host , string _port);
        ~CAsioNet();

        void Start();
        void Stop();
        void disconnect();
        void addCallServer_Connect(function<void(string host)> _func);
        void addCallServer_Disconnect(function<void(string host)> _func);
        void addCallServer_Error(function<void(error_code error)> _func);

        void addCallClient_Connect(function<void(string host)> _func);
        void addCallClient_Disconnect(function<void(string host)> _func);
        void addCallClient_Error(function<void(error_code error)> _func);

        void addCallSend(function<void(error_code error,size_t)> _func);
        void addCallReceived(function<void(error_code error,uint8_t*,size_t)> _func);

        bool SendData(bool async,CAsioSocket::send_buffer _buffer,size_t _size);
    Protocol GetProtocol() { return  m_protocol;};
        bool IsConnected();

        static uint8_t *BuildPack(
                uint64_t _id ,
                uint64_t _lostRate ,
                uint32_t _oscRate  ,
                uint32_t _resolution ,
                uint32_t _adc_mode,
                uint32_t _adc_bits,
                const void *_ch1 ,
                size_t _size_ch1 ,
                const void *_ch2 ,
                size_t _size_ch2 ,
                size_t &_buffer_size );

        static void BuildPack(
                CAsioSocket::send_buffer buffer ,
                uint64_t _id ,
                uint64_t _lostRate ,
                uint32_t _oscRate  ,
                uint32_t _resolution ,
                uint32_t _adc_mode,
                uint32_t _adc_bits,
                const void *_ch1 ,
                size_t _size_ch1 ,
                const void  *_ch2 ,
                size_t _size_ch2 ,
                size_t &_buffer_size);

        static bool     ExtractPack(
                CAsioSocket::send_buffer _buffer ,
                size_t _size ,
                uint64_t &_id ,
                uint64_t &_lostRate ,
                uint32_t &_oscRate  ,
                uint32_t &_resolution ,
                uint32_t &_adc_mode,
                uint32_t &_adc_bits,
                CAsioSocket::send_buffer &_ch1 ,
                size_t &_size_ch1 ,
                CAsioSocket::send_buffer  &_ch2 ,
                size_t &_size_ch2);

    private:

        CAsioNet(const CAsioNet &) = delete;
        CAsioNet(CAsioNet &&) = delete;
        void SendServerStop();

        Mode m_mode;
        Protocol m_protocol;
        string m_host;
        string m_port;
        asio::io_service m_Ios;
        asio::io_service::work m_Work;
        asio::thread *m_asio_th;
        bool m_IsRun;
        shared_ptr<CAsioSocket> m_server;

    };
}



