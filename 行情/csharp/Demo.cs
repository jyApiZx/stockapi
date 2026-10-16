// HqApi.dll L1 调用说明。DLL 与 exe 放在同一目录。进程位数须一致（x86 或 x64）。
// Result、ErrInfo 里的中文是 GBK。表格行与行之间是 \n，列与列之间是 \t。
// ErrInfo 一般分配 256 字节，Result 一般分配 1024*1024 字节。
// L1 账号必须写成 L1*账号，不能用 guest，也不能留空。密码不能是空串。

using System;
using System.Runtime.InteropServices;
using System.Text;

internal static class HqApi
{
    public const string DllName = "HqApi.dll";

    /// <summary>
    /// 连接通达信行情服务器。成功返回 ConnectionID（大于等于 0），失败返回 -1。
    /// L1 主站端口是 7709。Account 必须写成 L1*账号。Password 填该账号的密码。
    /// Result 成功时是表格，列：服务器名称、最后交易日期。
    /// </summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    public static extern int TdxHq_Connect(string ip, ushort port, string account, string password, StringBuilder result, StringBuilder errInfo);

    /// <summary>断开连接。断开后该 ConnectionID 不能再使用。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    public static extern void TdxHq_Disconnect(int connectionId);

    /// <summary>获取某个市场里证券的总数量。Market：0 深圳，1 上海，2 北京。</summary>
    [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetSecurityCount(int connectionId, byte market, ref ushort result, StringBuilder errInfo);

    /// <summary>
    /// 获取市场内从 start 开始的一批证券代码。一次最多约 1000 条。
    /// count 调用前是希望返回的条数，调用后是实际条数。
    /// Result 列：代码、一手股数、名称、保留、价格小数位数、昨收、保留、保留。
    /// </summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetSecurityList(int connectionId, byte market, ushort start, ref ushort count, StringBuilder result, StringBuilder errInfo);

    /// <summary>
    /// 获取证券 K 线。指数请用 TdxHq_GetIndexBars。
    /// Category：0=5分钟，1=15分钟，2=30分钟，3=60分钟，4=日K，5=周K，6=月K，7=1分钟，8=1分钟，9=日K，10=季K，11=年K。
    /// start 从最新一根往前数。count 最大 800。
    /// Result 列：时间、开盘价、收盘价、最高价、最低价、成交量、成交额。
    /// </summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetSecurityBars(int connectionId, byte category, byte market, string zqdm, ushort start, ref ushort count, StringBuilder result, StringBuilder errInfo);

    /// <summary>获取指数 K 线。比个股多涨家数、跌家数两列。</summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetIndexBars(int connectionId, byte category, byte market, string zqdm, ushort start, ref ushort count, StringBuilder result, StringBuilder errInfo);

    /// <summary>获取当日分时。列：现价、成交量、均价。</summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetMinuteTimeData(int connectionId, byte market, string zqdm, StringBuilder result, StringBuilder errInfo);

    /// <summary>获取历史某一天的分时。date 为 YYYYMMDD。</summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetHistoryMinuteTimeData(int connectionId, byte market, string zqdm, int date, StringBuilder result, StringBuilder errInfo);

    /// <summary>获取当日分笔。列：时间、价格、现量、笔数、买卖、保留。买卖：0=买，1=卖。</summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetTransactionData(int connectionId, byte market, string zqdm, ushort start, ref ushort count, StringBuilder result, StringBuilder errInfo);

    /// <summary>获取历史某一天的分笔。列与当日相同。date 为 YYYYMMDD。</summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetHistoryTransactionData(int connectionId, byte market, string zqdm, ushort start, ref ushort count, int date, StringBuilder result, StringBuilder errInfo);

    /// <summary>批量五档报价。market 和 zqdm 一一对应。DLL 按 80 只一包拆开再合并。</summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetSecurityQuotes(int connectionId, byte[] market, string[] zqdm, ref ushort count, StringBuilder result, StringBuilder errInfo);

    /// <summary>获取 F10 分类目录。列：类别、文件、开始、长度。</summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetCompanyInfoCategory(int connectionId, byte market, string zqdm, StringBuilder result, StringBuilder errInfo);

    /// <summary>获取 F10 某一分类的正文。fileName、start、length 必须来自目录的同一行。</summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetCompanyInfoContent(int connectionId, byte market, string zqdm, string fileName, int start, int length, StringBuilder result, StringBuilder errInfo);

    /// <summary>获取除权除息。可以一次查多只。count 调用后不会改成返回行数。</summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetXDXRInfo(int connectionId, byte[] market, string[] zqdm, int count, StringBuilder result, StringBuilder errInfo);

    /// <summary>获取财务信息。可以一次查多只。</summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetFinanceInfo(int connectionId, byte[] market, string[] zqdm, int count, StringBuilder result, StringBuilder errInfo);

    /// <summary>获取集合竞价。整段早盘和尾盘建议 count 设到 2000。</summary>
    [DllImport(DllName, CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetCallAuctionData(int connectionId, byte market, string zqdm, int start, ref int count, StringBuilder result, StringBuilder errInfo);
}

internal static class Program
{
    // L1 行情主站，端口都是 7709。名称|IP|端口
    private static readonly string[] Servers =
    {
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
        "深圳双线主站9|110.41.174.169|7709",
    };

    private static void Main()
    {
        Console.OutputEncoding = Encoding.UTF8;
        Console.WriteLine("股票行情演示 L1:");

        var result = new StringBuilder(1024 * 1024);
        var err = new StringBuilder(256);
        int connectionId = -1;
        foreach (string item in Servers)
        {
            string[] parts = item.Split('|');
            Console.WriteLine("连接" + parts[0] + "[" + parts[1] + ":" + parts[2] + "]");
            connectionId = HqApi.TdxHq_Connect(parts[1], ushort.Parse(parts[2]), "L1*YOUR_ACCOUNT", "YOUR_PASSWORD", result, err);
            Console.WriteLine("TdxHq_Connect:" + result + err);
            if (connectionId != -1)
                break;
        }
        if (connectionId == -1)
            return;

        ushort count = 0;
        HqApi.TdxHq_GetSecurityCount(connectionId, 0, ref count, err);
        Console.WriteLine("TdxHq_GetSecurityCount:" + count + err);

        count = 10;
        HqApi.TdxHq_GetSecurityList(connectionId, 0, 0, ref count, result, err);
        Console.WriteLine("TdxHq_GetSecurityList:" + result + err);

        count = 10;
        HqApi.TdxHq_GetSecurityBars(connectionId, 4, 0, "000001", 0, ref count, result, err);
        Console.WriteLine("TdxHq_GetSecurityBars:" + result + err);

        count = 10;
        HqApi.TdxHq_GetIndexBars(connectionId, 4, 0, "399001", 0, ref count, result, err);
        Console.WriteLine("TdxHq_GetIndexBars:" + result + err);

        HqApi.TdxHq_GetMinuteTimeData(connectionId, 0, "000001", result, err);
        Console.WriteLine("TdxHq_GetMinuteTimeData:" + result + err);

        HqApi.TdxHq_GetHistoryMinuteTimeData(connectionId, 0, "000001", 20261016, result, err);
        Console.WriteLine("TdxHq_GetHistoryMinuteTimeData:" + result + err);

        count = 10;
        HqApi.TdxHq_GetTransactionData(connectionId, 0, "000001", 0, ref count, result, err);
        Console.WriteLine("TdxHq_GetTransactionData:" + result + err);

        count = 10;
        HqApi.TdxHq_GetHistoryTransactionData(connectionId, 0, "000001", 0, ref count, 20261016, result, err);
        Console.WriteLine("TdxHq_GetHistoryTransactionData:" + result + err);

        ushort quoteCount = 2;
        HqApi.TdxHq_GetSecurityQuotes(connectionId, new byte[] { 0, 1 }, new[] { "000001", "600000" }, ref quoteCount, result, err);
        Console.WriteLine("TdxHq_GetSecurityQuotes:" + result + err);

        HqApi.TdxHq_GetCompanyInfoCategory(connectionId, 0, "000001", result, err);
        Console.WriteLine("TdxHq_GetCompanyInfoCategory:" + result + err);

        HqApi.TdxHq_GetCompanyInfoContent(connectionId, 0, "000001", "000001.txt", 0, 4096, result, err);
        Console.WriteLine("TdxHq_GetCompanyInfoContent:" + result + err);

        HqApi.TdxHq_GetXDXRInfo(connectionId, new byte[] { 0, 1 }, new[] { "000001", "600000" }, 2, result, err);
        Console.WriteLine("TdxHq_GetXDXRInfo:" + result + err);

        HqApi.TdxHq_GetFinanceInfo(connectionId, new byte[] { 0, 1 }, new[] { "000001", "600000" }, 2, result, err);
        Console.WriteLine("TdxHq_GetFinanceInfo:" + result + err);

        int auctionCount = 2000;
        HqApi.TdxHq_GetCallAuctionData(connectionId, 0, "000001", 0, ref auctionCount, result, err);
        Console.WriteLine("TdxHq_GetCallAuctionData:" + result + err);

        HqApi.TdxHq_Disconnect(connectionId);
    }
}
