// Api.dll 行情调用示例。进程位数须与 DLL 一致（x86 或 x64）。调用约定是 stdcall。
// Result、ErrInfo 为 GBK。Result 多为 TSV：行以 \n 分隔，列以 \t 分隔。
// ErrInfo 建议 >= 256 字节，Result 建议 >= 1MB。
// 连接返回 >=0 为 ConnectionID，失败返回 -1。其余接口 true 成功。
// Market：0 深圳  1 上海  2 北交所。扩展行情的市场号以 TdxExHq_GetMarkets 为准。
// K 线 Category：0~3 为 5/15/30/60 分钟，4 日，5 周，6 月，7 起为 1 分钟及扩展周期。
// A 股常见端口 7709，L2 常见 7719。扩展行情常见 7727 / 7721。

#include <iostream>
#include <Windows.h>

// 连接 A 股行情。账号由授权方提供。
typedef int (__stdcall* TdxHq_ConnectFn)(const char* IP, unsigned short Port, const char* Account, const char* Password, char* Result, char* ErrInfo);
// 断开 A 股连接。
typedef void (__stdcall* TdxHq_DisconnectFn)(int ConnectionID);
// 市场证券数量。Result 是数量，不是字符串。
typedef bool (__stdcall* TdxHq_GetSecurityCountFn)(int ConnectionID, BYTE Market, unsigned short* Result, char* ErrInfo);
// 证券列表。Count 入为请求条数，出为实际条数。
typedef bool (__stdcall* TdxHq_GetSecurityListFn)(int ConnectionID, BYTE Market, unsigned short Start, unsigned short* Count, char* Result, char* ErrInfo);
// 个股 K 线。指数请用 TdxHq_GetIndexBars。
typedef bool (__stdcall* TdxHq_GetSecurityBarsFn)(int ConnectionID, BYTE Category, BYTE Market, const char* Zqdm, unsigned short Start, unsigned short* Count, char* Result, char* ErrInfo);
// 指数 K 线，含涨跌家数。
typedef bool (__stdcall* TdxHq_GetIndexBarsFn)(int ConnectionID, BYTE Category, BYTE Market, const char* Zqdm, unsigned short Start, unsigned short* Count, char* Result, char* ErrInfo);
// 当日分时。
typedef bool (__stdcall* TdxHq_GetMinuteTimeDataFn)(int ConnectionID, BYTE Market, const char* Zqdm, char* Result, char* ErrInfo);
// 历史分时。Date 为 yyyyMMdd。
typedef bool (__stdcall* TdxHq_GetHistoryMinuteTimeDataFn)(int ConnectionID, BYTE Market, const char* Zqdm, int Date, char* Result, char* ErrInfo);
// 分笔成交。
typedef bool (__stdcall* TdxHq_GetTransactionDataFn)(int ConnectionID, BYTE Market, const char* Zqdm, unsigned short Start, unsigned short* Count, char* Result, char* ErrInfo);
// 历史分笔。
typedef bool (__stdcall* TdxHq_GetHistoryTransactionDataFn)(int ConnectionID, BYTE Market, const char* Zqdm, unsigned short Start, unsigned short* Count, int Date, char* Result, char* ErrInfo);
// 五档行情。Market 与 Zqdm 等长，Count 入出参。
typedef bool (__stdcall* TdxHq_GetSecurityQuotesFn)(int ConnectionID, BYTE Market[], const char* Zqdm[], unsigned short* Count, char* Result, char* ErrInfo);
// 公司信息目录。
typedef bool (__stdcall* TdxHq_GetCompanyInfoCategoryFn)(int ConnectionID, BYTE Market, const char* Zqdm, char* Result, char* ErrInfo);
// 公司信息正文。
typedef bool (__stdcall* TdxHq_GetCompanyInfoContentFn)(int ConnectionID, BYTE Market, const char* Zqdm, const char* FileName, int Start, int Length, char* Result, char* ErrInfo);
// 除权除息。
typedef bool (__stdcall* TdxHq_GetXDXRInfoFn)(int ConnectionID, BYTE Market[], const char* Zqdm[], int Count, char* Result, char* ErrInfo);
// 财务简表。
typedef bool (__stdcall* TdxHq_GetFinanceInfoFn)(int ConnectionID, BYTE Market[], const char* Zqdm[], int Count, char* Result, char* ErrInfo);
// 集合竞价。
typedef bool (__stdcall* TdxHq_GetCallAuctionDataFn)(int ConnectionID, BYTE Market, const char* Zqdm, int Start, int* Count, char* Result, char* ErrInfo);
// 十档行情。需要 L2。
typedef bool (__stdcall* TdxHq_GetSecurityQuotes10Fn)(int ConnectionID, BYTE Market[], const char* Zqdm[], unsigned short* Count, char* Result, char* ErrInfo);
// 买卖队列。需要 L2。
typedef bool (__stdcall* TdxHq_GetBuySellQueueFn)(int ConnectionID, BYTE Market[], const char* Zqdm[], int Count, char* Result, char* ErrInfo);
// 逐笔成交。需要 L2。
typedef bool (__stdcall* TdxHq_GetDetailTransactionDataFn)(int ConnectionID, BYTE Market, const char* Zqdm, int Start, unsigned short* Count, char* Result, char* ErrInfo);
// 逐笔成交原始数据。需要 L2。
typedef bool (__stdcall* TdxHq_GetDetailOriginalTransactionDataFn)(int ConnectionID, BYTE Market, const char* Zqdm, int Start, unsigned short* Count, char* Result, char* ErrInfo);
// 逐笔委托。需要 L2。
typedef bool (__stdcall* TdxHq_GetDetailOrderDataFn)(int ConnectionID, BYTE Market, const char* Zqdm, int Start, unsigned short* Count, char* Result, char* ErrInfo);
// 板块文件。BlockFile 空则默认概念板块。名称是 GBK。
typedef bool (__stdcall* TdxHq_GetBlockInfoFn)(int ConnectionID, const char* BlockFile, char* Result, char* ErrInfo);
// 下载报表文件到 SavePath。
typedef bool (__stdcall* TdxHq_GetReportFileFn)(int ConnectionID, const char* FileName, const char* SavePath, char* Result, char* ErrInfo);
// 板块列表。BoardType: concept / style / industry / index。
typedef bool (__stdcall* TdxHq_ListBoardsFn)(int ConnectionID, const char* BoardType, char* Result, char* ErrInfo);
// 板块成分。BoardName 为 GBK。
typedef bool (__stdcall* TdxHq_ListBoardMembersFn)(int ConnectionID, const char* BoardName, const char* BlockFile, char* Result, char* ErrInfo);
// 全市场涨跌统计。
typedef bool (__stdcall* TdxHq_GetMarketStatFn)(int ConnectionID, char* Result, char* ErrInfo);
// 当日资金流向。
typedef bool (__stdcall* TdxHq_GetFundFlowFn)(int ConnectionID, BYTE Market, const char* Zqdm, char* Result, char* ErrInfo);
// 历史资金流向。
typedef bool (__stdcall* TdxHq_GetHistoryFundFlowFn)(int ConnectionID, BYTE Market, const char* Zqdm, unsigned short Start, unsigned short Count, char* Result, char* ErrInfo);
// 从本机通达信目录读取站点。Channel：1 L1，2 L2，3 扩展。
typedef bool (__stdcall* TdxHq_LoadHostCfgFn)(int Channel, const char* TdxRoot, char* Result, char* ErrInfo);

// 连接扩展行情。与 A 股 ConnectionID 不要混用。
typedef int (__stdcall* TdxExHq_ConnectFn)(char* IP, int Port, char* Account, char* Password, char* Result, char* ErrInfo);
// 断开扩展行情。
typedef void (__stdcall* TdxExHq_DisconnectFn)(int ConnectionID);
// 扩展市场列表。
typedef bool (__stdcall* TdxExHq_GetMarketsFn)(int ConnectionID, char* Result, char* ErrInfo);
// 扩展品种数量。Result 是数量。
typedef bool (__stdcall* TdxExHq_GetInstrumentCountFn)(int ConnectionID, int* Result, char* ErrInfo);
// 扩展品种信息。
typedef bool (__stdcall* TdxExHq_GetInstrumentInfoFn)(int ConnectionID, int Start, short Count, char* Result, char* ErrInfo);
// 扩展 K 线。
typedef bool (__stdcall* TdxExHq_GetInstrumentBarsFn)(int ConnectionID, BYTE Category, BYTE Market, char* Zqdm, int Start, short* Count, char* Result, char* ErrInfo);
// 扩展当日分时。
typedef bool (__stdcall* TdxExHq_GetMinuteTimeDataFn)(int ConnectionID, BYTE Market, char* Zqdm, char* Result, char* ErrInfo);
// 扩展分笔。
typedef bool (__stdcall* TdxExHq_GetTransactionDataFn)(int ConnectionID, BYTE Market, char* Zqdm, int Start, short* Count, char* Result, char* ErrInfo);
// 扩展单品种行情。
typedef bool (__stdcall* TdxExHq_GetInstrumentQuoteFn)(int ConnectionID, BYTE Market, char* Zqdm, char* Result, char* ErrInfo);
// 扩展历史分笔。
typedef bool (__stdcall* TdxExHq_GetHistoryTransactionDataFn)(int ConnectionID, BYTE Market, char* Zqdm, int Date, int Start, short* Count, char* Result, char* ErrInfo);
// 扩展历史分时。
typedef bool (__stdcall* TdxExHq_GetHistoryMinuteTimeDataFn)(int ConnectionID, BYTE Market, char* Zqdm, int Date, char* Result, char* ErrInfo);

int main()
{
    HMODULE dll = LoadLibraryA("Api.dll");
    if (!dll)
    {
        std::cout << "LoadLibrary Api.dll failed" << std::endl;
        return 1;
    }

    TdxHq_ConnectFn TdxHq_Connect = (TdxHq_ConnectFn)GetProcAddress(dll, "TdxHq_Connect");
    TdxHq_DisconnectFn TdxHq_Disconnect = (TdxHq_DisconnectFn)GetProcAddress(dll, "TdxHq_Disconnect");
    TdxHq_GetSecurityQuotesFn TdxHq_GetSecurityQuotes = (TdxHq_GetSecurityQuotesFn)GetProcAddress(dll, "TdxHq_GetSecurityQuotes");
    if (!TdxHq_Connect || !TdxHq_Disconnect || !TdxHq_GetSecurityQuotes)
    {
        std::cout << "GetProcAddress failed" << std::endl;
        FreeLibrary(dll);
        return 1;
    }

    char* result = new char[1024 * 1024];
    char* errInfo = new char[256];
    result[0] = 0;
    errInfo[0] = 0;

    int connectionId = TdxHq_Connect("YOUR_HQ_HOST", 7709, "YOUR_ACCOUNT", "YOUR_PASSWORD", result, errInfo);
    std::cout << "TdxHq_Connect " << connectionId << " " << result << " " << errInfo << std::endl;
    if (connectionId >= 0)
    {
        BYTE market[1] = { 0 };
        const char* zqdm[1] = { "000001" };
        unsigned short count = 1;
        TdxHq_GetSecurityQuotes(connectionId, market, zqdm, &count, result, errInfo);
        std::cout << "TdxHq_GetSecurityQuotes" << std::endl << result << std::endl << errInfo << std::endl;

        // unsigned short n = 100;
        // TdxHq_GetSecurityBars(connectionId, 4, 0, "000001", 0, &n, result, errInfo);
        // TdxHq_GetMinuteTimeData(connectionId, 0, "000001", result, errInfo);

        TdxHq_Disconnect(connectionId);
    }

    delete[] result;
    delete[] errInfo;
    FreeLibrary(dll);
    return 0;
}
