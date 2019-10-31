#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <json/json.h>
#include <iostream> // std::cout
#include <string>   // std::string, std::to_string

#include "iolib.h" //自己编写的I/O库函数的函数声明

using namespace std;
#define MAX_LINE 602400

// int a[3][4] = {
//  {0, 1, 2, 3} ,   /*  初始化索引号为 0 的行 */
//  {4, 5, 6, 7} ,   /*  初始化索引号为 1 的行 */
//  {8, 9, 10, 11}   /*  初始化索引号为 2 的行 */
// };

string writeJson()
{ //将数组读成一个字符流
    int a[200][200];
    for (int i = 0; i < 200; i++)
    {
        for (int j = 0; j < 200; j++)
        {
            a[i][j] = i * j;
        }
    }
    int len = sizeof(a) / sizeof(int);
    int col = sizeof(a[0]) / sizeof(int);
    int row = len / col; // 获取二维数组的行数和列数
    Json::Value root;
    Json::FastWriter fast_write;
    int tmp = 0; //表示在一个数组中的序号
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < col; j++)
        {
            string str1 = to_string(tmp++);
            // string str2 = to_string(tmp++);
            root[str1] = to_string(a[i][j]); //存储在json里
        }
    }
    root["row"] = to_string(row);
    root["col"] = to_string(col);
    root["len"] = to_string(len);
    // string out = root.toStyledString();
    string SendBuf = fast_write.write(root);
    // cout << SendBuf << endl;
    cout << SendBuf.size() << endl;
    return SendBuf;
}

//大写字符转为小写字符函数
void my_fun(char *p)
{
    if (p == NULL)
    { //空串
        return;
    }
    for (; *p != '\0'; p++)
    {
        if (*p >= 'A' && *p <= 'Z')
        { //判断字符并进行转换
            *p = *p - 'A' + 'a';
        }
    }
}
int main(int argc, char const *argv[])
{
    struct sockaddr_in sin;
    struct sockaddr_in cin;
    int l_fd;
    int c_fd;
    socklen_t len;                //unsigned int
    char buf[MAX_LINE];           //存储传送内容的缓冲区
    char addr_p[INET_ADDRSTRLEN]; //存储客户端地址的缓冲区
    int port = 8001;              //端口号
    int n;                        //读写字节数
    bzero(&sin, sizeof(sin));     //清空地址结构

    sin.sin_family = AF_INET;         //IPv4通信域
    sin.sin_addr.s_addr = INADDR_ANY; //服务器可以接受任意地址
    sin.sin_port = htons(port);       //端口号转为网络字节序
    if ((l_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { //创建套接字，使用TCP协议
        perror("fail to creat socket");
        exit(1);
    }
    if (bind(l_fd, (struct sockaddr *)&sin, sizeof(sin)) == -1)
    { //将地址和套接字绑定
        perror("fail to bind");
        exit(1);
    }
    if (listen(l_fd, 10) == -1)
    { //开始监听连接请求
        perror("fail to listen");
        exit(1);
    }
    printf("waiting...\n");

    // while(1){
    len = sizeof(sin);
    if ((c_fd = accept(l_fd, (struct sockaddr *)&cin, &len)) == -1)
    { //接受连接请求
        perror("fail to accept");
        exit(1);
    }    
    n = my_read(c_fd, buf, MAX_LINE); //读取客户端传来的信息
    Json::Reader reader;
    Json::Value root;
    string tmp2(buf, strlen(buf));
    if (reader.parse(tmp2, root))
    {
        cout << "可以进行解析了" << endl;
        int out = root["age"].asInt();
        cout << out << endl;
    }
    // n = my_read(c_fd, buf, MAX_LINE); //读取客户端传来的信息
    if (n == -1)
    { //读取出错，my_read()函数做错误处理，读入字节为-1则退出
        exit(1);
    }
    else if (n == 0)
    { //通信的另一端已经关闭
        printf("the connect has been closed\n");
        close(c_fd); //关闭通信用的套接字，注意监听套接字没有关闭
        // continue;   //此处连接处理结束，准备处理下一个连接
    }
    inet_ntop(AF_INET, &cin.sin_addr, addr_p, sizeof(addr_p)); //将客户端地址转换位字符串
    printf("client IP is %s, port is %d\n", addr_p, ntohs(cin.sin_port));
    printf("client is : %s\n", buf);
    // my_fun(buf);
    bzero(buf, MAX_LINE);

    // char buf[MAX_LINE]; //存储传送内容的缓冲区
    string tmp = writeJson();
    strcpy(buf, tmp.c_str());
    // cout<<buf<<endl;
    // n = my_write(c_fd, buf, n);
    write(c_fd, buf, strlen(buf)); // 确定要发送的长度 strlen(buf)
    printf("send byte: %d string: %d", strlen(buf), tmp.size());
    if (n == -1)
    { //写失败则退出
        exit(1);
    }
    sleep(2);                         //休眠2s再接收

    if (close(c_fd) == -1)
    { //通信结束，关闭套接字，准备下一次通信
        perror("fail to close");
        exit(1);
    }
    // }
    return 0;
}
//g++ -std=c++11 tcp_server.cpp -o tcp_server -ljson
