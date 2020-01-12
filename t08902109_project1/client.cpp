#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <netinet/in.h>
#include <list> 
#include <sstream>
#include <vector> 
#include <chrono>
#include<sys/time.h>


using namespace std;

// C++ DIY split
void split(const std::string& s,
    std::vector<std::string>& sv,
                   const char delim = ' ') {
    sv.clear();
    std::istringstream iss(s);
    std::string temp;

    while (std::getline(iss, temp, delim)) {
        sv.emplace_back(std::move(temp));
    }

    return;
}

struct Packet{
    long long sent_time;
};

class Info {
public:
    string host;
    int port;
    int timeout;
    int packets_num;
    Info(string h, int p, int t, int p_num) {
        host = h;
        port = p;
        timeout = t;
        packets_num = p_num;
    }
};

int main(int argc , char *argv[])
{
    vector<Info> info_list;
    int t = 1000; // default timeout is 1000
    int p = 0; // default is 0

    for(int i = 1; i < argc; ++i) {
        string arg;
        int n;
        std::istringstream iss( argv[i] );
        iss >> arg;
        if(arg == "-n") {
            ++i;
            std::istringstream iss( argv[i] );
            iss >> n;
            //cout << "number of package is " << n << endl;
            p = n;
        } else if (arg == "-t") {
            ++i;
            std::istringstream iss( argv[i] );
            iss >> n;
              //cout << "timeout is " << n << endl;
              t = n;
        } else {
            vector<string> sv;
            vector<string> res;
            split(arg, sv, ':');
            for (const auto& arg : sv) {
                res.push_back(arg);
            }
            string host;
            host = res[0];
            int port;   
            port = stoi(res[1]);
            info_list.push_back(Info(host, port, t, p));
        }
    }

    for (int i = 0; i < info_list.size(); ++i) {
        //socket bulid
        int sockfd = 0;
        sockfd = socket(AF_INET , SOCK_STREAM , 0);
        if (sockfd == -1){
            printf("Fail to create a socket.");
        }
        //socket的連線
        struct sockaddr_in info;
        bzero(&info,sizeof(info));
        info.sin_family = PF_INET;
        Info curr = info_list[i];
        info.sin_addr.s_addr = inet_addr(curr.host.c_str());
        info.sin_port = htons(curr.port);
        int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
        if(err==-1){
            cout << "timeout when connect to " << inet_ntoa(info.sin_addr) << ":" << htons(info.sin_port) << endl;
        }
        for(int i = 0; i < curr.packets_num || curr.packets_num == 0; ++i) {
            struct timeval sent_time,recv_time;
            Packet *p = new Packet;
            gettimeofday(&sent_time,NULL);
            send(sockfd, p, sizeof(p),0);
            Packet *recv_p = new Packet;
            recv(sockfd, recv_p, sizeof(recv_p),0);
            gettimeofday(&recv_time,NULL);
            long long sent_long = sent_time.tv_sec*1000 + sent_time.tv_usec/1000;
            long long recv_long = recv_time.tv_sec*1000 + recv_time.tv_usec/1000;
            long long rtt = recv_long - sent_long;
            if(rtt > curr.timeout) {
                cout << "timeout when connect to " << inet_ntoa(info.sin_addr) << ":" << htons(info.sin_port) << endl;
            } else {
                cout << "recv from " << inet_ntoa(info.sin_addr) << ":" << htons(info.sin_port);
                cout << ", RTT = " <<  rtt << " msec"<< endl;
            }
            close(sockfd);
        }
    }
    return 0;
}