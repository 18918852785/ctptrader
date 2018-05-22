// ctp-market.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ctp-market-saver.h"
#include "ctp-market-global.h"

#include <windows.h>
#include "../../include/string/utf8_string.h"
#include "../../include/config/bs_config.h"


ctp_market_saver::ctp_market_saver()
{
}

int ctp_market_saver::start()
{
    return 0;
}

int ctp_market_saver::save(CThostFtdcDepthMarketDataField* d)
{
    auto filename = get_filename(d);
    if(filename.size()==0)//无效code
        return -1;
    //找出文件句柄
    auto f = get_filehandle(filename);
    if(!f)return -1;

    auto ret = write_market(f, d);
    return ret;
}

std::string ctp_market_saver::get_filename(CThostFtdcDepthMarketDataField* d)
{
    std::string ret;
    auto exchange = string_trim(d->ExchangeID);
    auto code = string_trim(d->InstrumentID);
    auto day = string_trim(d->TradingDay);
    ret = code+"."+exchange+"."+day+".dat";
    return ret;
}

FILE* ctp_market_saver::get_filehandle(const std::string& name)
{
    auto it = m_files.find(name);
    if(it == m_files.end()){
        auto ret = open_file(name);
        m_files[name]=ret;
        return ret;
    }
    return it->second;
}

int ctp_market_saver::write_market(FILE* f,CThostFtdcDepthMarketDataField* d)
{
    if(!f || !d)return -1;
    fwrite(d,sizeof(CThostFtdcDepthMarketDataField),1,f);
    fflush(f);
    return 0;
}

FILE* ctp_market_saver::open_file(const std::string& name)
{
    auto path = cosmos::bs_config::get_exe_folder();
    path += "\\log";
    ::CreateDirectoryA(path.c_str(),nullptr);
    path += "\\market";
    ::CreateDirectoryA(path.c_str(),nullptr);

    path += "\\" + name;
    auto ret = _fsopen( path.c_str(), "a+",_SH_DENYWR);
    return ret;
}

