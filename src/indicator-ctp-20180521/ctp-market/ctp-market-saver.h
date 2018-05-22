#pragma once
#include "ctp-market-export.h"
#include "ctp-market-callback.h"

#include <string>
#include <unordered_map>

struct CThostFtdcDepthMarketDataField;

// �����Ǵ� ctp-market.dll ������
class CTPMARKET_API ctp_market_saver {
public:
	ctp_market_saver(void);

    int start();

    int save(CThostFtdcDepthMarketDataField* d);

protected:
    std::string get_filename(CThostFtdcDepthMarketDataField* d);
    FILE* get_filehandle(const std::string& name);
    int write_market(FILE* f,CThostFtdcDepthMarketDataField* d);

    FILE* open_file(const std::string& name);

    std::unordered_map<std::string,FILE*> m_files;
};

