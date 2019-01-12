/*
*Author:GeneralSandman
*Code:https://github.com/GeneralSandman/TinyWeb
*E-mail:generalsandman@163.com
*Web:www.generalsandman.cn
*/

/*---XXX---
*
****************************************
*
*/

#include <tiny_base/buffer.h>
#include <tiny_core/client.h>
#include <tiny_core/netaddress.h>
#include <tiny_core/connection.h>
#include <tiny_core/eventloop.h>
#include <tiny_core/factory.h>
#include <tiny_core/timerid.h>
#include <tiny_http/http_protocol.h>

using namespace std;

void connectCallback(Connection *newCon)
{
    std::cout << "get a new connection:["
              << newCon->getLocalAddress().getIpPort()
              << "--" << newCon->getPeerAddress().getIpPort() << "]" << std::endl;
    newCon->send("I'm tinyweb fcgi client");
}

void getDataCallback(Connection *con, Buffer *data, Time time)
{

    std::cout << "get data frome connection:["
              << con->getLocalAddress().getIpPort()
              << "--" << con->getPeerAddress().getIpPort() << "]:";
    std::cout << data->getAll() << "("
              << time.toString() << ")" << std::endl;
    // con->send("I'm client");
}

void writeCompleteCallback(Connection *con)
{

    std::cout << "write to connection complete:["
              << con->getLocalAddress().getIpPort()
              << "--" << con->getPeerAddress().getIpPort() << "]" << std::endl;
}

void closeCallback(Connection *con)
{
    std::cout << "lose a connection:["
              << con->getLocalAddress().getIpPort()
              << "--" << con->getPeerAddress().getIpPort() << "]" << std::endl;
}

void test1()
{
    EventLoop *loop = new EventLoop();
    //Don't use factory and protocol,
    //we set callback.
    Client *tcpClient = new Client(loop, nullptr);
    tcpClient->setConnectCallback(boost::bind(&connectCallback, _1));
    tcpClient->setMessageCallback(boost::bind(&getDataCallback, _1, _2, _3));
    tcpClient->setWriteCompleteCallback(boost::bind(&writeCompleteCallback, _1));
    tcpClient->setCloseCallback(boost::bind(&closeCallback, _1));

    loop->runAfter(10, std::bind(&EventLoop::quit, loop));

    NetAddress serveraddress("127.0.0.1:9000");
    NetAddress clientaddress(9595);
    bool retry = false;
    bool keepconnect = false;
    tcpClient->start();
    tcpClient->connect(clientaddress, serveraddress, retry, keepconnect);

    loop->loop();

    //delete obj in right order.
    delete tcpClient;
    delete loop;
    //basic test successfully.
}

void test2()
{
    EventLoop *loop = new EventLoop();
    //Don't use factory and protocol,
    //we set callback.
    Client *tcpClient = new Client(loop, nullptr);
    tcpClient->setConnectCallback(boost::bind(&connectCallback, _1));
    tcpClient->setMessageCallback(boost::bind(&getDataCallback, _1, _2, _3));
    tcpClient->setWriteCompleteCallback(boost::bind(&writeCompleteCallback, _1));
    tcpClient->setCloseCallback(boost::bind(&closeCallback, _1));

    loop->runAfter(10, std::bind(&EventLoop::quit, loop));

    NetAddress serveraddress("172.17.0.3:9000");
    NetAddress clientaddress("127.0.0.1:9595");
    bool retry = true;
    bool keepconnect = false;
    tcpClient->start();
    tcpClient->connect(clientaddress, serveraddress, retry, keepconnect);

    loop->loop();

    //delete obj in right order.
    delete tcpClient;
    delete loop;
    //basic test successfully.
}

void test3()
{
    //no retry , no keepconnect
    EventLoop *loop = new EventLoop();
    Protocol *clientProtocol = new FcgiClientProtocol();
    Factory *clientFactory = new ClientFactory(loop, clientProtocol);
    Client *tcpClient = new Client(loop, clientFactory);

    loop->runAfter(10, std::bind(&EventLoop::quit, loop));

    NetAddress serveraddress("172.17.0.3:9000");
    NetAddress clientaddress(9595);
    bool retry = false;
    bool keepconnect = false;
    tcpClient->start();
    tcpClient->connect(clientaddress, serveraddress, retry, keepconnect);

    loop->loop();

    //delete obj in right order.
    delete tcpClient;
    delete clientFactory;
    delete clientProtocol;
    delete loop;
}

void test4()
{
    //no retry , no keepconnect
    EventLoop *loop = new EventLoop();
    Protocol *clientProtocol = new FcgiClientProtocol();
    Factory *clientFactory = new ClientFactory(loop, clientProtocol);
    Client *tcpClient = new Client(loop, clientFactory);

    loop->runAfter(10, std::bind(&EventLoop::quit, loop));

    NetAddress serveraddress("172.17.0.3:9000");
    NetAddress clientaddress("127.0.0.1:9595");
    bool retry = false;
    bool keepconnect = false;
    tcpClient->start();
    tcpClient->connect(clientaddress, serveraddress, retry, keepconnect);

    loop->loop();

    //delete obj in right order.
    delete tcpClient;
    delete clientFactory;
    delete clientProtocol;
    delete loop;
}

int main()
{
    test1();
    // test2();
    // test3();
    // test4();
}