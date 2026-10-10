// Api.dll 行情调用示例。进程位数须与 DLL 一致（x86 或 x64）。调用约定是 stdcall。
// Result、ErrInfo 为 GBK。Result 多为 TSV：行以 \n 分隔，列以 \t 分隔。
// ErrInfo 建议 >= 256，Result 建议 >= 1MB。
// 连接返回 >=0 为 ConnectionID，失败返回 -1。其余接口 true 成功。
// Market：0 深圳  1 上海  2 北交所。扩展行情的市场号以 TdxExHq_GetMarkets 为准。
// K 线 Category：0~3 为 5/15/30/60 分钟，4 日，5 周，6 月，7 起为 1 分钟及扩展周期。
// A 股常见端口 7709，L2 常见 7719。扩展行情常见 7727 / 7721。

using System;
using System.Runtime.InteropServices;
using System.Text;

internal static class Api
{
    // 连接 A 股行情。账号由授权方提供。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    public static extern int TdxHq_Connect(string ip, ushort port, string account, string password, StringBuilder result, StringBuilder errInfo);

    // 断开 A 股连接。
    [DllImport("Api.dll", CallingConvention = CallingConvention.StdCall)]
    public static extern void TdxHq_Disconnect(int connectionId);

    // 市场证券数量。result 是数量，不是字符串。
    [DllImport("Api.dll", CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetSecurityCount(int connectionId, byte market, ref ushort result, StringBuilder errInfo);

    // 证券列表。count 入为请求条数，出为实际条数。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetSecurityList(int connectionId, byte market, ushort start, ref ushort count, StringBuilder result, StringBuilder errInfo);

    // 个股 K 线。指数请用 TdxHq_GetIndexBars。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetSecurityBars(int connectionId, byte category, byte market, string zqdm, ushort start, ref ushort count, StringBuilder result, StringBuilder errInfo);

    // 指数 K 线，含涨跌家数。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetIndexBars(int connectionId, byte category, byte market, string zqdm, ushort start, ref ushort count, StringBuilder result, StringBuilder errInfo);

    // 当日分时。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetMinuteTimeData(int connectionId, byte market, string zqdm, StringBuilder result, StringBuilder errInfo);

    // 历史分时。date 为 yyyyMMdd。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetHistoryMinuteTimeData(int connectionId, byte market, string zqdm, int date, StringBuilder result, StringBuilder errInfo);

    // 分笔成交。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetTransactionData(int connectionId, byte market, string zqdm, ushort start, ref ushort count, StringBuilder result, StringBuilder errInfo);

    // 历史分笔。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetHistoryTransactionData(int connectionId, byte market, string zqdm, ushort start, ref ushort count, int date, StringBuilder result, StringBuilder errInfo);

    // 五档行情。market 与 zqdm 等长，count 入出参。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetSecurityQuotes(int connectionId, byte[] market, string[] zqdm, ref ushort count, StringBuilder result, StringBuilder errInfo);

    // 公司信息目录。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetCompanyInfoCategory(int connectionId, byte market, string zqdm, StringBuilder result, StringBuilder errInfo);

    // 公司信息正文。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetCompanyInfoContent(int connectionId, byte market, string zqdm, string fileName, int start, int length, StringBuilder result, StringBuilder errInfo);

    // 除权除息。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetXDXRInfo(int connectionId, byte[] market, string[] zqdm, int count, StringBuilder result, StringBuilder errInfo);

    // 财务简表。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetFinanceInfo(int connectionId, byte[] market, string[] zqdm, int count, StringBuilder result, StringBuilder errInfo);

    // 集合竞价。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetCallAuctionData(int connectionId, byte market, string zqdm, int start, ref int count, StringBuilder result, StringBuilder errInfo);

    // 十档行情。需要 L2。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetSecurityQuotes10(int connectionId, byte[] market, string[] zqdm, ref ushort count, StringBuilder result, StringBuilder errInfo);

    // 买卖队列。需要 L2。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetBuySellQueue(int connectionId, byte[] market, string[] zqdm, int count, StringBuilder result, StringBuilder errInfo);

    // 逐笔成交。需要 L2。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetDetailTransactionData(int connectionId, byte market, string zqdm, int start, ref ushort count, StringBuilder result, StringBuilder errInfo);

    // 逐笔成交原始数据。需要 L2。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetDetailOriginalTransactionData(int connectionId, byte market, string zqdm, int start, ref ushort count, StringBuilder result, StringBuilder errInfo);

    // 逐笔委托。需要 L2。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetDetailOrderData(int connectionId, byte market, string zqdm, int start, ref ushort count, StringBuilder result, StringBuilder errInfo);

    // 板块文件。blockFile 空则默认概念板块。名称是 GBK。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetBlockInfo(int connectionId, string blockFile, StringBuilder result, StringBuilder errInfo);

    // 下载报表文件到 savePath。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetReportFile(int connectionId, string fileName, string savePath, StringBuilder result, StringBuilder errInfo);

    // 板块列表。boardType: concept / style / industry / index。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_ListBoards(int connectionId, string boardType, StringBuilder result, StringBuilder errInfo);

    // 板块成分。boardName 为 GBK。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_ListBoardMembers(int connectionId, string boardName, string blockFile, StringBuilder result, StringBuilder errInfo);

    // 全市场涨跌统计。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetMarketStat(int connectionId, StringBuilder result, StringBuilder errInfo);

    // 当日资金流向。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetFundFlow(int connectionId, byte market, string zqdm, StringBuilder result, StringBuilder errInfo);

    // 历史资金流向。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_GetHistoryFundFlow(int connectionId, byte market, string zqdm, ushort start, ushort count, StringBuilder result, StringBuilder errInfo);

    // 从本机通达信目录读取站点。channel：1 L1，2 L2，3 扩展。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxHq_LoadHostCfg(int channel, string tdxRoot, StringBuilder result, StringBuilder errInfo);

    // 连接扩展行情。与 A 股 ConnectionID 不要混用。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    public static extern int TdxExHq_Connect(string ip, int port, string account, string password, StringBuilder result, StringBuilder errInfo);

    // 断开扩展行情。
    [DllImport("Api.dll", CallingConvention = CallingConvention.StdCall)]
    public static extern void TdxExHq_Disconnect(int connectionId);

    // 扩展市场列表。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxExHq_GetMarkets(int connectionId, StringBuilder result, StringBuilder errInfo);

    // 扩展品种数量。result 是数量。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxExHq_GetInstrumentCount(int connectionId, ref int result, StringBuilder errInfo);

    // 扩展品种信息。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxExHq_GetInstrumentInfo(int connectionId, int start, short count, StringBuilder result, StringBuilder errInfo);

    // 扩展 K 线。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxExHq_GetInstrumentBars(int connectionId, byte category, byte market, string zqdm, int start, ref short count, StringBuilder result, StringBuilder errInfo);

    // 扩展当日分时。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxExHq_GetMinuteTimeData(int connectionId, byte market, string zqdm, StringBuilder result, StringBuilder errInfo);

    // 扩展分笔。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxExHq_GetTransactionData(int connectionId, byte market, string zqdm, int start, ref short count, StringBuilder result, StringBuilder errInfo);

    // 扩展单品种行情。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxExHq_GetInstrumentQuote(int connectionId, byte market, string zqdm, StringBuilder result, StringBuilder errInfo);

    // 扩展历史分笔。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxExHq_GetHistoryTransactionData(int connectionId, byte market, string zqdm, int date, int start, ref short count, StringBuilder result, StringBuilder errInfo);

    // 扩展历史分时。
    [DllImport("Api.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.StdCall)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool TdxExHq_GetHistoryMinuteTimeData(int connectionId, byte market, string zqdm, int date, StringBuilder result, StringBuilder errInfo);
}

internal static class Program
{
    private static void Main()
    {
        var result = new StringBuilder(1024 * 1024);
        var errInfo = new StringBuilder(256);

        int connectionId = Api.TdxHq_Connect("YOUR_HQ_HOST", 7709, "YOUR_ACCOUNT", "YOUR_PASSWORD", result, errInfo);
        Console.WriteLine("TdxHq_Connect " + connectionId + " " + result + " " + errInfo);
        if (connectionId >= 0)
        {
            ushort count = 1;
            Api.TdxHq_GetSecurityQuotes(connectionId, new byte[] { 0 }, new[] { "000001" }, ref count, result, errInfo);
            Console.WriteLine("TdxHq_GetSecurityQuotes");
            Console.WriteLine(result);
            Console.WriteLine(errInfo);

            // ushort n = 100;
            // Api.TdxHq_GetSecurityBars(connectionId, 4, 0, "000001", 0, ref n, result, errInfo);
            // Api.TdxHq_GetMinuteTimeData(connectionId, 0, "000001", result, errInfo);

            Api.TdxHq_Disconnect(connectionId);
        }
    }
}
