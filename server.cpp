#include <winsock2.h>
#include <iostream>
#include "Protocol.h"

using namespace std;

int main()
{
    WSADATA wsaData;

    int result = WSAStartup( MAKEWORD( 2, 2 ), & wsaData );
    if( result != NO_ERROR ){cout<<"Initialization error."<<endl;}

    SOCKET serverSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if( serverSocket == INVALID_SOCKET)
    {
        std::cout<<"Error creating socket: "+WSAGetLastError();
        WSACleanup();
        return 1;
    }

    sockaddr_in service;
    memset( & service, 0, sizeof( service ) );
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    service.sin_port = htons( 27015 );

    if( bind( serverSocket,( SOCKADDR * ) & service, sizeof( service ) ) == SOCKET_ERROR ) {
        cout<< "bind() failed."<<endl;
        closesocket( serverSocket );
        return 1;
    }

    if( listen( serverSocket, 1 ) == SOCKET_ERROR ) {
        cout<<"Error listening on socket."<<endl;
    }

    SOCKET clientSocket = SOCKET_ERROR;
    SOCKET clientSocket2 = SOCKET_ERROR;

///Zmienne petli Glownej
    int bytesRecv = SOCKET_ERROR,sessionCounter=0,L1,L2,random_number,random_modifier;
    bool client_padl;
    char recvbuf[128]="";
    TextProtocol msg,answ;
    string division;

    srand( time( NULL ) );

///Petla sesji
    cout<<"New session.\n";
    while(true&&answ.getOP()!="QUIT_SERVER"){
        cout<<"Waiting for clients to connect..."<<endl;
        while( clientSocket == SOCKET_ERROR || clientSocket2 == SOCKET_ERROR ) {

            if(clientSocket = accept( serverSocket, NULL, NULL )){
                cout<<"Client_A connected."<<endl;
            }
            if(clientSocket2 = accept( serverSocket, NULL, NULL )){
                cout<<"Client_B connected."<<endl;
            }
        }
        cout<<"Both clients connected."<<endl;
///-----------------Generowanie Sesji---------------------------------
        sessionCounter++;
        random_modifier=(rand()%85)+0;
        random_number=(rand()%15)+random_modifier;
        cout<<"Division in this session: "<<random_modifier<<" to "<<random_modifier+15<<endl;
        cout<<"Random number in this session: "<<random_number<<endl;
///-----------------Glowna petla klienta
        while(bytesRecv!=0 && bytesRecv!=WSAECONNRESET && client_padl==false){
///---------Przydzielanie glosu oraz inicjowanie L
            if(msg.getATM()==""){
                msg.refreshTime();
                msg.setOP("DIVISION");
                msg.setOD(to_string(random_modifier)+"to"+to_string(random_modifier+15));
                send(clientSocket, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_A][SENT]: "<<msg.getString()<<endl;

                msg.reset_soft();

                msg.refreshTime();
                msg.setID("S"+to_string(sessionCounter));
                msg.setOP("SPEAK_L");
                send(clientSocket, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_A][SENT]: "<<msg.getString()<<endl;
                msg.reset_soft();
            }
            else if(atoi(msg.getATM().c_str())==0){///remis, koniec polaczenia
                msg.refreshTime();
                msg.setOP("STOP");
                msg.setOD("TIE");
                send(clientSocket, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_A][SENT]: "<<msg.getString()<<endl;

                msg.reset_soft();

                msg.refreshTime();
                msg.setOP("STOP");
                msg.setOD("TIE");
                send(clientSocket2, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_A][SENT]: "<<msg.getString()<<endl;
                msg.reset_soft();
                break;
            }
            else {
                msg.refreshTime();
                msg.setOP("SPEAK");
                send(clientSocket, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_A][SENT]: "<<msg.getString()<<endl;
                msg.reset_soft();
            }
///---------Odbieranie wiadomosc
            while( bytesRecv == SOCKET_ERROR ) {
                bytesRecv = recv( clientSocket, recvbuf, 74, 0 );

                if(bytesRecv==0||bytesRecv==WSAECONNRESET) {cout<<"Connection closed.\n"; break;}
                if(bytesRecv<0) {client_padl=true;}

                cout<<"[RECEIVED]: "<<recvbuf<<endl;
                analyze(&answ,recvbuf);
                bytesRecv = SOCKET_ERROR; break;
            }
///---------Odbieranie glosu i analiza otrzymanej  wiadomosci
            if(msg.getATM()==""){
                L1=atoi(answ.getVAL().c_str());
                msg.refreshTime();
                msg.setOP("STOP");
                send(clientSocket,msg.getString().c_str(),msg.getString().size(),0);
                cout<<"[To Client_A][SENT]: "<<msg.getString()<<endl;
                msg.reset_soft();
            }
            else if(atoi(answ.getVAL().c_str())==random_number){ /// wykrywanie wygranej
                msg.refreshTime();
                msg.setOP("STOP");
                msg.setOD("WIN");
                send(clientSocket, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_A][SENT]: "<<msg.getString()<<endl;

                msg.reset_soft();

                msg.setOP("STOP");
                msg.setOD("LOST");
                msg.refreshTime();
                send(clientSocket2, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_A][SENT]: "<<msg.getString()<<endl;
                msg.reset_soft();
                break;
            }
            else{
                msg.refreshTime();
                msg.setOP("STOP");
                msg.setOD("NO");
                send(clientSocket, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_A][SENT]: "<<msg.getString()<<endl;
                msg.reset_soft();
            }
///---------DRUGI GRACZ
///---------Przydzielanie glosu oraz inicjowanie L
            if(msg.getATM()==""){
                msg.refreshTime();
                msg.setOP("DIVISION");
                msg.setOD(to_string(random_modifier)+"to"+to_string(random_modifier+15));
                send(clientSocket2, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_B][SENT]: "<<msg.getString()<<endl;

                msg.reset_soft();

                msg.refreshTime();
                msg.setOP("SPEAK_L");
                msg.setOD("");
                send(clientSocket2, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_B][SENT]: "<<msg.getString()<<endl;
                msg.reset_soft();
            }
            else {
                msg.refreshTime();
                msg.setOP("SPEAK");
                send(clientSocket2, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_B][SENT]: "<<msg.getString()<<endl;
                msg.reset_soft();
            }
///---------Odbieranie wiadomosc
            while(bytesRecv==SOCKET_ERROR){
                bytesRecv = recv( clientSocket2, recvbuf, 74, 0 );

                if(bytesRecv==0||bytesRecv==WSAECONNRESET) {cout<<"Connection closed.\n"; break;}
                if(bytesRecv<0) {client_padl=true;}

                cout<<"[RECEIVED]: "<<recvbuf<<endl;
                analyze(&answ,recvbuf);
                bytesRecv = SOCKET_ERROR; break;
            }
///---------Odbieranie glosu i analiza otrzymanej  wiadomosci
            if(msg.getATM()==""){
                L2=atoi(answ.getVAL().c_str());
                msg.setATM(to_string((L1+L2)/2));
                int x=atoi(msg.getATM().c_str());
                msg.setATM(to_string(++x)); cout<<msg.getATM()<<endl;

                msg.refreshTime();
                msg.setOP("STOP");
                send(clientSocket2, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_B][SENT]: "<<msg.getString()<<endl;
                msg.reset_soft();
            }
            else if(atoi(answ.getVAL().c_str())==random_number){
                msg.refreshTime();
                msg.setOP("STOP");
                msg.setOD("WIN");
                send(clientSocket2, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_B][SENT]: "<<msg.getString()<<endl;

                msg.reset_soft();

                msg.setOP("STOP");
                msg.setOD("LOST");
                msg.refreshTime();
                send(clientSocket, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_B][SENT]: "<<msg.getString()<<endl;
                msg.reset_soft();
                break;
            }
            else {
                msg.refreshTime();
                msg.setOP("STOP");
                msg.setOD("NO");
                send(clientSocket2, msg.getString().c_str(), msg.getString().size(), 0 );
                cout<<"[To Client_B][SENT]: "<<msg.getString()<<endl;
                msg.reset_soft();
            }
            int x=atoi(msg.getATM().c_str()); msg.setATM(to_string(--x));
        }
        cout<<"Closing session...\n";
        msg.reset();
        answ.reset();

        shutdown(clientSocket,SD_BOTH); shutdown(clientSocket2,SD_BOTH);
        clientSocket=SOCKET_ERROR; clientSocket2=SOCKET_ERROR;
        //bytesRecv=SOCKET_ERROR;
        client_padl=false;
    }
}