#define MAX_MESSAGE_DATA  (4096 - 16)
#define MSGHDRSIZE      (sizeof(Msg) - MAXMESGDATA)
#define BUFFER_SIZE         256
#define FILENAME_SIZE       64

typedef struct msgBuffer
{
    long msgIdentifier;
    int priorityLevel;
    char data[BUFFER_SIZE];

} msgBuffer;

typedef struct reqBuffer
{
    long msgType;
    int clientID;
    int priorityLevel;
    char data[FILENAME_SIZE];
} reqBuffer;

typedef struct clientInfo
{
    key_t key;
    long clientID;

} clientInfo;