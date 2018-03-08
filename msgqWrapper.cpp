#include "msgqWrapper.h"

/**
 * FUNCTION HEADER
 */ 
int create_msgQ(key_t key)
{
    int qid;
    if ((qid = msgget(key, IPC_CREAT | IPC_EXCL | 0644)) == -1)
    {
        return -1;
    }
    return qid;
} // End of create_msgQ



/**
 * FUNCTION HEADER
 */ 
int get_msgQ(key_t key)
{
    int qid;
    if ((qid = msgget(key, IPC_CREAT)) == -1)
    {
        // Message queue does not exist
        return -1;
    }
    return qid;
} // End of get_msgQ



/**
 * FUNCTION HEADER
 */
int remove_msgQ(int msgqID)
{
    if (msgctl(msgqID, IPC_RMID, 0) == -1)
    {
        return -1;
    }
    return 0;
} // End of remove_msgQ



/**
 * FUNCTION HEADER
 */ 
int read_message(int msgqID, long type, msgBuffer* message)
{
    int result;
    int length;

    length = sizeof(msgBuffer) - sizeof(long);
    if ((result = msgrcv(msgqID, message, length, type, 0)) == -1)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    return result;
} // end of read_message



/**
 * FUNCTION HEADER
 */
int read_request(int msgqID, long type, reqBuffer* request)
{
    int result;
    int length;

    length = sizeof(reqBuffer) - sizeof(long);

    if ((result = msgrcv(msgqID, request, length, type, 0)) == -1)
    {
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    return result;
}



/**
 * FUNCTION HEADER
 */
int send_message(int msgqID, msgBuffer* message)
{
    int result;
    int length;

    length = sizeof(msgBuffer) - sizeof(long);

    if ((result = msgsnd(msgqID, message, length, 0)) == -1)
    {
        return -1;
    }
    return result;
} // end of send_message



/**
 * FUNCTION HEADER
 */
int send_request(int msgqID, reqBuffer* request)
{
    int result;
    int length;

    length = sizeof(reqBuffer) - sizeof(long);

    if ((result = msgsnd(msgqID, request, length, 0)) == -1)
    {
        return -1;
    }
    return result;
}
