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
int send_request(int, reqBuffer*);
int read_message(int, long, msgBuffer*);
int read_request(int, long, reqBuffer*);
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
   
    reqBuffer requestBuffer;
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
    
    // Set message type
    requestBuffer.msgType = CLIENT_REQUEST;
    // Get clientID
    std::cout << "Please enter a clientID" << std::endl;
    std::cin >> clientID;
    requestBuffer.clientID = clientID;
    // Get file to request from server
    std::cout << "Please enter a filename:" << std::endl;
    std::cin >> fileName;
    strcpy(requestBuffer.data, fileName.c_str());
    // Get priority level of client
    std::cout << "Please enter a priority level (3 - high | 2 - med | 1 - low)" << std::endl;
    std::cin >> priorityLevel;
    requestBuffer.priorityLevel = priorityLevel;

    result = send_request(msgqID, &requestBuffer);
    if (result == -1)
    {
        std::cerr << "Failed to write to Message Queue." << std::endl;
    }
    else
    {
        clientInfo* threadParam = (clientInfo*) malloc(sizeof(clientInfo));
        threadParam->key = key;
        threadParam->clientID = clientID;
        pthread_create(&readThread, NULL, readThrdFunc, (void*) threadParam);
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
    reqBuffer requestBuffer;
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
    memset(pid, -2, MAX_CLIENTS);

    // Read message queue for client connection requests
    while (running)
    {

        result = read_request(msgqID, CLIENT_REQUEST, &requestBuffer);
        if (result == -2)
        {
            std::cerr << "read error!" << std::endl;
            // fprintf(stderr, "%s\n", strerror(errno));
        }
        else
        {   
            // Output client info to stderr
            std::cerr << "Received request from clientID: " << requestBuffer.clientID << std::endl;
            fprintf(stderr, "Requested file: %s\n", requestBuffer.data);

            // Only the parent process forks
            if (pid[currentNumClients == -2])
            {
                pid[currentNumClients] = fork();
            }

            if (pid[currentNumClients] == -1)
            {
                std::cerr << "Fork error!" << std::endl;
            }

            // Executed by child process
            if (pid[currentNumClients] == 0)
            {   
                int prio = 0;
                switch(requestBuffer.priorityLevel)
                {
                    case 3:
                        prio = -20;
                        break;
                    case 2:
                        prio = 0;
                        break;
                    case 1:
                        prio = 19;
                        break;
                }

                setpriority(PRIO_PROCESS, 0, prio);
                std::cout << "priority: " << getpriority(PRIO_PROCESS, 0) << std::endl;

                fileStream.open(requestBuffer.data);
                if (!fileStream.is_open())
                {
                    std::cerr << "Could not open file." << std::endl;
                }
                else
                {
                    messageBuffer.msgType = requestBuffer.clientID;
                    while (!fileStream.eof())
                    {
                        memset(messageBuffer.data, '\0', BUFFER_SIZE);
                        fileStream.read(messageBuffer.data, BUFFER_SIZE);
                        //TODO: ADD PRIORITY.

                        send_message(msgqID, &messageBuffer);
                    }
                    fileStream.close();
                }
            } 
            // Executed by parent process
            else if (pid[currentNumClients > 0])
            {
                currentNumClients++;
            }
        }
    } // End of while-running loop
} // End of server




int exitProgram(int msgqID)
{
    int result; 
    remove_msgQ(msgqID);
} // End of exitProgram




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
} // End of readThrdFunc