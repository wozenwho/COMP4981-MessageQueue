#include "Header.h"
#define MAX_FILE_NAME       64

#define IFLAGS  (IPC_CREAT | IPC_EXCL)
#define ERR     ((struct databuf *) -1)

pthread_t ioThread;

void client();
void server();
int create_msgQ(key_t);
int get_msgQ(key_t);
int send_message(int, msgBuffer*);
int read_message(int, long, msgBuffer*);
int exitProgram(int);
int remove_msgQ(int);
void* readThrdFunc(void*);

// Global variables
int running;
int msgqID;


/**
 * Compile with:
 * g++ -Wall -o Main Main.cpp -pthread -ggdb
 * 
 * 
 */
int main(void)
{   
    std::string mode;
    
    std::cout << "Please input program mode (server / client)" << std::endl;
    std::cin >> mode;
    
    if (mode.compare(server_str) == 0)
    {
        std::cout << "Server selected." << std::endl;
        server();
    }   
    else if (mode.compare(client_str) == 0)
    {
        std::cout << "Client selected." << std::endl;
        client();
    }
    else
    {
        std::cerr << "Invalid mode selected: " << mode << std::endl;
    }

    

    //open read thread
    //pthread_join it. Wooo. YUSS.
    if (mode.compare(server_str) == 0)
    {
        msgqID = get_msgQ(9999);
        if (remove_msgQ(msgqID) == -1)
        {
            std::cerr << "Error closing Message Queue, do it manually." << std::endl;
        }
        else
        {
            std::cout << "Message Queue closed." << std::endl;
        }
    }

    return 0;
} // End of main


void client()
{
    key_t key = (key_t) 9999;
    int running = 1;
   
    msgBuffer messageBuffer;
    int priorityLevel;
    long clientID;
    std::string fileName;
    int result;
    
    pthread_t readThread;
 
    msgqID = get_msgQ(key);
    if (msgqID == -1)
    {
        std::cerr << "Could not open Message Queue." << std::endl;
        return;
    }
    else
    {
        std::cout << "Message Queue retrieved: " << msgqID << std::endl;
    }
    
    // Get clientID
    std::cout << "Please enter a clientID" << std::endl;
    std::cin >> clientID;
    messageBuffer.msgIdentifier = clientID;
    // Get file to request from server
    std::cout << "Please enter a filename:" << std::endl;
    std::cin >> fileName;
    strcpy(messageBuffer.data, fileName.c_str());
    // Get priority level of client
    std::cout << "Please enter a priority level (3 - high | 2 - med | 1 - low)" << std::endl;
    std::cin >> priorityLevel;
    messageBuffer.priorityLevel = priorityLevel;

    // Output client info to stderr
    std::cerr << "clientID: " << messageBuffer.msgIdentifier << std::endl;
    fprintf(stderr, "filename: %s\n", messageBuffer.data);
    std::cerr << "priority: " << messageBuffer.priorityLevel << std::endl;

    result = send_message(msgqID, &messageBuffer);
    if (result == -1)
    {
        std::cerr << "Failed to write to Message Queue." << std::endl;
    }
    else
    {
        // clientInfo* threadParam = (clientInfo*) malloc(sizeof(clientInfo));
        // threadParam->key = key;
        // threadParam->clientID = clientID;
        // pthread_create(&readThread, NULL, readThrdFunc, (void*) threadParam);
    }

    pthread_join(readThread, NULL);
    std::cout << "Client " << clientID << " completed." << std::endl;

} // End of client



void server()
{
    key_t key = (key_t) 9999;
    int pid[MAX_CLIENTS];
    int currentNumClients = 0;
    int running = 1;
    char sendBuffer[BUFFER_SIZE];
    int result;
    msgBuffer messageBuffer;
    std::ifstream fileStream;
    
    // Create message queue
    msgqID = create_msgQ(key);

    if (msgqID == -1)
    {
        std::cerr << "Could not create Message Queue." << std::endl;
        return;
    }
    else
    {
        std::cout << "Message Queue created: " << msgqID << std::endl;
    }

    // Sets all values of pid to -1
    memset(pid, -1, MAX_CLIENTS);

    // Read message queue for client connection requests
    while (running)
    {

        result = read_message(msgqID, CLIENT_REQUEST, &messageBuffer);
        if (result == -1)
        {
            std::cerr << "read error!" << std::endl;
            // fprintf(stderr, "%s\n", strerror(errno));
        }
        else
        {
            std::cerr << "I read something!" << std::endl;
            std::cerr << messageBuffer.msgIdentifier << std::endl;
            fprintf(stderr, "filename: %s\n", messageBuffer.data);

            pid[currentNumClients] = fork();
            if (pid[currentNumClients] == -1)
            {
                std::cerr << "Fork error!" << std::endl;
            }
        }
        
        // Fork a new process to handle connection i/o
    }

    /* Test file IO code */
    // fileStream.open("test1.txt");
    // if (fileStream.is_open())
    // {
    //     while (!fileStream.eof())
    //     {
    //         memset(sendBuffer, '\0', BUFFER_SIZE);
    //         fileStream.read(sendBuffer, BUFFER_SIZE);
    //         fprintf(stderr, sendBuffer);
    //     }
    //     fileStream.close();
    //     std::cout << std::endl;
    // }

} // End of server




int create_msgQ(key_t key)
{
    int qid;
    if ((qid = msgget(key, IPC_CREAT | IPC_EXCL | 0644)) == -1)
    {
        return -1;
    }
    return qid;
} // End of create_msgQ




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



int remove_msgQ(int msgqID)
{
    if (msgctl(msgqID, IPC_RMID, 0) == -1)
    {
        return -1;
    }
    return 0;
} // End of remove_msgQ



//it's supposed to be long int data. 
int read_message(int msgqID, long type, msgBuffer* message)
{
    int result;
    int length;

    length = sizeof(msgBuffer) - sizeof(long);

    std::cout << "Wat." << std::endl;
    if ((result = msgrcv(msgqID, message, length, type, 0)) == -1)
    {
        std::cout << "Afterwat." << std::endl;
        fprintf(stderr, "%s\n", strerror(errno));
        return -1;
    }
    return result;
} // end of read_message




// Is the second argument supposed to be a buffer? shit.
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




int client_request(int msgqID, std::string filename)
{

}




void terminate()
{

}




int exitProgram(int msgqID)
{
    int result; 
    remove_msgQ(msgqID);
}




void* readThrdFunc(void* threadParam)
{
    msgBuffer messageBuffer;
    clientInfo* ci = (clientInfo*) threadParam;
    key_t key = ci->key;
    long clientID = ci->clientID;
    int result;
    int running = 1;

    while (running)
    {
        result = read_message(msgqID, clientID, &messageBuffer);
        if (result > 0)
        {
            fprintf(stderr, messageBuffer.data);
        }
        if (result > 0 && result < BUFFER_SIZE)
        {
            std::cout << "I think I reached the end." << std::endl;
        }
    }
    free (threadParam);
}