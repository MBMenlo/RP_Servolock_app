#pragma once

#include "rp.h"
#include "StreamingApplication.h"
#include "StreamingManager.h"
#include "ServerNetConfigManager.h"
#include "options.h"

#ifdef Z20_250_12
#include "rp-spi.h"
#include "rp-i2c-max7311.h"
#endif

auto startServer(std::shared_ptr<ServerNetConfigManager> serverNetConfig) -> void;
auto stopNonBlocking(int x) -> void;
auto stopServer(int x) -> void;
