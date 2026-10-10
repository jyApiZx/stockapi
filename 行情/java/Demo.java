// Api.dll 行情调用示例。进程位数须与 DLL 一致（x86 或 x64）。调用约定是 stdcall。
// 依赖 JNA。Result、ErrInfo 为 GBK。Result 多为 TSV：行以 \n 分隔，列以 \t 分隔。
// ErrInfo 建议 >= 256 字节，Result 建议 >= 1MB。
// 连接返回 >=0 为 ConnectionID，失败返回 -1。其余接口 true 成功。
// Market：0 深圳  1 上海  2 北交所。扩展行情的市场号以 TdxExHq_GetMarkets 为准。
// K 线 Category：0~3 为 5/15/30/60 分钟，4 日，5 周，6 月，7 起为 1 分钟及扩展周期。
// A 股常见端口 7709，L2 常见 7719。扩展行情常见 7727 / 7721。

import com.sun.jna.Native;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.ShortByReference;
import com.sun.jna.win32.StdCallLibrary;

public class Demo {
    public interface Api extends StdCallLibrary {
        // 连接 A 股行情。账号由授权方提供。
        int TdxHq_Connect(String ip, short port, String account, String password, byte[] result, byte[] errInfo);

        // 断开 A 股连接。
        void TdxHq_Disconnect(int connectionId);

        // 市场证券数量。result 是数量，不是字符串。
        byte TdxHq_GetSecurityCount(int connectionId, byte market, ShortByReference result, byte[] errInfo);

        // 证券列表。count 入为请求条数，出为实际条数。
        byte TdxHq_GetSecurityList(int connectionId, byte market, short start, ShortByReference count, byte[] result, byte[] errInfo);

        // 个股 K 线。指数请用 TdxHq_GetIndexBars。
        byte TdxHq_GetSecurityBars(int connectionId, byte category, byte market, String zqdm, short start, ShortByReference count, byte[] result, byte[] errInfo);

        // 指数 K 线，含涨跌家数。
        byte TdxHq_GetIndexBars(int connectionId, byte category, byte market, String zqdm, short start, ShortByReference count, byte[] result, byte[] errInfo);

        // 当日分时。
        byte TdxHq_GetMinuteTimeData(int connectionId, byte market, String zqdm, byte[] result, byte[] errInfo);

        // 历史分时。date 为 yyyyMMdd。
        byte TdxHq_GetHistoryMinuteTimeData(int connectionId, byte market, String zqdm, int date, byte[] result, byte[] errInfo);

        // 分笔成交。
        byte TdxHq_GetTransactionData(int connectionId, byte market, String zqdm, short start, ShortByReference count, byte[] result, byte[] errInfo);

        // 历史分笔。
        byte TdxHq_GetHistoryTransactionData(int connectionId, byte market, String zqdm, short start, ShortByReference count, int date, byte[] result, byte[] errInfo);

        // 五档行情。market 与 zqdm 等长，count 入出参。
        byte TdxHq_GetSecurityQuotes(int connectionId, byte[] market, String[] zqdm, ShortByReference count, byte[] result, byte[] errInfo);

        // 公司信息目录。
        byte TdxHq_GetCompanyInfoCategory(int connectionId, byte market, String zqdm, byte[] result, byte[] errInfo);

        // 公司信息正文。
        byte TdxHq_GetCompanyInfoContent(int connectionId, byte market, String zqdm, String fileName, int start, int length, byte[] result, byte[] errInfo);

        // 除权除息。
        byte TdxHq_GetXDXRInfo(int connectionId, byte[] market, String[] zqdm, int count, byte[] result, byte[] errInfo);

        // 财务简表。
        byte TdxHq_GetFinanceInfo(int connectionId, byte[] market, String[] zqdm, int count, byte[] result, byte[] errInfo);

        // 集合竞价。
        byte TdxHq_GetCallAuctionData(int connectionId, byte market, String zqdm, int start, IntByReference count, byte[] result, byte[] errInfo);

        // 十档行情。需要 L2。
        byte TdxHq_GetSecurityQuotes10(int connectionId, byte[] market, String[] zqdm, ShortByReference count, byte[] result, byte[] errInfo);

        // 买卖队列。需要 L2。
        byte TdxHq_GetBuySellQueue(int connectionId, byte[] market, String[] zqdm, int count, byte[] result, byte[] errInfo);

        // 逐笔成交。需要 L2。
        byte TdxHq_GetDetailTransactionData(int connectionId, byte market, String zqdm, int start, ShortByReference count, byte[] result, byte[] errInfo);

        // 逐笔成交原始数据。需要 L2。
        byte TdxHq_GetDetailOriginalTransactionData(int connectionId, byte market, String zqdm, int start, ShortByReference count, byte[] result, byte[] errInfo);

        // 逐笔委托。需要 L2。
        byte TdxHq_GetDetailOrderData(int connectionId, byte market, String zqdm, int start, ShortByReference count, byte[] result, byte[] errInfo);

        // 板块文件。blockFile 空则默认概念板块。名称是 GBK。
        byte TdxHq_GetBlockInfo(int connectionId, String blockFile, byte[] result, byte[] errInfo);

        // 下载报表文件到 savePath。
        byte TdxHq_GetReportFile(int connectionId, String fileName, String savePath, byte[] result, byte[] errInfo);

        // 板块列表。boardType: concept / style / industry / index。
        byte TdxHq_ListBoards(int connectionId, String boardType, byte[] result, byte[] errInfo);

        // 板块成分。boardName 为 GBK。
        byte TdxHq_ListBoardMembers(int connectionId, String boardName, String blockFile, byte[] result, byte[] errInfo);

        // 全市场涨跌统计。
        byte TdxHq_GetMarketStat(int connectionId, byte[] result, byte[] errInfo);

        // 当日资金流向。
        byte TdxHq_GetFundFlow(int connectionId, byte market, String zqdm, byte[] result, byte[] errInfo);

        // 历史资金流向。
        byte TdxHq_GetHistoryFundFlow(int connectionId, byte market, String zqdm, short start, short count, byte[] result, byte[] errInfo);

        // 从本机通达信目录读取站点。channel：1 L1，2 L2，3 扩展。
        byte TdxHq_LoadHostCfg(int channel, String tdxRoot, byte[] result, byte[] errInfo);

        // 连接扩展行情。与 A 股 ConnectionID 不要混用。
        int TdxExHq_Connect(String ip, int port, String account, String password, byte[] result, byte[] errInfo);

        // 断开扩展行情。
        void TdxExHq_Disconnect(int connectionId);

        // 扩展市场列表。
        byte TdxExHq_GetMarkets(int connectionId, byte[] result, byte[] errInfo);

        // 扩展品种数量。result 是数量。
        byte TdxExHq_GetInstrumentCount(int connectionId, IntByReference result, byte[] errInfo);

        // 扩展品种信息。
        byte TdxExHq_GetInstrumentInfo(int connectionId, int start, short count, byte[] result, byte[] errInfo);

        // 扩展 K 线。
        byte TdxExHq_GetInstrumentBars(int connectionId, byte category, byte market, String zqdm, int start, ShortByReference count, byte[] result, byte[] errInfo);

        // 扩展当日分时。
        byte TdxExHq_GetMinuteTimeData(int connectionId, byte market, String zqdm, byte[] result, byte[] errInfo);

        // 扩展分笔。
        byte TdxExHq_GetTransactionData(int connectionId, byte market, String zqdm, int start, ShortByReference count, byte[] result, byte[] errInfo);

        // 扩展单品种行情。
        byte TdxExHq_GetInstrumentQuote(int connectionId, byte market, String zqdm, byte[] result, byte[] errInfo);

        // 扩展历史分笔。
        byte TdxExHq_GetHistoryTransactionData(int connectionId, byte market, String zqdm, int date, int start, ShortByReference count, byte[] result, byte[] errInfo);

        // 扩展历史分时。
        byte TdxExHq_GetHistoryMinuteTimeData(int connectionId, byte market, String zqdm, int date, byte[] result, byte[] errInfo);
    }

    public static void main(String[] args) {
        System.setProperty("jna.encoding", "GBK");
        Api api = Native.load("Api.dll", Api.class);

        byte[] result = new byte[1024 * 1024];
        byte[] errInfo = new byte[256];

        int connectionId = api.TdxHq_Connect("YOUR_HQ_HOST", (short) 7709, "YOUR_ACCOUNT", "YOUR_PASSWORD", result, errInfo);
        System.out.println("TdxHq_Connect " + connectionId + " " + Native.toString(result, "GBK") + " " + Native.toString(errInfo, "GBK"));
        if (connectionId >= 0) {
            ShortByReference count = new ShortByReference((short) 1);
            api.TdxHq_GetSecurityQuotes(connectionId, new byte[] { 0 }, new String[] { "000001" }, count, result, errInfo);
            System.out.println("TdxHq_GetSecurityQuotes");
            System.out.println(Native.toString(result, "GBK"));
            System.out.println(Native.toString(errInfo, "GBK"));

            // ShortByReference n = new ShortByReference((short) 100);
            // api.TdxHq_GetSecurityBars(connectionId, (byte) 4, (byte) 0, "000001", (short) 0, n, result, errInfo);
            // api.TdxHq_GetMinuteTimeData(connectionId, (byte) 0, "000001", result, errInfo);

            api.TdxHq_Disconnect(connectionId);
        }
    }
}
