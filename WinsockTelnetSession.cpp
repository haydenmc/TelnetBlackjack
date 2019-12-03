#include "WinsockTelnetSession.h"

#include <iostream>

#pragma region Constants
static const unsigned int DEFAULT_RECEIVE_BUFFER_LENGTH = 512u;
#pragma endregion

#pragma region Public methods
WinsockTelnetSession::WinsockTelnetSession(SOCKET socket) : 
    socket(socket)
{ }

void WinsockTelnetSession::Start()
{
    // Send a bunch of configuration
    // Set NVT mode to say that I will echo back characters.
    int iSendResult;
    unsigned char willEcho[3] = { 0xff, 0xfb, 0x01 };
    iSendResult = send(socket, (char *)willEcho, 3, 0);

    // Set NVT requesting that the remote system not/dont echo back characters
    unsigned char dontEcho[3] = { 0xff, 0xfe, 0x01 };
    iSendResult = send(socket, (char *)dontEcho, 3, 0);

    // Set NVT mode to say that I will supress go-ahead. Stops remote clients from doing local linemode.
    unsigned char willSGA[3] = { 0xff, 0xfb, 0x03 };
    iSendResult = send(socket, (char *)willSGA, 3, 0);

    // Send a banner
    iSendResult = send(socket, "Hello world!", 12, 0);

    // Receive until the peer shuts down the connection
    std::string received;
    char recvbuf[DEFAULT_RECEIVE_BUFFER_LENGTH];
    int iResult, bufferFilledTo = 0;
    int recvbuflen = DEFAULT_RECEIVE_BUFFER_LENGTH;
    while (true)
    {
        iResult = recv(socket, (char*)(recvbuf + bufferFilledTo), recvbuflen - bufferFilledTo, 0);
        if (iResult > 0)
        {
            // We've received data - activate callback
            std::cout << "Bytes received: " << iResult << std::endl;
            bufferFilledTo += iResult;
            if (this->onReceiveCallback)
            {
                this->onReceiveCallback(recvbuf, bufferFilledTo);
            }
        }
        else if (iResult == 0)
        {
            // This means the client closed the connection.
            std::cout << "Client connection closing." << std::endl;
            if (this->onClosedCallback)
            {
                this->onClosedCallback("Client closed the connection.");
            }
            break;
        }
        else
        {
            // Some error occurred.
            auto err = WSAGetLastError();
            std::cerr << "Error receiving from client: '" << err << std::endl;
            closesocket(socket);
            if (this->onClosedCallback)
            {
                this->onClosedCallback("Error receiving data from client: " + err);
            }
            break;
        }
    }
}

void WinsockTelnetSession::Send(std::string message)
{

}

void WinsockTelnetSession::Close()
{

}

void WinsockTelnetSession::OnReceive(
        std::function<BufferAction(char buffer[], unsigned int bufferFilled)> callback)
{
    this->onReceiveCallback = callback;
}

void WinsockTelnetSession::OnClosed(
    std::function<void(std::string reason)> callback)
{
    this->onClosedCallback = callback;
}
#pragma endregion
