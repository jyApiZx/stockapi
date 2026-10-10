// Api.dll 交易调用示例。进程位数须与 DLL 一致（x86 或 x64）。
// Result、ErrInfo 为 GBK。Result 是 TSV：行以 \n 分隔，列以 \t 分隔，首行表头。
// ErrInfo 建议 >= 256 字节，Result 建议 >= 1MB。

#include <iostream>
#include <Windows.h>

// 初始化交易运行时。进程内调用一次，须在 Logon 之前。
typedef void (__stdcall* InitFn)();

// 释放全部会话并关闭运行时。调用后不可再使用此前的 ClientID。
typedef void (__stdcall* UnInitFn)();

// 登录。成功返回 ClientID（>=0），失败返回 -1。
// Config 只填账户类型：
// 0 资金帐户  1 深圳帐户  2 上海帐户  3 基金帐户  4 深圳Ｂ股  5 上海Ｂ股  k 客户号
// 示例: {"account_type":0}  或 {"account_type":"k"}
typedef int (__stdcall* LogonFn)(const char* IP, short Port, const char* Config, short YybID,
    const char* AccountNo, const char* TradeAccount, const char* JyPassword, const char* TxPassword, char* ErrInfo);

// 注销指定 ClientID。
typedef void (__stdcall* LogoffFn)(int ClientID);

// 查询当日数据。成功返回 0，失败返回 -1。
// Category: 0资金 1股份 2当日委托 3当日成交 4可撤单 5股东代码
//           6融资负债 7融券负债 8可融证券 9实时合约流水
//           12可申购新股 13新股申购额度 14配号 15中签
typedef int (__stdcall* QueryDataFn)(int ClientID, int Category, char* Result, char* ErrInfo);

// 查询历史数据。Category: 0历史委托 1历史成交 2交割单 3资金明细 4对账单。日期 yyyyMMdd。
typedef void (__stdcall* QueryHistoryDataFn)(int ClientID, int Category, char* StartDate, char* EndDate, char* Result, char* ErrInfo);

// 同一账户批量查询。各数组长度均为 Count。
typedef void (__stdcall* QueryDatasFn)(int ClientID, int Category[], int Count, char* Result[], char* ErrInfo[]);

// 多账户批量查询。各数组长度均为 Count。
typedef void (__stdcall* QueryMultiAccountsDatasFn)(int ClientID[], int Category[], int Count, char* Result[], char* ErrInfo[]);

// 下单。
// Category: 0买入 1卖出 2融资买入 3融券卖出 4买券还券 5卖券还款 6现券还券 7新股申购
// PriceType: 0限价；深市市价 1/2/3/4/5，沪市市价 1/2/4/6。Price 为限价或保护限价。
typedef void (__stdcall* SendOrderFn)(int ClientID, int Category, int PriceType, char* Gddm, char* Zqdm, float Price, int Quantity, char* Result, char* ErrInfo);

// 同一账户批量下单。各数组长度均为 Count。
typedef void (__stdcall* SendOrdersFn)(int ClientID, int Category[], int PriceType[], char* Gddm[], char* Zqdm[], float Price[], int Quantity[], int Count, char* Result[], char* ErrInfo[]);

// 多账户批量下单。各数组长度均为 Count。
typedef void (__stdcall* SendMultiAccountsOrdersFn)(int ClientID[], int Category[], int PriceType[], char* Gddm[], char* Zqdm[], float Price[], int Quantity[], int Count, char* Result[], char* ErrInfo[]);

// 撤单。ExchangeID：上海 "1"，深圳 "0"。hth 为委托编号。
typedef void (__stdcall* CancelOrderFn)(int ClientID, char* ExchangeID, char* hth, char* Result, char* ErrInfo);

// 同一账户批量撤单。各数组长度均为 Count。
typedef void (__stdcall* CancelOrdersFn)(int ClientID, char* ExchangeID[], char* hth[], int Count, char* Result[], char* ErrInfo[]);

// 多账户批量撤单。各数组长度均为 Count。
typedef void (__stdcall* CancelMultiAccountsOrdersFn)(int ClientID[], char* ExchangeID[], char* hth[], int Count, char* Result[], char* ErrInfo[]);

// 五档行情。Zqdm 可带 sz/sh/bj 前缀。
typedef void (__stdcall* GetQuoteFn)(int ClientID, char* Zqdm, char* Result, char* ErrInfo);

// 同一账户批量五档。各数组长度均为 Count。
typedef void (__stdcall* GetQuotesFn)(int ClientID, char* Zqdm[], int Count, char* Result[], char* ErrInfo[]);

// 多账户批量五档。各数组长度均为 Count。
typedef void (__stdcall* GetMultiAccountsQuotesFn)(int ClientID[], char* Zqdm[], int Count, char* Result[], char* ErrInfo[]);

// 融资融券直接还款。Amount 为金额字符串。
typedef void (__stdcall* RepayFn)(int ClientID, char* Amount, char* Result, char* ErrInfo);

// 查询可买/可卖数量。成功 >=0，失败为负数。Category、PriceType 同下单。
typedef int (__stdcall* GetCanBuySellFn)(int ClientID, int Category, int PriceType, const char* Gddm, const char* Zqdm, float Price, char* Result, char* ErrInfo);

// 查询可交易数量。返回值优先取可买/可卖数量列。
typedef int (__stdcall* GetTradableQuantityFn)(int ClientID, int Category, int PriceType, const char* Gddm, const char* Zqdm, float Price, char* Result, char* ErrInfo);

// 非 0 时按列序重排 Result，默认关闭。
typedef void (__stdcall* EnableResultColumnOrderFn)(int enable);

// 非 0 时表头附带字段 ID 后缀，默认关闭。
typedef void (__stdcall* EnableResultColumnIdSuffixFn)(int enable);

// 查询股东代码。先查 Category 5，无数据时回落登录缓存。
typedef void (__stdcall* GetShareholderCodesFn)(int ClientID, char* Result, char* ErrInfo);

// 一键打新。DryRun 非 0 只出计划不下单。失败返回 -1。
typedef int (__stdcall* OneClickIpoFn)(int ClientID, int DryRun, char* Result, char* ErrInfo);

// 场内基金 / ETF。Category: 0场内申购(金额) 1场内赎回(份额) 2ETF申购(份额) 3ETF赎回(份额)。
// ExchangeID: 0深圳 1上海 6北交所。Amount 为金额或份额字符串。
typedef void (__stdcall* FundOrderFn)(int ClientID, int Category, char* Gddm, char* Zqdm, int ExchangeID, char* Amount, char* Result, char* ErrInfo);

int main()
{
    HMODULE dll = LoadLibraryA("Api.dll");
    if (!dll)
    {
        std::cout << "LoadLibrary Api.dll failed" << std::endl;
        return 1;
    }

    InitFn Init = (InitFn)GetProcAddress(dll, "Init");
    UnInitFn UnInit = (UnInitFn)GetProcAddress(dll, "UnInit");
    LogonFn Logon = (LogonFn)GetProcAddress(dll, "Logon");
    LogoffFn Logoff = (LogoffFn)GetProcAddress(dll, "Logoff");
    QueryDataFn QueryData = (QueryDataFn)GetProcAddress(dll, "QueryData");

    if (!Init || !UnInit || !Logon || !Logoff || !QueryData)
    {
        std::cout << "GetProcAddress failed" << std::endl;
        FreeLibrary(dll);
        return 1;
    }

    char* result = new char[1024 * 1024];
    char* errInfo = new char[256];
    result[0] = 0;
    errInfo[0] = 0;

    Init();

    // Config 只写账户类型。
    const char* config = "{\"account_type\":0}";
    int clientId = Logon("YOUR_BROKER_IP", 0, config, 0,
        "YOUR_ACCOUNT", "YOUR_ACCOUNT", "YOUR_PASSWORD", "", errInfo);
    std::cout << "Logon ClientID=" << clientId << " " << errInfo << std::endl;

    if (clientId >= 0)
    {
        QueryData(clientId, 0, result, errInfo);
        std::cout << "QueryData funds:" << std::endl << result << std::endl << errInfo << std::endl;

        // SendOrder(clientId, 0, 0, "股东代码", "证券代码", 0.0f, 100, result, errInfo);
        // CancelOrder(clientId, "1", "委托编号", result, errInfo);
        // Repay(clientId, "0", result, errInfo);

        Logoff(clientId);
    }

    UnInit();

    delete[] result;
    delete[] errInfo;
    FreeLibrary(dll);
    return 0;
}
