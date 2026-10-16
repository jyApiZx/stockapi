// thsapi.dll 交易调用示例。进程位数须与 DLL 一致（x86 或 x64）。
// Result、ErrInfo 为 GBK。Result 是 TSV：行以 \n 分隔，列以 \t 分隔，首行表头。
// ErrInfo 建议 >= 256 字节，Result 建议 >= 1024*1024 字节。没出错时 ErrInfo 为空字符串。

#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

// 初始化交易运行时。进程内调用一次，须在 Logon 之前。
typedef void (__stdcall* InitFn)();

// 释放全部会话并关闭运行时。调用后不可再使用此前的 ClientID。
typedef void (__stdcall* UnInitFn)();

// 交易账户登录。成功返回 ClientID（>=0），失败返回 -1。
// IP：交易服务器 IP 或域名。Port：端口。
// Config 只含两个字段：
//   qs_id：券商 ID。模拟炒股为 356，须与 IP 是同一家。
//   account_type：0 资金帐户，1 深圳，2 上海，3 基金，4 深圳Ｂ股，5 上海Ｂ股，k 客户号。
//   示例: {"qs_id":356,"account_type":0}
// YybID：营业部代码。模拟炒股为 48。
// AccountNo：登录账号。模拟账户的登录号和资金账号都填模拟资金账号。
// TradeAccount：交易账号，一般与登录账号相同。空则按登录账号处理。
// JyPassword：交易密码。
// TxPassword：通讯密码。大部分券商传空字符串，个别券商需要填写。
// ErrInfo：出错说明，建议 256 字节。没出错时为空字符串。
typedef int (__stdcall* LogonFn)(const char* IP, short Port, const char* Config, short YybID,
    const char* AccountNo, const char* TradeAccount, const char* JyPassword, const char* TxPassword, char* ErrInfo);

// 注销指定 ClientID。
typedef void (__stdcall* LogoffFn)(int ClientID);

// 查询当日交易数据。成功返回 0，失败返回 -1。
// Category：
//   0 资金  1 股份  2 当日委托  3 当日成交  4 可撤单  5 股东代码
//   6 融资余额  7 融券余额  8 可融证券  9 实时合约流水
//   12 可申购新股  13 新股申购额度  14 配号  15 中签
// 普通账户上 6 以后的类别可能失败，甚至把连接踢掉。本示例只查 0–5。
// Result：表格。建议 1024*1024 字节。打开表头 ID 后，列名形如「合同编号_2135」。
// 常用：2102 证券代码，2103 证券名称，2106 股东代码，2108 交易市场，
// 2112 资金余额，2116 可用金额，2121 可用余额，2135 合同编号（撤单用的委托编号）。
typedef int (__stdcall* QueryDataFn)(int ClientID, int Category, char* Result, char* ErrInfo);

// 历史查询。Category：0 历史委托  1 历史成交  2 交割单  3 资金明细  4 对账单。
// StartDate / EndDate 为 yyyyMMdd。无整型返回值，成败看 ErrInfo。
typedef void (__stdcall* QueryHistoryDataFn)(int ClientID, int Category, char* StartDate, char* EndDate, char* Result, char* ErrInfo);

// 同一账户批量查询。Category、Result、ErrInfo 长度都是 Count，下标一一对应。
typedef void (__stdcall* QueryDatasFn)(int ClientID, int Category[], int Count, char* Result[], char* ErrInfo[]);

// 多账户批量查询。四个数组长度都是 Count。
typedef void (__stdcall* QueryMultiAccountsDatasFn)(int ClientID[], int Category[], int Count, char* Result[], char* ErrInfo[]);

// 下委托。
// Category：0 买入  1 卖出  2 融资买入  3 融券卖出  4 买券还券  5 卖券还款  6 现券还券  7 新股申购。
// PriceType：0 限价；1 对方最优  2 本方最优  3 即时成交剩余撤销
//   4 五档即成剩撤  5 全额成交或撤销（深圳）  6 五档即成转限价（上海）。
// Gddm：股东代码。上海股票填上海股东代码，深圳股票填深圳股东代码。
// Price：委托价格。市价时为保护限价。Result 中含委托编号（字段 2135）。
typedef void (__stdcall* SendOrderFn)(int ClientID, int Category, int PriceType, char* Gddm, char* Zqdm, float Price, int Quantity, char* Result, char* ErrInfo);

// 同一账户批量下单。各数组长度都是 Count，语义同 SendOrder。
typedef void (__stdcall* SendOrdersFn)(int ClientID, int Category[], int PriceType[], char* Gddm[], char* Zqdm[], float Price[], int Quantity[], int Count, char* Result[], char* ErrInfo[]);

// 多账户批量下单。ClientID 与各参数数组长度都是 Count。
typedef void (__stdcall* SendMultiAccountsOrdersFn)(int ClientID[], int Category[], int PriceType[], char* Gddm[], char* Zqdm[], float Price[], int Quantity[], int Count, char* Result[], char* ErrInfo[]);

// 撤委托。
// ExchangeID：上海 "1"，深圳 "0"（个别券商普通账户深圳是 "2"），北交所 "6"。
// hth：要撤的委托编号，对应查询结果里的合同编号（字段 2135）。
typedef void (__stdcall* CancelOrderFn)(int ClientID, char* ExchangeID, char* hth, char* Result, char* ErrInfo);

// 同一账户批量撤单。ExchangeID、hth、Result、ErrInfo 长度都是 Count。
typedef void (__stdcall* CancelOrdersFn)(int ClientID, char* ExchangeID[], char* hth[], int Count, char* Result[], char* ErrInfo[]);

// 多账户批量撤单。各数组长度都是 Count。
typedef void (__stdcall* CancelMultiAccountsOrdersFn)(int ClientID[], char* ExchangeID[], char* hth[], int Count, char* Result[], char* ErrInfo[]);

// 五档行情。Zqdm 可带 sz/sh/bj 前缀。与交易共用 ClientID。
typedef void (__stdcall* GetQuoteFn)(int ClientID, char* Zqdm, char* Result, char* ErrInfo);

// 同一账户批量五档。Zqdm、Result、ErrInfo 长度都是 Count。
typedef void (__stdcall* GetQuotesFn)(int ClientID, char* Zqdm[], int Count, char* Result[], char* ErrInfo[]);

// 多账户批量五档。各数组长度都是 Count。
typedef void (__stdcall* GetMultiAccountsQuotesFn)(int ClientID[], char* Zqdm[], int Count, char* Result[], char* ErrInfo[]);

// 融资融券直接还款。Amount 为金额字符串。
typedef void (__stdcall* RepayFn)(int ClientID, char* Amount, char* Result, char* ErrInfo);

// 查询可买/可卖数量。返回值 >=0 为数量，负数表示失败。Category、PriceType 同 SendOrder（不含新股申购）。
typedef int (__stdcall* GetCanBuySellFn)(int ClientID, int Category, int PriceType, const char* Gddm, const char* Zqdm, float Price, char* Result, char* ErrInfo);

// 查询可交易数量。Result 为明细 TSV，返回值优先取「可买数量/可卖数量」。
typedef int (__stdcall* GetTradableQuantityFn)(int ClientID, int Category, int PriceType, const char* Gddm, const char* Zqdm, float Price, char* Result, char* ErrInfo);

// 是否按列序配置重排 Result。enable 非 0 打开，默认关。与 EnableResultColumnIdSuffix 互不影响。
typedef void (__stdcall* EnableResultColumnOrderFn)(int enable);

// 打开/关闭结果表头字段 ID 后缀（全局生效，默认关）。enable 非 0 时表头为「资金余额_2112」。
typedef void (__stdcall* EnableResultColumnIdSuffixFn)(int enable);

// 股东代码列表。先走 QueryData(5)；没有行则回落登录时缓存的股东号。
typedef void (__stdcall* GetShareholderCodesFn)(int ClientID, char* Result, char* ErrInfo);

// 一键打新。DryRun 非 0 只出计划不下单。
// 返回值：试算为计划笔数，实盘为成功笔数，查询失败为 -1。
typedef int (__stdcall* OneClickIpoFn)(int ClientID, int DryRun, char* Result, char* ErrInfo);

// 场内基金 / ETF 申购赎回。
// Category：0 场内申购(金额)  1 场内赎回(份额)  2 ETF申购(份额)  3 ETF赎回(份额)。
// ExchangeID：0 深圳，1 上海，6 北交所。Amount 为金额或份额字符串。
typedef void (__stdcall* FundOrderFn)(int ClientID, int Category, char* Gddm, char* Zqdm, int ExchangeID, char* Amount, char* Result, char* ErrInfo);

struct Field
{
    int Id;
    const char* Title;
};

struct Table
{
    std::vector<std::string> Headers;
    std::vector<std::vector<std::string> > Rows;
};

static std::vector<std::string> Split(const std::string& text, char sep)
{
    std::vector<std::string> parts;
    std::string cur;
    for (size_t i = 0; i < text.size(); ++i)
    {
        if (text[i] == sep)
        {
            parts.push_back(cur);
            cur.clear();
        }
        else
        {
            cur.push_back(text[i]);
        }
    }
    parts.push_back(cur);
    return parts;
}

static Table Parse(const std::string& text)
{
    Table table;
    std::string normalized = text;
    std::string flat;
    for (size_t i = 0; i < normalized.size(); ++i)
    {
        if (normalized[i] == '\r')
            continue;
        flat.push_back(normalized[i]);
    }
    std::vector<std::string> lines = Split(flat, '\n');
    std::vector<std::string> kept;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        if (!lines[i].empty())
            kept.push_back(lines[i]);
    }
    if (kept.empty())
        return table;
    table.Headers = Split(kept[0], '\t');
    for (size_t i = 1; i < kept.size(); ++i)
        table.Rows.push_back(Split(kept[i], '\t'));
    return table;
}

// 按字段 ID 取单元格。表头须已打开 ID 后缀，形如「合同编号_2135」或纯「2135」。
static std::string Cell(const Table& table, size_t rowIndex, int fieldId)
{
    if (rowIndex >= table.Rows.size())
        return "";
    std::string id;
    if (fieldId == 0)
        id = "0";
    else
    {
        int n = fieldId;
        char buf[16];
        int p = 0;
        while (n > 0 && p < 15)
        {
            buf[p++] = char('0' + (n % 10));
            n /= 10;
        }
        id.assign(buf, buf + p);
        for (int a = 0, b = (int)id.size() - 1; a < b; ++a, --b)
        {
            char t = id[a];
            id[a] = id[b];
            id[b] = t;
        }
    }
    std::string suffix = "_" + id;
    const std::vector<std::string>& row = table.Rows[rowIndex];
    for (size_t i = 0; i < table.Headers.size(); ++i)
    {
        std::string h = table.Headers[i];
        if (h == id || (h.size() >= suffix.size() && h.compare(h.size() - suffix.size(), suffix.size(), suffix) == 0))
            return i < row.size() ? row[i] : "";
    }
    return "";
}

static void PrintJoined(const std::vector<std::string>& cols)
{
    for (size_t i = 0; i < cols.size(); ++i)
    {
        if (i)
            std::cout << " | ";
        std::cout << cols[i];
    }
    std::cout << std::endl;
}

// 按指定字段 ID 抽出常用列。没有该 ID 时单元格为空，列仍然保留。
static void PrintProjected(const Table& raw, const Field* fields, int count)
{
    if (count <= 0)
    {
        std::cout << "(空)" << std::endl;
        return;
    }
    std::vector<std::string> titles;
    for (int c = 0; c < count; ++c)
    {
        std::string title = fields[c].Title;
        title += "_";
        char num[16];
        wsprintfA(num, "%d", fields[c].Id);
        title += num;
        titles.push_back(title);
    }
    PrintJoined(titles);
    for (size_t r = 0; r < raw.Rows.size(); ++r)
    {
        std::vector<std::string> row;
        for (int c = 0; c < count; ++c)
            row.push_back(Cell(raw, r, fields[c].Id));
        PrintJoined(row);
    }
    std::cout << "行数=" << raw.Rows.size() << std::endl;
}

// 交易市场文字换成 CancelOrder 的交易所 ID。深圳默认 "0"。
// 查询结果是 GBK：沪 BB A6，上 C9 CF，北 B1 B1。
static bool HasGbk(const std::string& text, unsigned char a, unsigned char b)
{
    for (size_t i = 0; i + 1 < text.size(); ++i)
    {
        if ((unsigned char)text[i] == a && (unsigned char)text[i + 1] == b)
            return true;
    }
    return false;
}

static std::string ExchangeIdOf(const std::string& market)
{
    if (HasGbk(market, 0xBB, 0xA6) || HasGbk(market, 0xC9, 0xCF) || market == "1")
        return "1";
    if (HasGbk(market, 0xB1, 0xB1) || market == "6")
        return "6";
    if (market == "2")
        return "2";
    return "0";
}

int main()
{
    HMODULE dll = LoadLibraryA("thsapi.dll");
    if (!dll)
    {
        std::cout << "LoadLibrary thsapi.dll failed" << std::endl;
        return 1;
    }

    InitFn Init = (InitFn)GetProcAddress(dll, "Init");
    UnInitFn UnInit = (UnInitFn)GetProcAddress(dll, "UnInit");
    LogonFn Logon = (LogonFn)GetProcAddress(dll, "Logon");
    LogoffFn Logoff = (LogoffFn)GetProcAddress(dll, "Logoff");
    QueryDataFn QueryData = (QueryDataFn)GetProcAddress(dll, "QueryData");
    EnableResultColumnIdSuffixFn EnableResultColumnIdSuffix =
        (EnableResultColumnIdSuffixFn)GetProcAddress(dll, "EnableResultColumnIdSuffix");

    if (!Init || !UnInit || !Logon || !Logoff || !QueryData || !EnableResultColumnIdSuffix)
    {
        std::cout << "GetProcAddress failed" << std::endl;
        FreeLibrary(dll);
        return 1;
    }

    char* result = new char[1024 * 1024];
    char* errInfo = new char[256];
    result[0] = 0;
    errInfo[0] = 0;

    // qs_id 356、端口 8002、营业部 48 是同花顺模拟炒股站点。账号和密码换成模拟资金账号。
    const char* config = "{\"qs_id\":356,\"account_type\":0}";

    Init();
    EnableResultColumnIdSuffix(1);

    int clientId = Logon("trade.10jqka.com.cn", 8002, config, 48,
        "YOUR_ACCOUNT", "YOUR_ACCOUNT", "YOUR_PASSWORD", "", errInfo);
    if (clientId < 0)
    {
        std::cout << "Logon 失败: " << errInfo << std::endl;
        UnInit();
        delete[] result;
        delete[] errInfo;
        FreeLibrary(dll);
        return 1;
    }

    std::cout << "Logon 成功 clientId=" << clientId << std::endl << std::endl;

    // 本示例只查 0–5。两融和新股类别在普通账户上可能失败，甚至把连接踢掉。
    const int categories[] = { 0, 1, 2, 3, 4, 5 };
    const char* names[] = { "资金", "股份", "当日委托", "当日成交", "可撤单", "股东代码" };
    Field fields0[] = { {2172, "货币单位"}, {2112, "资金余额"}, {2116, "可用金额"}, {2114, "可取金额"}, {2113, "冻结金额"}, {2191, "总资产"}, {2973, "总市值"} };
    Field fields1[] = { {2102, "证券代码"}, {2103, "证券名称"}, {2108, "交易市场"}, {2106, "股东代码"}, {2117, "股票余额"}, {2121, "可用余额"}, {2122, "成本价"}, {2124, "市价"}, {2125, "市值"}, {2147, "盈亏"} };
    Field fields2[] = { {2139, "委托日期"}, {2140, "委托时间"}, {2102, "证券代码"}, {2103, "证券名称"}, {2109, "操作"}, {2127, "委托价格"}, {2126, "委托数量"}, {2128, "成交数量"}, {2135, "委托编号"}, {2108, "交易市场"}, {2106, "股东代码"}, {2105, "备注"} };
    Field fields3[] = { {2141, "成交日期"}, {2142, "成交时间"}, {2135, "委托编号"}, {2130, "成交编号"}, {2102, "证券代码"}, {2103, "证券名称"}, {2109, "操作"}, {2128, "成交数量"}, {2129, "成交均价"}, {2131, "成交金额"}, {2108, "交易市场"}, {2106, "股东代码"} };
    Field fields5[] = { {2106, "股东代码"}, {2107, "股东名称"}, {2171, "市场名称"}, {2167, "市场代码"}, {2908, "资金帐户"}, {2176, "是否主帐号"}, {2172, "货币单位"} };
    Field* groups[] = { fields0, fields1, fields2, fields3, fields2, fields5 };
    int counts[] = { 7, 10, 12, 12, 12, 7 };

    std::string shareholder;
    std::string exchangeId = "1";
    std::string entrustId;

    for (int i = 0; i < 6; ++i)
    {
        result[0] = 0;
        errInfo[0] = 0;
        int rc = QueryData(clientId, categories[i], result, errInfo);
        std::cout << "======== QueryData " << categories[i] << " " << names[i] << "  rc=" << rc << " ========" << std::endl;
        if (errInfo[0])
            std::cout << "err=" << errInfo << std::endl;
        Table table = Parse(result);
        PrintProjected(table, groups[i], counts[i]);
        if (categories[i] == 5 && !table.Rows.empty())
        {
            shareholder = Cell(table, 0, 2106);
            std::string market = Cell(table, 0, 2171);
            if (market.empty())
                market = Cell(table, 0, 2108);
            exchangeId = ExchangeIdOf(market);
            std::cout << "  股东代码 2106=" << shareholder << "  市场=" << market << "  exchangeId=" << exchangeId << std::endl;
        }
        if (categories[i] == 2 && !table.Rows.empty() && entrustId.empty())
        {
            entrustId = Cell(table, 0, 2135);
            std::cout << "  委托编号 2135=" << entrustId << std::endl;
        }
        std::cout << std::endl;
        if (std::string(errInfo).find("断开") != std::string::npos)
        {
            std::cout << "连接已断开，停止后续查询。" << std::endl;
            break;
        }
    }

    // 下单、撤单默认不执行。股东代码、交易所、委托编号来自上面的查询。
    // SendOrderFn SendOrder = (SendOrderFn)GetProcAddress(dll, "SendOrder");
    // CancelOrderFn CancelOrder = (CancelOrderFn)GetProcAddress(dll, "CancelOrder");
    // SendOrder(clientId, 0, 0, (char*)shareholder.c_str(), (char*)"600000", 10.00f, 100, result, errInfo);
    // CancelOrder(clientId, (char*)exchangeId.c_str(), (char*)entrustId.c_str(), result, errInfo);

    std::cout << "下单/撤单已注释。股东=" << shareholder << " exchangeId=" << exchangeId << " 委托编号=" << entrustId << std::endl;

    Logoff(clientId);
    UnInit();
    delete[] result;
    delete[] errInfo;
    FreeLibrary(dll);
    return 0;
}
