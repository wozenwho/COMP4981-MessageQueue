#define BUFFER_SIZE         256
#define FILENAME_SIZE       64

/**
 * Message structure used by the Server to send data
 * to the client.
 */
typedef struct msgBuffer
{
    long msgType;
    int priorityLevel;
    char data[BUFFER_SIZE];

} msgBuffer;

/**
 * Message structure used by the Client to send requests 
 * to the server.
 */
typedef struct reqBuffer
{
    long msgType;
    int clientID;
    int priorityLevel;
    char data[FILENAME_SIZE];
    int flags;
} reqBuffer;

/**
 * Thread parameter struct used to pass the message queue's key
 * and clientID to the client's reading thread.
 */
typedef struct clientInfo
{
    key_t key;
    long clientID;
} clientInfo;