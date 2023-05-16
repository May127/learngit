#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<iostream>
#include<functional>
#include<string>

using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*基于muduo网络开发服务器程序
1.组合
*/

class ChatServer
{
public:
// 初始化TcpServer
    ChatServer(EventLoop *loop,
                const InetAddress &listenAddr,
                const string &nameArg)
        :_server(loop, listenAddr, nameArg),_loop(loop)
    {
        // 通过绑定器设置回调函数
        _server.setConnectionCallback(bind(&ChatServer::onConnection,this, _1));

        _server.setMessageCallback(bind(&ChatServer::onMessage,this, _1, _2, _3));
        // 设置EventLoop的线程个数
        _server.setThreadNum(10);
    }
    // 启动ChatServer服务
    void start()
    {
    _server.start();
    }


private:
    // TcpServer绑定的回调函数，当有新连接或连接中断时调用
    void onConnection(const TcpConnectionPtr &conn){
        if(conn->connected()){
            cout<<conn->peerAddress().toIpPort()<<"->"<<
            conn->localAddress().toIpPort()<<"state:online"<<endl;            
        }else{
            cout<<conn->peerAddress().toIpPort()<<"->"<<
            conn->localAddress().toIpPort()<<"state:onoffline"<<endl;     
            conn->shutdown();  //close(fd)
            //_loop->quit();
        }

    }
    // TcpServer绑定的回调函数，当有新数据时调用
    void onMessage(const TcpConnectionPtr &conn,
    Buffer *buffer,
    Timestamp time){
        string buf=buffer->retrieveAllAsString();
        cout<<"recv data:"<<buf<<" time:"<<time.toString()<<endl;
        conn->send(buf);
    }
    TcpServer _server;
    EventLoop *_loop;
};

int main(){
    EventLoop loop;
    InetAddress addr("127.0.0.1",6000);
    ChatServer server(&loop,addr,"ChatServer");

    server.start();
    loop.loop();  //epoll_wait以阻塞方式等待用户连接，已连接用户的读写事件等。
    return 0;
}