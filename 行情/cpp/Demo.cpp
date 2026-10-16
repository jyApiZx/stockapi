// HqApi.dll  L1 调用说明
// HqApi.dll 与 exe 放在同一目录。下面每个函数先写 /// 说明，再给出函数指针类型，最后是可编译的调用示例。
// Result、ErrInfo 里的中文是 GBK。表格行与行之间是 \n，列与列之间是 \t。

#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

typedef unsigned char byte;
#ifndef u_short
typedef unsigned short u_short;
#endif


/// <summary>
/// 连接通达信行情服务器。服务器地址可在券商软件登录界面的通讯设置里查到，下面示例已列入全部 L1 主站。
/// 必须先连接成功，才能调用后面的取数接口。可以建立多个连接，每个 ConnectionID 不要跨线程使用。
/// </summary>
/// <param name="IP">行情服务器 IP。</param>
/// <param name="Port">行情服务器端口。L1 主站是 7709。</param>
/// <param name="Account">账号。发布版不能用 guest，也不能留空。
/// L1 必须写成 L1*账号，例如 L1*your_account。
/// L2 账号不要加 L1*，直接写账号。本文件只演示 L1。</param>
/// <param name="Password">密码。发布版不能是空串，填该账号的密码。</param>
/// <param name="Result">成功时是表格。第一行表头，后面每行一台服务器的信息。
/// 列：服务器名称、最后交易日期。
/// 一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 ConnectionID（大于等于 0），失败返回 -1。</returns>
typedef int(__stdcall* TdxHq_ConnectDelegate)(const char* IP, u_short Port, const char* Account, const char* Password, char* Result, char* ErrInfo);


/// <summary>
/// 断开 TdxHq_Connect 建立的连接。断开后该 ConnectionID 不能再使用。
/// </summary>
/// <param name="ConnectionID">TdxHq_Connect 返回的连接号。</param>
typedef void(__stdcall* TdxHq_DisconnectDelegate)(int ConnectionID);


/// <summary>
/// 获取某个市场里证券的总数量。后面 TdxHq_GetSecurityList 的 Start 不能超过这个数量。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场。0=深圳，1=上海，2=北京。</param>
/// <param name="Result">成功时写入该市场的证券数量。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetSecurityCountDelegate)(int ConnectionID, byte Market, u_short& Result, char* ErrInfo);


/// <summary>
/// 获取市场内从 Start 开始的一批证券代码。一次最多返回约 1000 条，数量不够时加大 Start 再取。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场。0=深圳，1=上海，2=北京。</param>
/// <param name="Start">起始位置。第一只是 0，第二只是 1，依此类推。范围由 TdxHq_GetSecurityCount 决定。</param>
/// <param name="Count">调用前填希望返回的条数。调用后被改成实际返回的条数。</param>
/// <param name="Result">表格。列：代码、一手股数、名称、保留、价格小数位数、昨收、保留、保留。
/// 名称是 GBK。一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetSecurityListDelegate)(int ConnectionID, byte Market, u_short Start, u_short& Count, char* Result, char* ErrInfo);


/// <summary>
/// 获取证券的 K 线。股票、基金、北交所用这个接口。指数请用 TdxHq_GetIndexBars，两套列不一样，不要混用。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Category">K 线种类。0=5分钟，1=15分钟，2=30分钟，3=60分钟，4=日K，5=周K，6=月K，7=1分钟，8=1分钟，9=日K，10=季K，11=年K。</param>
/// <param name="Market">市场。0=深圳，1=上海，2=北京。北交所 92、43、83 开头的代码传 0 也可以。</param>
/// <param name="Zqdm">证券代码，6 位，例如 000001、600000、920000。</param>
/// <param name="Start">从最新一根往前数。最新一根是 0，前一根是 1。</param>
/// <param name="Count">调用前填希望返回的根数，最大 800。调用后被改成实际返回的根数。</param>
/// <param name="Result">表格。列：时间、开盘价、收盘价、最高价、最低价、成交量、成交额。
/// 日K 的时间是 YYYYMMDD，分钟K 带时分。价格单位是元。一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetSecurityBarsDelegate)(int ConnectionID, byte Category, byte Market, const char* Zqdm, u_short Start, u_short& Count, char* Result, char* ErrInfo);


/// <summary>
/// 获取指数的 K 线。例如上证指数 000001（市场 1）、深证成指 399001（市场 0）、沪深300 000300（市场 1）。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Category">K 线种类，与 TdxHq_GetSecurityBars 相同。0=5分钟，1=15分钟，2=30分钟，3=60分钟，4=日K，5=周K，6=月K，7=1分钟，8=1分钟，9=日K，10=季K，11=年K。</param>
/// <param name="Market">市场。0=深圳，1=上海。沪市指数 000300、000905、000852、000016 用 1；深市指数 399300、399905 用 0。</param>
/// <param name="Zqdm">指数代码。</param>
/// <param name="Start">从最新一根往前数。最新一根是 0。</param>
/// <param name="Count">调用前填希望返回的根数，最大 800。调用后被改成实际返回的根数。</param>
/// <param name="Result">表格。列：时间、开盘价、收盘价、最高价、最低价、成交量、成交额、涨家数、跌家数。
/// 比个股 K 线多最后两列。一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetIndexBarsDelegate)(int ConnectionID, byte Category, byte Market, const char* Zqdm, u_short Start, u_short& Count, char* Result, char* ErrInfo);


/// <summary>
/// 获取当日分时数据。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场。0=深圳，1=上海，2=北京。</param>
/// <param name="Zqdm">证券代码。</param>
/// <param name="Result">表格。列：现价、成交量、均价。现价和均价单位是元。
/// 一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetMinuteTimeDataDelegate)(int ConnectionID, byte Market, const char* Zqdm, char* Result, char* ErrInfo);


/// <summary>
/// 获取历史某一天的分时数据。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场。0=深圳，1=上海，2=北京。</param>
/// <param name="Zqdm">证券代码。</param>
/// <param name="Date">日期，整数 YYYYMMDD。例如 2014 年 1 月 1 日写 20140101。</param>
/// <param name="Result">表格。列与当日分时相同：现价、成交量、均价。
/// 一般分配 1024*1024 字节。当天没有交易时只有表头。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetHistoryMinuteTimeDataDelegate)(int ConnectionID, byte Market, const char* Zqdm, int Date, char* Result, char* ErrInfo);


/// <summary>
/// 获取当日分笔成交。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场。0=深圳，1=上海，2=北京。</param>
/// <param name="Zqdm">证券代码。</param>
/// <param name="Start">从最新一笔往前数。最新一笔是 0。</param>
/// <param name="Count">调用前填希望返回的笔数。调用后被改成实际返回的笔数。</param>
/// <param name="Result">表格，当日和历史都是这 6 列：时间、价格、现量、笔数、买卖、保留。
/// 价格单位是元。买卖是数字，不是字母：0=买，1=卖，4 或 8 等=其它（方向不明）。
/// 一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetTransactionDataDelegate)(int ConnectionID, byte Market, const char* Zqdm, u_short Start, u_short& Count, char* Result, char* ErrInfo);


/// <summary>
/// 获取历史某一天的分笔成交。列与当日分笔相同。服务器没有笔数时，笔数列为 0。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场。0=深圳，1=上海，2=北京。</param>
/// <param name="Zqdm">证券代码。</param>
/// <param name="Start">从该日最新一笔往前数。最新一笔是 0。</param>
/// <param name="Count">调用前填希望返回的笔数。调用后被改成实际返回的笔数。</param>
/// <param name="Date">日期，整数 YYYYMMDD。例如 2014 年 1 月 1 日写 20140101。</param>
/// <param name="Result">表格。列：时间、价格、现量、笔数、买卖、保留。
/// 买卖：0=买，1=卖，4 或 8 等=其它。历史数据的笔数经常是 0。
/// 一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetHistoryTransactionDataDelegate)(int ConnectionID, byte Market, const char* Zqdm, u_short Start, u_short& Count, int Date, char* Result, char* ErrInfo);


/// <summary>
/// 批量获取多只证券的五档报价。Market 和 Zqdm 一一对应，第 i 个市场配第 i 个代码。
/// 一次可以超过 80 只，DLL 会按 80 只一包自动拆开再合并。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场数组。0=深圳，1=上海，2=北京。北交所 92、43、83 开头传 0 也可以，返回行里的市场会是 2。</param>
/// <param name="Zqdm">证券代码数组，和 Market 等长。</param>
/// <param name="Count">调用前填请求的证券个数。调用后被改成实际返回的行数。</param>
/// <param name="Result">表格。列包括：市场、代码、活跃度、现价、昨收、开盘、最高、最低、时间、总量、现量、总金额、内盘、外盘、买一价到买五价、卖一价到卖五价及对应的量、涨速。
/// 价格单位是元，量的单位是股。一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetSecurityQuotesDelegate)(int ConnectionID, byte Market[], const char* Zqdm[], u_short& Count, char* Result, char* ErrInfo);


/// <summary>
/// 获取 F10 公司资料的分类目录。正文要用返回的文件名、起始位置和长度再调 TdxHq_GetCompanyInfoContent。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场。0=深圳，1=上海，2=北京。</param>
/// <param name="Zqdm">证券代码。</param>
/// <param name="Result">表格。列：类别、文件、开始、长度。
/// 类别是 GBK 名称，例如最新提示、公司概况。文件、开始、长度原样传给 TdxHq_GetCompanyInfoContent。
/// 一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetCompanyInfoCategoryDelegate)(int ConnectionID, byte Market, const char* Zqdm, char* Result, char* ErrInfo);


/// <summary>
/// 获取 F10 某一分类的正文。FileName、Start、Length 必须来自 TdxHq_GetCompanyInfoCategory 的同一行，不要自己猜偏移。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场。0=深圳，1=上海，2=北京。</param>
/// <param name="Zqdm">证券代码。</param>
/// <param name="FileName">分类所在文件名，例如 000001.txt。</param>
/// <param name="Start">该分类在文件中的起始字节，来自目录的「开始」列。</param>
/// <param name="Length">该分类的字节长度，来自目录的「长度」列。Result 缓冲必须大于这个长度。</param>
/// <param name="Result">该分类的正文，GBK 文本，不是表格。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetCompanyInfoContentDelegate)(int ConnectionID, byte Market, const char* Zqdm, const char* FileName, int Start, int Length, char* Result, char* ErrInfo);


/// <summary>
/// 获取除权除息信息。可以一次查多只，Market 和 Zqdm 一一对应。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场数组。0=深圳，1=上海，2=北京。</param>
/// <param name="Zqdm">证券代码数组。</param>
/// <param name="Count">证券个数。这个参数调用后不会改成返回行数，一只股票通常有多行权息。</param>
/// <param name="Result">表格。列：市场、证券代码、日期、权息数据类别、前流通盘或派息金额、前总股本或配股价、后流通盘或送股数、后总股本或配售数。
/// 日期是 YYYYMMDD。一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetXDXRInfoDelegate)(int ConnectionID, byte Market[], const char* Zqdm[], int Count, char* Result, char* ErrInfo);


/// <summary>
/// 获取财务信息。可以一次查多只，Market 和 Zqdm 一一对应。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场数组。0=深圳，1=上海，2=北京。</param>
/// <param name="Zqdm">证券代码数组。</param>
/// <param name="Count">证券个数。</param>
/// <param name="Result">表格。列：市场、证券代码、流通股本、所属省份、所属行业、财务更新日期、上市日期、总股本、国家股、发起人法人股、法人股、B股、H股、职工股、总资产、流动资产、固定资产、无形资产、股东人数、流动负债、长期负债、资本公积金、净资产、主营收入、主营利润、应收帐款、营业利润、投资收益、经营现金流、总现金流、存货、利润总额、税后利润、净利润、未分配利润、每股净资产，以及若干保留列。
/// 股本类单位与行情主机一致，更新日期、上市日期是 YYYYMMDD。一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetFinanceInfoDelegate)(int ConnectionID, byte Market[], const char* Zqdm[], int Count, char* Result, char* ErrInfo);


/// <summary>
/// 获取集合竞价数据。一次返回里同时有早盘和尾盘：早盘大约 09:15 到 09:24，尾盘大约 14:57 到 14:59。
/// Count 太小会只拿到最前面的几笔，看起来像 09:20 就断了。要整段时把 Count 设到 2000。
/// </summary>
/// <param name="ConnectionID">连接号。</param>
/// <param name="Market">市场。0=深圳，1=上海，2=北京。</param>
/// <param name="Zqdm">证券代码。</param>
/// <param name="Start">起始位置。从最早一笔算起，第一笔是 0。</param>
/// <param name="Count">调用前填希望返回的条数。调用后被改成实际返回的条数。整段集合竞价建议填 2000。</param>
/// <param name="Result">表格。列：时间、匹配价、竞价量、未匹配量、保留。
/// 时间是 HH:MM:SS。匹配价单位是元。未匹配量为负表示卖方剩余。一般分配 1024*1024 字节。失败时为空字符串。</param>
/// <param name="ErrInfo">失败时的错误说明。一般分配 256 字节。成功时为空字符串。</param>
/// <returns>成功返回 true，失败返回 false。</returns>
typedef bool(__stdcall* TdxHq_GetCallAuctionDataDelegate)(int ConnectionID, byte Market, const char* Zqdm, int Start, int& Count, char* Result, char* ErrInfo);


void L1Hq()
{
    // HqApi.dll 复制到 exe 同目录
    HMODULE TdxApiHMODULE = LoadLibraryA("HqApi.dll");
    if (!TdxApiHMODULE)
    {
        cout << "LoadLibrary HqApi.dll 失败 " << GetLastError() << endl;
        return;
    }

    TdxHq_ConnectDelegate TdxHq_Connect = (TdxHq_ConnectDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_Connect");
    TdxHq_DisconnectDelegate TdxHq_Disconnect = (TdxHq_DisconnectDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_Disconnect");
    TdxHq_GetSecurityCountDelegate TdxHq_GetSecurityCount = (TdxHq_GetSecurityCountDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityCount");
    TdxHq_GetSecurityListDelegate TdxHq_GetSecurityList = (TdxHq_GetSecurityListDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityList");
    TdxHq_GetSecurityBarsDelegate TdxHq_GetSecurityBars = (TdxHq_GetSecurityBarsDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityBars");
    TdxHq_GetIndexBarsDelegate TdxHq_GetIndexBars = (TdxHq_GetIndexBarsDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetIndexBars");
    TdxHq_GetMinuteTimeDataDelegate TdxHq_GetMinuteTimeData = (TdxHq_GetMinuteTimeDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetMinuteTimeData");
    TdxHq_GetHistoryMinuteTimeDataDelegate TdxHq_GetHistoryMinuteTimeData = (TdxHq_GetHistoryMinuteTimeDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetHistoryMinuteTimeData");
    TdxHq_GetTransactionDataDelegate TdxHq_GetTransactionData = (TdxHq_GetTransactionDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetTransactionData");
    TdxHq_GetHistoryTransactionDataDelegate TdxHq_GetHistoryTransactionData = (TdxHq_GetHistoryTransactionDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetHistoryTransactionData");
    TdxHq_GetSecurityQuotesDelegate TdxHq_GetSecurityQuotes = (TdxHq_GetSecurityQuotesDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetSecurityQuotes");
    TdxHq_GetCompanyInfoCategoryDelegate TdxHq_GetCompanyInfoCategory = (TdxHq_GetCompanyInfoCategoryDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetCompanyInfoCategory");
    TdxHq_GetCompanyInfoContentDelegate TdxHq_GetCompanyInfoContent = (TdxHq_GetCompanyInfoContentDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetCompanyInfoContent");
    TdxHq_GetXDXRInfoDelegate TdxHq_GetXDXRInfo = (TdxHq_GetXDXRInfoDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetXDXRInfo");
    TdxHq_GetFinanceInfoDelegate TdxHq_GetFinanceInfo = (TdxHq_GetFinanceInfoDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetFinanceInfo");
    TdxHq_GetCallAuctionDataDelegate TdxHq_GetCallAuctionData = (TdxHq_GetCallAuctionDataDelegate)GetProcAddress(TdxApiHMODULE, "TdxHq_GetCallAuctionData");

    char* Result = new char[1024 * 1024];
    char* ErrInfo = new char[256];

    // L1 行情主站，端口都是 7709。名称|IP|端口
    string ServerAddress[] = {
"通达信深圳双线主站1|110.41.147.114|7709",
"通达信深圳双线主站2|110.41.2.72|7709",
"通达信深圳双线主站3|110.41.4.4|7709",
"通达信深圳双线主站4|175.178.112.197|7709",
"通达信深圳双线主站5|175.178.128.227|7709",
"通达信深圳双线主站6|110.41.154.219|7709",
"通达信上海双线主站1|124.70.176.52|7709",
"通达信上海双线主站2|122.51.120.217|7709",
"通达信上海双线主站3|123.60.186.45|7709",
"通达信上海双线主站4|123.60.164.122|7709",
"通达信上海双线主站5|111.229.247.189|7709",
"通达信上海双线主站6|124.70.199.56|7709",
"通达信北京双线主站1|121.36.54.217|7709",
"通达信北京双线主站2|121.36.81.195|7709",
"通达信北京双线主站3|123.249.15.60|7709",
"通达信广州双线主站1|124.71.85.110|7709",
"通达信广州双线主站2|139.9.51.18|7709",
"通达信广州双线主站3|139.159.239.163|7709",
"通达信上海双线主站7|122.51.232.182|7709",
"通达信上海双线主站8|118.25.98.114|7709",
"通达信上海双线主站9|121.36.225.169|7709",
"通达信上海双线主站10|123.60.70.228|7709",
"通达信上海双线主站11|123.60.73.44|7709",
"通达信上海双线主站12|124.70.133.119|7709",
"通达信上海双线主站13|124.71.187.72|7709",
"通达信上海双线主站14|124.71.187.122|7709",
"通达信武汉电信主站1|119.97.185.59|7709",
"通达信深圳双线主站7|129.204.230.128|7709",
"通达信北京双线主站4|124.70.75.113|7709",
"通达信广州双线主站4|124.71.9.153|7709",
"通达信上海双线主站15|123.60.84.66|7709",
"通达信深圳双线主站8|111.230.186.52|7709",
"通达信北京双线主站5|120.46.186.223|7709",
"通达信北京双线主站6|124.70.22.210|7709",
"通达信北京双线主站7|139.9.133.247|7709",
"通达信广州双线主站5|116.205.163.254|7709",
"通达信广州双线主站6|116.205.171.132|7709",
"通达信广州双线主站7|116.205.183.150|7709",
"上海双线主站16|121.37.183.82|7709",
"深圳双线主站9|110.41.174.169|7709"
    };

    const char* Account = "L1*YOUR_ACCOUNT";
    const char* Password = "YOUR_PASSWORD";

    int ConnectionID = -1;
    for (auto& it : ServerAddress)
    {
        int k1 = (int)it.find("|", 0);
        int k2 = (int)it.find("|", k1 + 1);
        string Name = it.substr(0, k1);
        string IP = it.substr(k1 + 1, k2 - 1 - k1);
        u_short Port = (u_short)atoi(it.substr(k2 + 1).c_str());

        cout << "连接" << Name << "[" << IP << ":" << Port << "]" << endl;
        ConnectionID = TdxHq_Connect(IP.c_str(), Port, Account, Password, Result, ErrInfo);
        cout << "TdxHq_Connect:" << Result << ErrInfo << endl;
        if (ConnectionID != -1)
            break;
    }
    if (ConnectionID == -1)
    {
        delete[] Result;
        delete[] ErrInfo;
        FreeLibrary(TdxApiHMODULE);
        return;
    }

    byte Market = 0;
    u_short Count = 0;
    bool bool1 = TdxHq_GetSecurityCount(ConnectionID, Market, Count, ErrInfo);
    cout << "TdxHq_GetSecurityCount:" << Count << ErrInfo << endl;

    u_short Start = 0;
    Count = 10;
    bool1 = TdxHq_GetSecurityList(ConnectionID, Market, Start, Count, Result, ErrInfo);
    cout << "TdxHq_GetSecurityList:" << Result << ErrInfo << endl;

    byte Category = 4;
    const char* Zqdm = "000001";
    Start = 0;
    Count = 10;
    bool1 = TdxHq_GetSecurityBars(ConnectionID, Category, Market, Zqdm, Start, Count, Result, ErrInfo);
    cout << "TdxHq_GetSecurityBars:" << Result << ErrInfo << endl;

    Start = 0;
    Count = 10;
    bool1 = TdxHq_GetIndexBars(ConnectionID, Category, Market, "399001", Start, Count, Result, ErrInfo);
    cout << "TdxHq_GetIndexBars:" << Result << ErrInfo << endl;

    bool1 = TdxHq_GetMinuteTimeData(ConnectionID, Market, Zqdm, Result, ErrInfo);
    cout << "TdxHq_GetMinuteTimeData:" << Result << ErrInfo << endl;

    bool1 = TdxHq_GetHistoryMinuteTimeData(ConnectionID, Market, Zqdm, 20261016, Result, ErrInfo);
    cout << "TdxHq_GetHistoryMinuteTimeData:" << Result << ErrInfo << endl;

    Start = 0;
    Count = 10;
    bool1 = TdxHq_GetTransactionData(ConnectionID, Market, Zqdm, Start, Count, Result, ErrInfo);
    cout << "TdxHq_GetTransactionData:" << Result << ErrInfo << endl;

    Start = 0;
    Count = 10;
    bool1 = TdxHq_GetHistoryTransactionData(ConnectionID, Market, Zqdm, Start, Count, 20261016, Result, ErrInfo);
    cout << "TdxHq_GetHistoryTransactionData:" << Result << ErrInfo << endl;

    u_short ZqdmCount = 2;
    byte Markets[] = { 0, 1 };
    const char* Zqdms[] = { "000001", "600000" };
    bool1 = TdxHq_GetSecurityQuotes(ConnectionID, Markets, Zqdms, ZqdmCount, Result, ErrInfo);
    cout << "TdxHq_GetSecurityQuotes:" << Result << ErrInfo << endl;

    bool1 = TdxHq_GetCompanyInfoCategory(ConnectionID, Market, Zqdm, Result, ErrInfo);
    cout << "TdxHq_GetCompanyInfoCategory:" << Result << ErrInfo << endl;

    bool1 = TdxHq_GetCompanyInfoContent(ConnectionID, Market, Zqdm, "000001.txt", 0, 4096, Result, ErrInfo);
    cout << "TdxHq_GetCompanyInfoContent:" << Result << ErrInfo << endl;

    bool1 = TdxHq_GetXDXRInfo(ConnectionID, Markets, Zqdms, 2, Result, ErrInfo);
    cout << "TdxHq_GetXDXRInfo:" << Result << ErrInfo << endl;

    bool1 = TdxHq_GetFinanceInfo(ConnectionID, Markets, Zqdms, 2, Result, ErrInfo);
    cout << "TdxHq_GetFinanceInfo:" << Result << ErrInfo << endl;

    int nCount = 2000;
    bool1 = TdxHq_GetCallAuctionData(ConnectionID, Market, Zqdm, 0, nCount, Result, ErrInfo);
    cout << "TdxHq_GetCallAuctionData:" << Result << ErrInfo << endl;

    TdxHq_Disconnect(ConnectionID);
    delete[] Result;
    delete[] ErrInfo;
    FreeLibrary(TdxApiHMODULE);
}

int main()
{
    cout << "股票行情演示 L1:" << endl;
    L1Hq();
    system("pause");
    return 0;
}
