// Api.dll 交易调用示例。进程位数须与 DLL 一致（x86 或 x64）。
// 依赖 JNA。Result、ErrInfo 为 GBK。Result 是 TSV：行以 \n 分隔，列以 \t 分隔。
// ErrInfo 建议 >= 256 字节，Result 建议 >= 1MB。

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.win32.StdCallLibrary;

public class Demo {
    public interface Api extends StdCallLibrary {
        // 初始化交易运行时。进程内调用一次，须在 Logon 之前。
        void Init();

        // 释放全部会话并关闭运行时。调用后不可再使用此前的 ClientID。
        void UnInit();

        // 登录。成功返回 ClientID（>=0），失败返回 -1。
        // Config 只填账户类型：
        // 0 资金帐户  1 深圳帐户  2 上海帐户  3 基金帐户  4 深圳Ｂ股  5 上海Ｂ股  k 客户号
        // 示例: {"account_type":0}  或 {"account_type":"k"}
        int Logon(String ip, short port, String config, short yybId,
                  String accountNo, String tradeAccount, String jyPassword, String txPassword, byte[] errInfo);

        // 注销指定 ClientID。
        void Logoff(int clientId);

        // 查询当日数据。成功返回 0，失败返回 -1。
        // Category: 0资金 1股份 2当日委托 3当日成交 4可撤单 5股东代码
        //           6融资负债 7融券负债 8可融证券 9实时合约流水
        //           12可申购新股 13新股申购额度 14配号 15中签
        int QueryData(int clientId, int category, byte[] result, byte[] errInfo);

        // 查询历史数据。Category: 0历史委托 1历史成交 2交割单 3资金明细 4对账单。日期 yyyyMMdd。
        void QueryHistoryData(int clientId, int category, String startDate, String endDate, byte[] result, byte[] errInfo);

        // 同一账户批量查询。各数组长度均为 count。
        void QueryDatas(int clientId, int[] category, int count, Pointer[] result, Pointer[] errInfo);

        // 多账户批量查询。各数组长度均为 count。
        void QueryMultiAccountsDatas(int[] clientId, int[] category, int count, Pointer[] result, Pointer[] errInfo);

        // 下单。
        // Category: 0买入 1卖出 2融资买入 3融券卖出 4买券还券 5卖券还款 6现券还券 7新股申购
        // PriceType: 0限价；深市市价 1/2/3/4/5，沪市市价 1/2/4/6。price 为限价或保护限价。
        void SendOrder(int clientId, int category, int priceType, String gddm, String zqdm, float price, int quantity, byte[] result, byte[] errInfo);

        // 同一账户批量下单。各数组长度均为 count。
        void SendOrders(int clientId, int[] category, int[] priceType, String[] gddm, String[] zqdm, float[] price, int[] quantity, int count, Pointer[] result, Pointer[] errInfo);

        // 多账户批量下单。各数组长度均为 count。
        void SendMultiAccountsOrders(int[] clientId, int[] category, int[] priceType, String[] gddm, String[] zqdm, float[] price, int[] quantity, int count, Pointer[] result, Pointer[] errInfo);

        // 撤单。exchangeId：上海 "1"，深圳 "0"。hth 为委托编号。
        void CancelOrder(int clientId, String exchangeId, String hth, byte[] result, byte[] errInfo);

        // 同一账户批量撤单。各数组长度均为 count。
        void CancelOrders(int clientId, String[] exchangeId, String[] hth, int count, Pointer[] result, Pointer[] errInfo);

        // 多账户批量撤单。各数组长度均为 count。
        void CancelMultiAccountsOrders(int[] clientId, String[] exchangeId, String[] hth, int count, Pointer[] result, Pointer[] errInfo);

        // 五档行情。zqdm 可带 sz/sh/bj 前缀。
        void GetQuote(int clientId, String zqdm, byte[] result, byte[] errInfo);

        // 同一账户批量五档。各数组长度均为 count。
        void GetQuotes(int clientId, String[] zqdm, int count, Pointer[] result, Pointer[] errInfo);

        // 多账户批量五档。各数组长度均为 count。
        void GetMultiAccountsQuotes(int[] clientId, String[] zqdm, int count, Pointer[] result, Pointer[] errInfo);

        // 融资融券直接还款。amount 为金额字符串。
        void Repay(int clientId, String amount, byte[] result, byte[] errInfo);

        // 查询可买/可卖数量。成功 >=0，失败为负数。category、priceType 同下单。
        int GetCanBuySell(int clientId, int category, int priceType, String gddm, String zqdm, float price, byte[] result, byte[] errInfo);

        // 查询可交易数量。返回值优先取可买/可卖数量列。
        int GetTradableQuantity(int clientId, int category, int priceType, String gddm, String zqdm, float price, byte[] result, byte[] errInfo);

        // 非 0 时按列序重排 Result，默认关闭。
        void EnableResultColumnOrder(int enable);

        // 非 0 时表头附带字段 ID 后缀，默认关闭。
        void EnableResultColumnIdSuffix(int enable);

        // 查询股东代码。先查 Category 5，无数据时回落登录缓存。
        void GetShareholderCodes(int clientId, byte[] result, byte[] errInfo);

        // 一键打新。dryRun 非 0 只出计划不下单。失败返回 -1。
        int OneClickIpo(int clientId, int dryRun, byte[] result, byte[] errInfo);

        // 场内基金 / ETF。category: 0场内申购(金额) 1场内赎回(份额) 2ETF申购(份额) 3ETF赎回(份额)。
        // exchangeId: 0深圳 1上海 6北交所。amount 为金额或份额字符串。
        void FundOrder(int clientId, int category, String gddm, String zqdm, int exchangeId, String amount, byte[] result, byte[] errInfo);
    }

    public static void main(String[] args) {
        System.setProperty("jna.encoding", "GBK");
        Api api = Native.load("Api.dll", Api.class);

        byte[] result = new byte[1024 * 1024];
        byte[] errInfo = new byte[256];

        api.Init();

        // Config 只写账户类型。
        int clientId = api.Logon("YOUR_BROKER_IP", (short) 0, "{\"account_type\":0}", (short) 0,
                "YOUR_ACCOUNT", "YOUR_ACCOUNT", "YOUR_PASSWORD", "", errInfo);
        System.out.println("Logon ClientID=" + clientId + " " + Native.toString(errInfo, "GBK"));

        if (clientId >= 0) {
            api.QueryData(clientId, 0, result, errInfo);
            System.out.println("QueryData funds:");
            System.out.println(Native.toString(result, "GBK"));
            System.out.println(Native.toString(errInfo, "GBK"));

            // api.SendOrder(clientId, 0, 0, "股东代码", "证券代码", 0.0f, 100, result, errInfo);
            // api.CancelOrder(clientId, "1", "委托编号", result, errInfo);
            // api.Repay(clientId, "0", result, errInfo);

            api.Logoff(clientId);
        }

        api.UnInit();
    }
}
