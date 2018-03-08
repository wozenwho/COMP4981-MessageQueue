#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "mesg.h"
#include "msgqWrapper.h"

#define STD_IN          0
#define STD_OUT         1
#define STD_ERR         2
#define MAX_MODE_SIZE   32
#define MAX_CLIENTS     16

#define TRUE            1
#define FALSE           0

#define client_str      "client"
#define server_str      "server"

// Message types
#define CLIENT_REQUEST      20
#define HIGH_PRIORITY       4
#define MED_PRIORITY        3
#define LOW_PRIORITY        2 
#define ERROR_MSG           111111

// Flag types
#define TRANSFER_COMPLETE   123123

extern int msgqID;