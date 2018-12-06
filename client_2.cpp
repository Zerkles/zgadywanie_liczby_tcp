#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Protocol.h"
using namespace std;

int main()
{
    WSADATA wsaData;

    int result = WSAStartup( MAKEWORD( 2, 2 ), & wsaData );
    if( result != NO_ERROR )
        cout<<"Initialization error."<<endl;

    SOCKET mainSocket = socket( AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if( mainSocket == INVALID_SOCKET )
    {
        cout<<"Error creating socket: "+WSAGetLastError()<<endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in service;
    memset( & service, 0, sizeof( service ) );
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    service.sin_port = htons( 27015 );

    if( connect( mainSocket,( SOCKADDR * ) & service, sizeof( service ) ) == SOCKET_ERROR )
    {
        cout<<"Failed to connect."<<endl;
        WSACleanup();
        return 1;
    }

    cout<<"Succesfully connected."<<endl;
///Zmienne Petli Glownej
    int bytesRecv = SOCKET_ERROR;
    char recvbuf[128]=""; int L=2;
    TextProtocol msg,answ;
///Petla glowna klienta
    while(bytesRecv!=WSAECONNRESET&&bytesRecv!=0){
///Odbieranie wiadomosci
        while( bytesRecv == SOCKET_ERROR ) {
            clearbuffer(recvbuf,strlen(recvbuf));
            bytesRecv = recv( mainSocket, recvbuf, 128, 0 );

            if(bytesRecv==0||bytesRecv==WSAECONNRESET) {cout<<"Connection closed.\n"; break;}
            if(bytesRecv<0) {return 1;}

            cout<<"[RECEIVED]: "<<recvbuf<<endl;
            analyze(&answ,recvbuf);
            if(answ.getOP()=="SPEAK"){cout<<"Attempts left: "<<answ.getATM()<<endl;}
            bytesRecv = SOCKET_ERROR; break;
        }
///Analiza wiadomosci
        if(answ.getOP()=="SPEAK"){
            cout<<"Your turn!"<<endl;

            string odp;
            do{
                cout<<"Enter Answer: ";
                cin>>odp;
                if(odp=="QUIT"){break;}
                if(!if_only_numbers(odp)||odp.size()>10){
                    cout<<"Invalid input, try again.\n"<<endl;
                }
            }while(!if_only_numbers(odp)||odp.size()>10);

            msg.setVAL(odp);
            if(msg.getVAL()=="QUIT") {
                shutdown(mainSocket,SD_BOTH);
                return 0;
            }

            msg.refreshTime();
            msg.setOP("SPEAK");
            send(mainSocket, msg.getString().c_str(), msg.getString().size(), 0 );
            cout<<"[SENT]: "<<msg.getString().c_str()<<endl;
            msg.reset_soft();
        }
        else if(answ.getOD()=="NO"&&answ.getOP()=="STOP"){
            cout<<"You answer is wrong.\n";
            cout<<"Waiting for second player...\n";
        }
        else if(answ.getOP()=="SPEAK_L"){
            msg.setID(answ.getID());

            string LS;
            do{
                cout<<"Enter one, odd digit:";
                cin>>LS;
                atoi(LS.c_str());

                if(!if_only_numbers(LS)||atoi(LS.c_str())>10||atoi(LS.c_str())%2==0||LS.size()>10){
                    cout<<"Invalid input, try again.\n"<<endl;
                }
            }while(!if_only_numbers(LS)||atoi(LS.c_str())>10||atoi(LS.c_str())%2==0||LS.size()>10);

            L=atoi(LS.c_str());

            msg.refreshTime();
            msg.setVAL(to_string(L));
            msg.setOP("SPEAK_L");
            send(mainSocket, msg.getString().c_str(), msg.getString().size(), 0 );
            cout<<"[SENT]: "<<msg.getString()<<endl;
            msg.reset_soft();
            cout<<"Waiting for second player...\n";
        }
        else if(answ.getOP()=="DIVISION"){
            string odp=answ.getOD();
            std::smatch wynikA,wynikB;

            std::regex_search(odp,wynikA,regex("\\d+"));
            std::regex_search(odp,wynikB,regex("to\\d+"));
            string B= wynikB[0]; B.erase(B.begin(),B.begin()+=2);

            cout<<"Server has draw number from "<<wynikA[0]<<" to "<<B<<endl;
        }
        else if(answ.getOD()=="TIE"||answ.getOD()=="WIN"||answ.getOD()=="LOST"||answ.getATM()=="0"){
            if (answ.getOD()=="WIN"){cout<<"You won the game."<<endl;}
            if(answ.getOD()=="LOST"){cout<<"You lost the game."<<endl;}
            if(answ.getATM()=="0"){cout<<"You are out of attempts!\n";}
            if(answ.getOD()=="TIE"){cout<<"You tied the game."<<endl;}
            shutdown(mainSocket,SD_BOTH);
            break;
        }
    }
}