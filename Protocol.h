#ifndef ZADANIE4_PROTOCOL_H
#define ZADANIE4_PROTOCOL_H
#include <bitset>
#include <array>
#include <regex>
#include <iostream>

class TextProtocol{
private:
    std::string id="",op="",od="",val="",atm="",tim=""; ///identyfikator sesji/operacja/odpowiedz/proby/godzina

public:
    void setID(std::string s){ id=s; }
    void setOP(std::string s){ op=s; }
    void setOD(std::string s){ od=s; }
    void setVAL(std::string s){ val=s; }
    void setATM(std::string s){ atm=s; }
    void setTIM(std::string s){ tim=s; }
    std::string getID(){return id;}
    std::string getOP(){return op;}
    std::string getOD(){return od;}
    std::string getVAL(){return val;}
    std::string getATM(){return atm;}
    std::string getTIM(){return tim;}
    void refreshTime(){
        time_t czas;
        struct tm * aktualnyCzas;
        //time( & czas );
        //aktualnyCzas = localtime( & czas );
        //this->tim=std::to_string(aktualnyCzas->tm_hour)+":"+std::to_string(aktualnyCzas->tm_min)+":"+std::to_string(aktualnyCzas->tm_sec);
        this->tim=std::to_string(time( & czas ));
    }

    std::string getString(){
        std::string s="id#"+id+"/"+"op#"+op+"/"+"od#"+od+"/";
        if(val!=""){
            s+=("val#"+val+"/");
        }
        if(atm!=""){
            s+=("atm#"+atm+"/");
        }
        if(tim!=""){
            s+=("tim#"+tim+"/");
        }
        return s;
    }

    void reset(){
        id=""; op=""; od=""; atm=""; tim=""; val="";
    }

    void reset_soft(){
        op=""; od=""; tim=""; val="";
    }
};

void clearbuffer(char *x,int size){
    for(int i=0; i<size; i++){
        x[i]=NULL;
    }
}

bool check_char(char *c,int sizeC,std::string s){
    for(int i=0; i<sizeC; i++){
        if(c[i]!=s[i]){return false;}
    }
    return true;
}

void analyze (TextProtocol *msg, std::string s){
    std::regex ID("id#\\w*"),OP("op#\\w*"),OD("od#\\w*"),VAL("val#\\d*"),ATM("atm#\\d*"),TIM("tim#d*");
    std::smatch wynik; std::string buffer;

    std::regex_search(s,wynik,ID); buffer=wynik[0]; msg->setID(buffer.erase(0,3)); //std::cout<<msg->getID()<<std::endl;
    std::regex_search(s,wynik,OP); buffer=wynik[0]; msg->setOP(buffer.erase(0,3)); //std::cout<<msg->getOP()<<std::endl;
    std::regex_search(s,wynik,OD); buffer=wynik[0]; msg->setOD(buffer.erase(0,3)); //std::cout<<msg->getOD()<<std::endl;
    std::regex_search(s,wynik,VAL); buffer=wynik[0]; msg->setVAL(buffer.erase(0,4)); //std::cout<<msg->getTIM()<<std::endl;
    std::regex_search(s,wynik,ATM); buffer=wynik[0]; msg->setATM(buffer.erase(0,4)); //std::cout<<msg->getATM()<<std::endl;
    std::regex_search(s,wynik,TIM); buffer=wynik[0]; msg->setTIM(buffer.erase(0,4)); //std::cout<<msg->getTIM()<<std::endl;

}
bool if_only_numbers(std::string s){
    std::smatch invalid_character;
    atoi(s.c_str());
    regex_search(s,invalid_character,(std::regex("\\D")));
    if(invalid_character.empty()){return true;}
    else{return false;}
}
#endif //ZADANIE4_PROTOCOL_H
