/*
 *Author:GeneralSandman
 *Code:https://github.com/GeneralSandman/TinyWeb
 *E-mail:generalsandman@163.com
 *Web:www.dissigil.cn
 */

/*---XXX---
 *
 ****************************************
 *
 */

#include <tiny_base/log.h>
#include <tiny_core/connection.h>
#include <tiny_core/factory.h>
#include <tiny_core/protocol.h>

#include <vector>

std::string
getName(const Protocol* p)
{
    std::stringstream s;
    s << typeid(*p).name();
    std::string tmp(s.str().c_str());

    std::string res;
    for (unsigned int i = 0; i < tmp.size(); i++)
        if (!(tmp[i] >= 48 && tmp[i] <= 57))
            res += tmp[i];
    return res;
}

//----------Reflect api----------//
std::map<std::string, createProtocol> Reflect::m_nProts;

Protocol* getInstanceByPointer(const Protocol* p)
{
    return Reflect::getReflectInstance().getProtocolByName(getName(p));
}

//----------Protocol api----------//

Protocol::Protocol()
{
    // LOG(Debug) << "class Protocol constructor\n";
}

void Protocol::makeConnection()
{
    //It is used by Factory
    //user can't change this function.
    connectionMade();
}

void Protocol::getMessage(const std::string& data)
{
    //It is used by Factory
    //user can't change this function.
    dataReceived(data);
}

void Protocol::writeComplete()
{
    //It is used by Factory
    //user can't change this function.
    writeCompletely();
}

void Protocol::loseConnection()
{
    //It is used by Factory
    //user can't change this function.
    connectionLost();
}

void Protocol::sendMessage(const std::string& data)
{
    m_pConnection->send(data);
}

void Protocol::sendBuffer(buffer_t* buffer)
{
    m_pConnection->send(buffer);
}

void Protocol::sendChain(const chain_t* chain)
{
    m_pConnection->send(chain);
}

void Protocol::closeProtocol()
{
    m_pFactory->closeProtocol(this);
}

void Protocol::closeProtocolAfter(int seconds)
{
    m_pFactory->closeProtocolAfter(this, seconds);
}

void Protocol::connectionMade()
{
    //It can be override user.
}

void Protocol::dataReceived(const std::string& data)
{
    //It can be override by user.
}

void Protocol::writeCompletely()
{
    //It can be override by user.
}

void Protocol::connectionLost()
{
    //It can be override by user.
}

Protocol::~Protocol()
{
    // LOG(Debug) << "class Protocol destructor\n";
}

RegistProtocol(Protocol);

//----------end Protocol api----------//
