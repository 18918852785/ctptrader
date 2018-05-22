#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class ctp_int{
public:
        ctp_int():i(0),valid(false){}

        int i;
        bool valid;

        void set(int a){
            valid=true;
            i=a;
        }

        bool equal(const ctp_int& j) const {
            return j.valid == valid && j.i == i;
        }
};

class ctp_double{
public:
    ctp_double():i(0){}

    double i;
};


class ctp_price{
public:
    ctp_price(double b,double s):buy(b),sell(s){}
    ctp_price():buy(0),sell(0){}
    double buy,sell;
};

class ctp_codes{
public:
    ctp_codes(){
        data=nullptr;
        count=0;
    }
    int init(const std::vector<std::string> & a){
        delete[] data;

        count = a.size();
        data = new char*[count];
        for(int i=0;i<count;i++){
            data[i] =(char*) a[i].c_str();
        }
        return 0;
    }
    void clear(){
        delete []data;
        count=0;
    }
    ~ctp_codes(){
        delete[] data;
    }
    char** data;
    int count;
};

typedef std::shared_ptr<ctp_codes> ctp_codes_ptr;


