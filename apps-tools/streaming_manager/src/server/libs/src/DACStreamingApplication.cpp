#include <chrono>
#include <iostream>
#include <fstream>
#include <functional>
#include <cstdlib>
#include "DACStreamingApplication.h"
#include "AsioNet.h"

#define UNUSED(x) [&x]{}()

CDACStreamingApplication::CDACStreamingApplication(CDACStreamingManager::Ptr _streamingManager,CGenerator::Ptr _gen) :
    m_gen(_gen),
    m_streamingManager(_streamingManager),
    m_Thread(),
    mtx(),
    m_ReadyToPass(0),
    m_isRun(false),
    m_isRunNonBloking(false)
{
    m_GenThreadRun.test_and_set();
}

CDACStreamingApplication::~CDACStreamingApplication() {
    stop(true);
}

auto CDACStreamingApplication::run() -> void {
    m_isRun = true;
    m_isRunNonBloking = false;
    try {
        m_streamingManager->run();
        m_Thread = std::thread(&CDACStreamingApplication::genWorker, this);
        if (m_Thread.joinable()){
            m_Thread.join();
        }
    }
    catch (const asio::system_error &e)
    {
        std::cerr << "Error: CDACStreamingApplication::run(), " << e.what() << std::endl;
    }

}

auto CDACStreamingApplication::runNonBlock() -> void {
    m_isRun = true;
    m_isRunNonBloking = true;    
    try {
        m_streamingManager->run(); // MUST BE INIT FIRST for thread logic
        m_Thread = std::thread(&CDACStreamingApplication::genWorker, this);        
    }
    catch (const asio::system_error &e)
    {
        std::cerr << "Error: CStreamingApplication::runNonBlock(), " << e.what() << std::endl;
    }
}

auto CDACStreamingApplication::stop(bool wait) -> bool{
    mtx.lock();
    bool state = false;
    if (m_isRun){
        m_GenThreadRun.clear();
        if (wait) {
            m_Thread.join();
        }else{
            while(isRun());
        }
        m_streamingManager->stop();
        m_gen->stop();
        state = true;
    }
    mtx.unlock();
    return state;
}

void CDACStreamingApplication::genWorker()
{
       
    auto timeNow = std::chrono::system_clock::now();
    auto curTime = std::chrono::time_point_cast<std::chrono::milliseconds >(timeNow);
    auto value = curTime.time_since_epoch();

    long long int timeBegin = value.count();
    uintmax_t counter = 0;
    uintmax_t passCounter = 0;
    uint8_t   skipBuffs = 0;
    m_gen->prepare();
try{
    while (m_GenThreadRun.test_and_set())
    {

        // oscNotify(overFlow, m_oscRate, m_adc_mode, m_adc_bits, m_WriteBuffer_ch1, m_size_ch1, m_WriteBuffer_ch2, m_size_ch2);
        // ++counter;

        // timeNow = std::chrono::system_clock::now();
        // curTime = std::chrono::time_point_cast<std::chrono::milliseconds >(timeNow);
        // value = curTime.time_since_epoch();

          
        // if ((value.count() - timeBegin) >= 5000) {
        //     std::cout << "Lost samples: " << m_lostRate  << "\n";
        //     counter = 0;
        //     m_lostRate = 0;
        //     passCounter = 0;
        //     timeBegin = value.count();
        // }

        // if (!m_StreamingManager->isFileThreadWork()){
            
        //     if (m_StreamingManager->notifyStop){
        //         if (m_StreamingManager->isOutOfSpace())
        //             m_StreamingManager->notifyStop(0);
        //         else 
        //             m_StreamingManager->notifyStop(1);
        //         m_StreamingManager->notifyStop = nullptr;                
        //     }
        // }
    }
    
}catch (std::exception& e)
	{
		std::cerr << "Error: oscWorker() " << e.what() << std::endl ;
	}
    m_isRun = false;
}

void CDACStreamingApplication::signalHandler(const asio::error_code &_error, int _signalNumber)
{
    UNUSED(_error);
    static_cast<void>(_signalNumber);
    stop(true);
}
