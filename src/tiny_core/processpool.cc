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

#include <tiny_core/processpool.h>
#include <tiny_core/process.h>
#include <tiny_core/eventloop.h>
#include <tiny_core/master.h>
#include <tiny_core/timerid.h>
#include <tiny_base/buffer.h>
#include <tiny_base/signalmanager.h>
#include <tiny_base/log.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <map>
#include <string>
#include <memory>
#include <boost/bind.hpp>

void test_parent_MessageCallback(Connection *con, Buffer *buf, Time time)
{
    LOG(Debug) << "[parent] get message:"
              << buf->getAll() << std::endl;
}

void test_parent_CloseCallback(Connection *con)
{
    LOG(Debug) << "[parent] one connection close" << std::endl;
}

void test_parent_period_print(void)
{
    LOG(Debug) << "[parent] print every second\n";
}

ProcessPool *ProcessPool::m_pPoolInstance = nullptr;

ProcessPool::ProcessPool()
    : m_pEventLoop(new EventLoop()),
      m_pMaster(new Master(this, m_pEventLoop.get(), 0, "master")),
      m_pProcess(nullptr),
      m_nListenSocketFd(-1)
{
    m_nPid = getpid();
    m_pPoolInstance = this;
    LOG(Debug) << "class ProcessPoll constructor\n";
}

void ProcessPool::init()
{
    m_pMaster->init();
    m_nListenSocketFd = m_pMaster->getListenSocket();
}

void ProcessPool::createProcess(int nums)
{
    std::vector<pair> pair_tmp;
    std::vector<pid_t> pids_tmp;
    pair_tmp.reserve(nums);
    pids_tmp.reserve(nums);

    //first-step:create nums process
    for (int i = 0; i < nums; i++)
    {
        int socketpairFds[2];
        int res = socketpair(AF_UNIX, SOCK_STREAM, 0, socketpairFds);
        if (res == -1)
            handle_error("[processpool] socketpair error:");

        pid_t pid = fork();
        if (pid < 0)
        {
            LOG(Debug) << "[processpool] fork error\n";
        }
        else if (pid == 0)
        {
            //Child process:
            //1.establish channel with Parent;
            //2.set signal handlers
            //3.create listen server
            m_pProcess = std::make_shared<Process>(std::to_string(i),
                                                   i, socketpairFds);
            m_pProcess->setAsChild(int(getpid()));
            m_pProcess->setSignalHandlers();
            m_pProcess->createListenServer(m_nListenSocketFd);
            goto WAIT;
        }
        else
        {
            //Parent process:
            //Store information of process.
            //And set something after forking.
            //1.push socketpair
            //2.push pid
            LOG(Debug) << "[processpool] create process(" << pid << ")\n";
            pair_tmp.push_back({socketpairFds[0],
                                socketpairFds[1]});
            pids_tmp.push_back(pid);
        }
    }

    //Parent process second-step:build pipe with every child process
    for (auto t : pids_tmp)
        m_nPids.push_back(t);
    for (unsigned int i = 0; i < pair_tmp.size(); i++)
    {
        int pair[2];
        pair[0] = pair_tmp[i].d1;
        pair[1] = pair_tmp[i].d2;
        LOG(Debug) << "[parent]:establish connection with child(" << m_nPids[i] << ")\n";
        std::shared_ptr<SocketPair> pipe(new SocketPair(m_pEventLoop.get(), pair));
        m_nPipes.push_back(pipe);

        pipe->setParentSocket(int(m_nPids[i]));
        pipe->setMessageCallback(boost::bind(&test_parent_MessageCallback, _1, _2, _3)); //FIXME:
        pipe->setCloseCallback(boost::bind(&test_parent_CloseCallback, _1));
        setSignalHandlers();
    }
    assert(m_nPids.size() == m_nPipes.size());

WAIT:
    start();
}

void ProcessPool::setSignalHandlers()
{
    std::vector<Signal> signals = {
        Signal(SIGINT, "SIGINT", "killAll", parentSignalHandler),
        Signal(SIGTERM, "SIGTERM", "killSoftly", parentSignalHandler),
        Signal(SIGCHLD, "SIGCHLD", "childdead", parentSignalHandler),
        Signal(SIGUSR1, "SIGUSR1", "restart", parentSignalHandler),
        Signal(SIGUSR2, "SIGUSR2", "reload", parentSignalHandler),
        Signal(SIGQUIT, "QIGQUIT", "quit softly", parentSignalHandler),
        Signal(SIGPIPE, "SIGPIPE", "socket close", parentSignalHandler),
        Signal(SIGHUP, "SIGHUP", "reconfigure", parentSignalHandler),
    };

    for (auto t : signals)
        m_nSignalManager.addSignal(t);
}

void ProcessPool::start()
{
    if (!m_pProcess)
    {
        //Parent process
        assert(m_nPipes.size() == m_nPids.size());

        for (unsigned int index = 0; index < m_nPipes.size(); index++)
        {
            LOG(Debug) << "[parent] I will send to child(" << m_nPids[index]
                      << ") message every one seconds\n";

            m_pEventLoop->runEvery(1, boost::bind(&test_parent_period_print));
        }
        status = 1;
        while (status)
        {
            m_pMaster->work();
            if (status_reconfigure || status_reconfigure)
            {
                LOG(Debug) << "[processpool] create new pool\n";
                //reload config-file
                //create new processpool
                // createProcess(2);
            }
            if (status_terminate || status_quit_softly)
            {
                LOG(Debug) << "[processpool] end\n";
                status = 0;
            }
        }
    }
    else
    {
        //Child process
        m_pProcess->start();
    }
}

void ProcessPool::killAll()
{
    //This function will be invoked by master.
    LOG(Debug) << "[parent] kill " << m_nPids.size() << " chilern\n";
    for (auto t : m_nPids)
    {
        LOG(Debug) << "[parent] kill child(" << t << ")\n";
        int res = kill(t, SIGTERM);
        if (res == 0)
        {
            LOG(Debug) << "[parent] kill child (" << t << ") successfully\n";
            m_fDestoryProcess(t);
        }
    }
}

void ProcessPool::killSoftly()
{
    LOG(Debug) << "[parent] kill chilern softly\n";
    //the difference with killAll
    //FIXME:
}

ProcessPool::~ProcessPool()
{
    LOG(Debug) << "class ProcessPoll destructor\n";
}
