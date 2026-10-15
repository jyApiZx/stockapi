// thsapi.dll 交易调用示例。进程位数须与 DLL 一致（x86 或 x64）。
// 中文 Windows 下 CharSet.Ansi 即 GBK。Result 是 TSV：行以 \n 分隔，列以 \t 分隔。
// ErrInfo 建议分配 256 字节，Result 建议分配 1024*1024 字节。没出错时 ErrInfo 为空字符串。

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

/// <summary>
/// thsapi.dll 导出。调用约定 stdcall。
/// </summary>
internal static class ThsApi
{
    public const string DllName = "thsapi.dll";

    /// <summary>初始化交易运行时。进程内调用一次，须在 Logon 之前。同 OpenTdx。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void Init();

    /// <summary>释放全部会话并关闭运行时。调用后不可再使用此前的 ClientID。同 CloseTdx。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void UnInit();

    /// <summary>
    /// 交易账户登录。成功返回 ClientID（&gt;=0），失败返回 -1。
    /// </summary>
    /// <param name="ip">交易服务器 IP 或域名。</param>
    /// <param name="port">交易服务器端口。</param>
    /// <param name="config">
    /// 登录配置 JSON，只含两个字段。
    /// qs_id：券商 ID。模拟炒股为 356，须与 ip 是同一家。
    /// account_type：0 资金帐户，1 深圳，2 上海，3 基金，4 深圳Ｂ股，5 上海Ｂ股，k 客户号。
    /// 示例: {"qs_id":356,"account_type":0}
    /// </param>
    /// <param name="yybId">营业部代码。模拟炒股为 48。</param>
    /// <param name="accountNo">登录账号。模拟账户的登录号和资金账号都填模拟资金账号。</param>
    /// <param name="tradeAccount">交易账号，一般与登录账号相同。空则按登录账号处理。</param>
    /// <param name="jyPassword">交易密码。</param>
    /// <param name="txPassword">通讯密码。大部分券商传空字符串，个别券商需要填写。</param>
    /// <param name="errInfo">出错说明。建议分配 256 字节。没出错时为空字符串。</param>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int Logon(
        string ip, short port, string config, short yybId,
        string accountNo, string tradeAccount,
        string jyPassword, string txPassword, StringBuilder errInfo);

    /// <summary>注销指定 ClientID。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void Logoff(int clientId);

    /// <summary>
    /// 查询当日交易数据。成功返回 0，失败返回 -1。
    /// </summary>
    /// <param name="clientId">ClientID。</param>
    /// <param name="category">
    /// 0 资金  1 股份  2 当日委托  3 当日成交  4 可撤单  5 股东代码
    /// 6 融资余额  7 融券余额  8 可融证券  9 实时合约流水
    /// 12 可申购新股  13 新股申购额度  14 配号  15 中签。
    /// 普通账户上 6 以后的类别可能失败，甚至把连接踢掉。本示例只查 0–5。
    /// </param>
    /// <param name="result">
    /// 表格。行以 \n 分隔，列以 \t 分隔，首行是表头。建议分配 1024*1024 字节。出错时为空字符串。
    /// 打开表头 ID 后，列名形如「合同编号_2135」。
    /// 常用：2102 证券代码，2103 证券名称，2106 股东代码，2108 交易市场，
    /// 2112 资金余额，2116 可用金额，2121 可用余额，2135 合同编号（撤单用的委托编号）。
    /// </param>
    /// <param name="errInfo">同 Logon 的 ErrInfo。</param>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int QueryData(int clientId, int category, StringBuilder result, StringBuilder errInfo);

    /// <summary>
    /// 历史查询。category：0 历史委托  1 历史成交  2 交割单  3 资金明细  4 对账单。
    /// startDate / endDate 为 yyyyMMdd。无整型返回值，成败看 errInfo。
    /// </summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void QueryHistoryData(
        int clientId, int category, string startDate, string endDate,
        StringBuilder result, StringBuilder errInfo);

    /// <summary>
    /// 同一账户批量查询。category、result、errInfo 长度都是 count，下标一一对应。
    /// result / errInfo 的每个元素须是调用方事先分配好的 ANSI 缓冲指针。
    /// </summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void QueryDatas(int clientId, int[] category, int count, IntPtr[] result, IntPtr[] errInfo);

    /// <summary>多账户批量查询。四个数组长度都是 count。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void QueryMultiAccountsDatas(int[] clientId, int[] category, int count, IntPtr[] result, IntPtr[] errInfo);

    /// <summary>
    /// 下委托。
    /// </summary>
    /// <param name="clientId">ClientID。</param>
    /// <param name="category">0 买入  1 卖出  2 融资买入  3 融券卖出  4 买券还券  5 卖券还款  6 现券还券  7 新股申购。</param>
    /// <param name="priceType">
    /// 0 限价。
    /// 1 对方最优  2 本方最优  3 即时成交剩余撤销
    /// 4 五档即成剩撤  5 全额成交或撤销（深圳）  6 五档即成转限价（上海）。
    /// </param>
    /// <param name="gddm">股东代码。上海股票填上海股东代码，深圳股票填深圳股东代码。</param>
    /// <param name="zqdm">证券代码。</param>
    /// <param name="price">委托价格。市价时为保护限价。</param>
    /// <param name="quantity">委托数量。</param>
    /// <param name="result">回报表，其中含委托编号（字段 2135 合同编号）。</param>
    /// <param name="errInfo">同 Logon 的 ErrInfo。</param>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void SendOrder(
        int clientId, int category, int priceType,
        string gddm, string zqdm,
        float price, int quantity, StringBuilder result, StringBuilder errInfo);

    /// <summary>同一账户批量下单。各数组长度都是 count，语义同 SendOrder。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void SendOrders(
        int clientId, int[] category, int[] priceType, IntPtr[] gddm, IntPtr[] zqdm,
        float[] price, int[] quantity, int count, IntPtr[] result, IntPtr[] errInfo);

    /// <summary>多账户批量下单。clientId 与各参数数组长度都是 count。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void SendMultiAccountsOrders(
        int[] clientId, int[] category, int[] priceType, IntPtr[] gddm, IntPtr[] zqdm,
        float[] price, int[] quantity, int count, IntPtr[] result, IntPtr[] errInfo);

    /// <summary>
    /// 撤委托。
    /// </summary>
    /// <param name="clientId">ClientID。</param>
    /// <param name="exchangeId">交易所。上海 "1"，深圳 "0"（个别券商普通账户深圳是 "2"），北交所 "6"。</param>
    /// <param name="hth">要撤的委托编号，对应查询结果里的合同编号（字段 2135）。</param>
    /// <param name="result">回报表。</param>
    /// <param name="errInfo">同 Logon 的 ErrInfo。</param>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void CancelOrder(int clientId, string exchangeId, string hth, StringBuilder result, StringBuilder errInfo);

    /// <summary>同一账户批量撤单。exchangeId、hth、result、errInfo 长度都是 count。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void CancelOrders(
        int clientId, IntPtr[] exchangeId, IntPtr[] hth, int count, IntPtr[] result, IntPtr[] errInfo);

    /// <summary>多账户批量撤单。各数组长度都是 count。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void CancelMultiAccountsOrders(
        int[] clientId, IntPtr[] exchangeId, IntPtr[] hth, int count, IntPtr[] result, IntPtr[] errInfo);

    /// <summary>五档行情。zqdm 可带 sz/sh/bj 前缀。与交易共用 ClientID。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void GetQuote(int clientId, string zqdm, StringBuilder result, StringBuilder errInfo);

    /// <summary>同一账户批量五档。zqdm、result、errInfo 长度都是 count。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void GetQuotes(int clientId, IntPtr[] zqdm, int count, IntPtr[] result, IntPtr[] errInfo);

    /// <summary>多账户批量五档。各数组长度都是 count。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void GetMultiAccountsQuotes(int[] clientId, IntPtr[] zqdm, int count, IntPtr[] result, IntPtr[] errInfo);

    /// <summary>融资融券直接还款。amount 为金额字符串。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void Repay(int clientId, string amount, StringBuilder result, StringBuilder errInfo);

    /// <summary>
    /// 查询可买/可卖数量。返回值 &gt;=0 为数量，负数表示失败。
    /// category、priceType 同 SendOrder（不含新股申购）。
    /// </summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int GetCanBuySell(
        int clientId, int category, int priceType, string gddm, string zqdm, float price,
        StringBuilder result, StringBuilder errInfo);

    /// <summary>查询可交易数量。Result 为明细 TSV，返回值优先取「可买数量/可卖数量」。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int GetTradableQuantity(
        int clientId, int category, int priceType, string gddm, string zqdm, float price,
        StringBuilder result, StringBuilder errInfo);

    /// <summary>是否按列序配置重排 Result。enable 非 0 打开，默认关。与 EnableResultColumnIdSuffix 互不影响。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void EnableResultColumnOrder(int enable);

    /// <summary>打开/关闭结果表头字段 ID 后缀（全局生效，默认关）。enable 非 0 时表头为「资金余额_2112」。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void EnableResultColumnIdSuffix(int enable);

    /// <summary>股东代码列表。先走 QueryData(5)；没有行则回落登录时缓存的股东号。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void GetShareholderCodes(int clientId, StringBuilder result, StringBuilder errInfo);

    /// <summary>
    /// 一键打新。dryRun 非 0 只出计划不下单。
    /// 返回值：试算为计划笔数，实盘为成功笔数，查询失败为 -1。
    /// </summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern int OneClickIpo(int clientId, int dryRun, StringBuilder result, StringBuilder errInfo);

    /// <summary>
    /// 场内基金 / ETF 申购赎回。
    /// category：0 场内申购(金额)  1 场内赎回(份额)  2 ETF申购(份额)  3 ETF赎回(份额)。
    /// exchangeId：0 深圳，1 上海，6 北交所。amount 为金额或份额字符串。
    /// </summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
    public static extern void FundOrder(
        int clientId, int category, string gddm, string zqdm, int exchangeId, string amount,
        StringBuilder result, StringBuilder errInfo);
}

internal static class Program
{
    /// <summary>本示例只查 0–5。两融和新股类别在普通账户上可能失败，甚至把连接踢掉。</summary>
    private static readonly (int Category, string Name)[] QueryOrder =
    {
        (0, "资金"),
        (1, "股份"),
        (2, "当日委托"),
        (3, "当日成交"),
        (4, "可撤单"),
        (5, "股东代码"),
    };

    /// <summary>各类查询要留下的常用字段。顺序就是打印的列序。</summary>
    private static readonly Dictionary<int, (int Id, string Title)[]> CommonFields =
        new Dictionary<int, (int Id, string Title)[]>
        {
            { 0, new[] { (2172, "货币单位"), (2112, "资金余额"), (2116, "可用金额"), (2114, "可取金额"), (2113, "冻结金额"), (2191, "总资产"), (2973, "总市值") } },
            { 1, new[] { (2102, "证券代码"), (2103, "证券名称"), (2108, "交易市场"), (2106, "股东代码"), (2117, "股票余额"), (2121, "可用余额"), (2122, "成本价"), (2124, "市价"), (2125, "市值"), (2147, "盈亏") } },
            { 2, new[] { (2139, "委托日期"), (2140, "委托时间"), (2102, "证券代码"), (2103, "证券名称"), (2109, "操作"), (2127, "委托价格"), (2126, "委托数量"), (2128, "成交数量"), (2135, "委托编号"), (2108, "交易市场"), (2106, "股东代码"), (2105, "备注") } },
            { 3, new[] { (2141, "成交日期"), (2142, "成交时间"), (2135, "委托编号"), (2130, "成交编号"), (2102, "证券代码"), (2103, "证券名称"), (2109, "操作"), (2128, "成交数量"), (2129, "成交均价"), (2131, "成交金额"), (2108, "交易市场"), (2106, "股东代码") } },
            { 4, new[] { (2139, "委托日期"), (2140, "委托时间"), (2102, "证券代码"), (2103, "证券名称"), (2109, "操作"), (2127, "委托价格"), (2126, "委托数量"), (2128, "成交数量"), (2135, "委托编号"), (2108, "交易市场"), (2106, "股东代码"), (2105, "备注") } },
            { 5, new[] { (2106, "股东代码"), (2107, "股东名称"), (2171, "市场名称"), (2167, "市场代码"), (2908, "资金帐户"), (2176, "是否主帐号"), (2172, "货币单位") } },
        };

    private static void Main()
    {
        Console.OutputEncoding = Encoding.UTF8;
        var err = NewErr();

        // qs_id 356、端口 8002、营业部 48 是同花顺模拟炒股站点。账号和密码换成模拟资金账号。
        string config = "{\"qs_id\":356,\"account_type\":0}";

        ThsApi.Init();
        ThsApi.EnableResultColumnIdSuffix(1);

        int clientId = ThsApi.Logon(
            "trade.10jqka.com.cn",
            8002,
            config,
            48,
            "YOUR_ACCOUNT",
            "YOUR_ACCOUNT",
            "YOUR_PASSWORD",
            "",
            err);

        if (clientId < 0)
        {
            Console.WriteLine("Logon 失败: " + err);
            ThsApi.UnInit();
            return;
        }

        Console.WriteLine("Logon 成功 clientId=" + clientId);
        Console.WriteLine();

        string shareholder = "";
        string exchangeId = "1";
        string entrustId = "";

        foreach (var item in QueryOrder)
        {
            bool disconnected;
            Table table = Query(clientId, item.Category, item.Name, out disconnected);
            if (item.Category == 5 && table.Rows.Count > 0)
            {
                shareholder = Cell(table, 0, 2106);
                string market = Cell(table, 0, 2171);
                if (market.Length == 0)
                    market = Cell(table, 0, 2108);
                exchangeId = ExchangeIdOf(market);
                Console.WriteLine("  股东代码 2106=" + shareholder + "  市场=" + market + "  exchangeId=" + exchangeId);
            }
            if (item.Category == 2 && table.Rows.Count > 0 && entrustId.Length == 0)
            {
                entrustId = Cell(table, 0, 2135);
                Console.WriteLine("  委托编号 2135=" + entrustId);
            }
            Console.WriteLine();
            if (disconnected)
                break;
        }

        // 下单、撤单默认不执行。股东代码、交易所、委托编号来自上面的查询。
        /*
        var orderResult = NewResult();
        var orderErr = NewErr();
        ThsApi.SendOrder(clientId, 0, 0, shareholder, "600000", 10.00f, 100, orderResult, orderErr);
        Console.WriteLine("SendOrder err=" + orderErr);
        PrintTable(Project(Parse(orderResult.ToString()), CommonFields[2]));
        entrustId = Cell(Parse(orderResult.ToString()), 0, 2135);

        var cancelResult = NewResult();
        var cancelErr = NewErr();
        ThsApi.CancelOrder(clientId, exchangeId, entrustId, cancelResult, cancelErr);
        Console.WriteLine("CancelOrder err=" + cancelErr);
        PrintTable(Parse(cancelResult.ToString()));
        */

        Console.WriteLine("下单/撤单已注释。股东=" + shareholder + " exchangeId=" + exchangeId + " 委托编号=" + entrustId);

        ThsApi.Logoff(clientId);
        ThsApi.UnInit();
    }

    private static Table Query(int clientId, int category, string name, out bool disconnected)
    {
        var result = NewResult();
        var err = NewErr();
        int rc = ThsApi.QueryData(clientId, category, result, err);
        Console.WriteLine("======== QueryData " + category + " " + name + "  rc=" + rc + " ========");
        if (err.Length > 0)
            Console.WriteLine("err=" + err);
        Table raw = Parse(result.ToString());
        PrintTable(Project(raw, CommonFields[category]));
        disconnected = err.ToString().Contains("断开");
        if (disconnected)
            Console.WriteLine("连接已断开，停止后续查询。");
        return raw;
    }

    /// <summary>按指定字段 ID 抽出常用列。没有该 ID 时单元格为空，列仍然保留。</summary>
    private static Table Project(Table raw, (int Id, string Title)[] fields)
    {
        var view = new Table();
        view.Headers = new string[fields.Length];
        for (int c = 0; c < fields.Length; c++)
            view.Headers[c] = fields[c].Title + "_" + fields[c].Id;
        for (int r = 0; r < raw.Rows.Count; r++)
        {
            var row = new string[fields.Length];
            for (int c = 0; c < fields.Length; c++)
                row[c] = Cell(raw, r, fields[c].Id);
            view.Rows.Add(row);
        }
        return view;
    }

    /// <summary>按字段 ID 取单元格。表头须已打开 ID 后缀，形如「合同编号_2135」或纯「2135」。</summary>
    private static string Cell(Table table, int rowIndex, int fieldId)
    {
        if (rowIndex < 0 || rowIndex >= table.Rows.Count)
            return "";
        string id = fieldId.ToString();
        string suffix = "_" + id;
        string[] headers = table.Headers;
        string[] row = table.Rows[rowIndex];
        for (int i = 0; i < headers.Length; i++)
        {
            string h = headers[i].Trim();
            if (h.EndsWith(suffix, StringComparison.Ordinal) || h == id)
                return i < row.Length ? row[i].Trim() : "";
        }
        return "";
    }

    /// <summary>交易市场文字换成 CancelOrder 的交易所 ID。深圳默认 "0"。</summary>
    private static string ExchangeIdOf(string market)
    {
        if (market.Contains("沪") || market.Contains("上") || market == "1")
            return "1";
        if (market.Contains("北") || market == "6")
            return "6";
        if (market == "2")
            return "2";
        return "0";
    }

    private static Table Parse(string text)
    {
        var table = new Table();
        if (string.IsNullOrWhiteSpace(text))
            return table;
        string[] lines = text.Replace("\r\n", "\n").Replace('\r', '\n')
            .Split(new[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
        if (lines.Length == 0)
            return table;
        table.Headers = lines[0].Split('\t');
        for (int i = 1; i < lines.Length; i++)
            table.Rows.Add(lines[i].Split('\t'));
        return table;
    }

    private static void PrintTable(Table table)
    {
        if (table.Headers.Length == 0)
        {
            Console.WriteLine("(空)");
            return;
        }
        Console.WriteLine(string.Join(" | ", table.Headers));
        foreach (string[] row in table.Rows)
            Console.WriteLine(string.Join(" | ", row));
        Console.WriteLine("行数=" + table.Rows.Count);
    }

    private static StringBuilder NewResult() => new StringBuilder(1024 * 1024);
    private static StringBuilder NewErr() => new StringBuilder(256);

    private sealed class Table
    {
        public string[] Headers = new string[0];
        public List<string[]> Rows = new List<string[]>();
    }
}
