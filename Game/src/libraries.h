#pragma once 

// cpp libraries
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <memory>
#include <sstream>
#include <random>
#include <set>
// unix libraries
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
// my libraries
#include "core/logger.h"
#include "core/socket_reader.h"
#include "core/socket_writer.h"
#include "core/msg_queue.h"
#include "core/server_messages.h"

