#pragma once
#include "Header.h"

/**
 * Function declarations of the Unix Message Queue system functions.
 */ 

extern int msgqID;

int create_msgQ(key_t key);
int get_msgQ(key_t key);
int remove_msgQ(int msgqID);
int read_message(int msgqID, long type, msgBuffer* message);
int read_request(int msgqID, long type, reqBuffer* request);
int send_message(int msgqID, msgBuffer* message);
int send_request(int msgqID, reqBuffer* request);