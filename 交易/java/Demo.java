// thsapi.dll 交易调用示例。进程位数须与 DLL 一致（x86 或 x64）。
// 依赖 JNA。Result、ErrInfo 为 GBK。Result 是 TSV：行以 \n 分隔，列以 \t 分隔。
// ErrInfo 建议 >= 256 字节，Result 建议 >= 1024*1024 字节。没出错时 ErrInfo 为空字符串。

import com.sun.jna.Native;
import com.sun.jna.Pointer;
import com.sun.jna.win32.StdCallLibrary;

import java.util.ArrayList;
import java.util.List;

public class Demo {
    public interface ThsApi extends StdCallLibrary {
        // 初始化交易运行时。进程内调用一次，须在 Logon 之前。同 OpenTdx。
        void Init();

        // 释放全部会话并关闭运行时。调用后不可再使用此前的 ClientID。同 CloseTdx。
        void UnInit();

        // 交易账户登录。成功返回 ClientID（>=0），失败返回 -1。
        // ip：交易服务器 IP 或域名。port：端口。
        // config 只含两个字段：
        //   qs_id：券商 ID。模拟炒股为 356，须与 ip 是同一家。
        //   account_type：0 资金帐户，1 深圳，2 上海，3 基金，4 深圳Ｂ股，5 上海Ｂ股，k 客户号。
        //   示例: {"qs_id":356,"account_type":0}
        // yybId：营业部代码。模拟炒股为 48。
        // accountNo：登录账号。模拟账户的登录号和资金账号都填模拟资金账号。
        // tradeAccount：交易账号，一般与登录账号相同。空则按登录账号处理。
        // jyPassword：交易密码。
        // txPassword：通讯密码。大部分券商传空字符串，个别券商需要填写。
        // errInfo：出错说明，建议 256 字节。没出错时为空字符串。
        int Logon(String ip, short port, String config, short yybId,
                  String accountNo, String tradeAccount, String jyPassword, String txPassword, byte[] errInfo);

        // 注销指定 ClientID。
        void Logoff(int clientId);

        // 查询当日交易数据。成功返回 0，失败返回 -1。
        // category：
        //   0 资金  1 股份  2 当日委托  3 当日成交  4 可撤单  5 股东代码
        //   6 融资余额  7 融券余额  8 可融证券  9 实时合约流水
        //   12 可申购新股  13 新股申购额度  14 配号  15 中签
        // 普通账户上 6 以后的类别可能失败，甚至把连接踢掉。本示例只查 0–5。
        // result：表格。建议 1024*1024 字节。打开表头 ID 后，列名形如「合同编号_2135」。
        // 常用：2102 证券代码，2103 证券名称，2106 股东代码，2108 交易市场，
        // 2112 资金余额，2116 可用金额，2121 可用余额，2135 合同编号（撤单用的委托编号）。
        int QueryData(int clientId, int category, byte[] result, byte[] errInfo);

        // 历史查询。category：0 历史委托  1 历史成交  2 交割单  3 资金明细  4 对账单。
        // startDate / endDate 为 yyyyMMdd。无整型返回值，成败看 errInfo。
        void QueryHistoryData(int clientId, int category, String startDate, String endDate, byte[] result, byte[] errInfo);

        // 同一账户批量查询。category、result、errInfo 长度都是 count，下标一一对应。
        void QueryDatas(int clientId, int[] category, int count, Pointer[] result, Pointer[] errInfo);

        // 多账户批量查询。四个数组长度都是 count。
        void QueryMultiAccountsDatas(int[] clientId, int[] category, int count, Pointer[] result, Pointer[] errInfo);

        // 下委托。
        // category：0 买入  1 卖出  2 融资买入  3 融券卖出  4 买券还券  5 卖券还款  6 现券还券  7 新股申购。
        // priceType：0 限价；1 对方最优  2 本方最优  3 即时成交剩余撤销
        //   4 五档即成剩撤  5 全额成交或撤销（深圳）  6 五档即成转限价（上海）。
        // gddm：股东代码。上海股票填上海股东代码，深圳股票填深圳股东代码。
        // price：委托价格。市价时为保护限价。result 中含委托编号（字段 2135）。
        void SendOrder(int clientId, int category, int priceType, String gddm, String zqdm, float price, int quantity, byte[] result, byte[] errInfo);

        // 同一账户批量下单。各数组长度都是 count，语义同 SendOrder。
        void SendOrders(int clientId, int[] category, int[] priceType, String[] gddm, String[] zqdm, float[] price, int[] quantity, int count, Pointer[] result, Pointer[] errInfo);

        // 多账户批量下单。clientId 与各参数数组长度都是 count。
        void SendMultiAccountsOrders(int[] clientId, int[] category, int[] priceType, String[] gddm, String[] zqdm, float[] price, int[] quantity, int count, Pointer[] result, Pointer[] errInfo);

        // 撤委托。
        // exchangeId：上海 "1"，深圳 "0"（个别券商普通账户深圳是 "2"），北交所 "6"。
        // hth：要撤的委托编号，对应查询结果里的合同编号（字段 2135）。
        void CancelOrder(int clientId, String exchangeId, String hth, byte[] result, byte[] errInfo);

        // 同一账户批量撤单。exchangeId、hth、result、errInfo 长度都是 count。
        void CancelOrders(int clientId, String[] exchangeId, String[] hth, int count, Pointer[] result, Pointer[] errInfo);

        // 多账户批量撤单。各数组长度都是 count。
        void CancelMultiAccountsOrders(int[] clientId, String[] exchangeId, String[] hth, int count, Pointer[] result, Pointer[] errInfo);

        // 五档行情。zqdm 可带 sz/sh/bj 前缀。与交易共用 ClientID。
        void GetQuote(int clientId, String zqdm, byte[] result, byte[] errInfo);

        // 同一账户批量五档。zqdm、result、errInfo 长度都是 count。
        void GetQuotes(int clientId, String[] zqdm, int count, Pointer[] result, Pointer[] errInfo);

        // 多账户批量五档。各数组长度都是 count。
        void GetMultiAccountsQuotes(int[] clientId, String[] zqdm, int count, Pointer[] result, Pointer[] errInfo);

        // 融资融券直接还款。amount 为金额字符串。
        void Repay(int clientId, String amount, byte[] result, byte[] errInfo);

        // 查询可买/可卖数量。返回值 >=0 为数量，负数表示失败。category、priceType 同 SendOrder（不含新股申购）。
        int GetCanBuySell(int clientId, int category, int priceType, String gddm, String zqdm, float price, byte[] result, byte[] errInfo);

        // 查询可交易数量。Result 为明细 TSV，返回值优先取「可买数量/可卖数量」。
        int GetTradableQuantity(int clientId, int category, int priceType, String gddm, String zqdm, float price, byte[] result, byte[] errInfo);

        // 是否按列序配置重排 Result。enable 非 0 打开，默认关。与 EnableResultColumnIdSuffix 互不影响。
        void EnableResultColumnOrder(int enable);

        // 打开/关闭结果表头字段 ID 后缀（全局生效，默认关）。enable 非 0 时表头为「资金余额_2112」。
        void EnableResultColumnIdSuffix(int enable);

        // 股东代码列表。先走 QueryData(5)；没有行则回落登录时缓存的股东号。
        void GetShareholderCodes(int clientId, byte[] result, byte[] errInfo);

        // 一键打新。dryRun 非 0 只出计划不下单。
        // 返回值：试算为计划笔数，实盘为成功笔数，查询失败为 -1。
        int OneClickIpo(int clientId, int dryRun, byte[] result, byte[] errInfo);

        // 场内基金 / ETF 申购赎回。
        // category：0 场内申购(金额)  1 场内赎回(份额)  2 ETF申购(份额)  3 ETF赎回(份额)。
        // exchangeId：0 深圳，1 上海，6 北交所。amount 为金额或份额字符串。
        void FundOrder(int clientId, int category, String gddm, String zqdm, int exchangeId, String amount, byte[] result, byte[] errInfo);
    }

    private static final int[][] QUERY_ORDER = {
            {0}, {1}, {2}, {3}, {4}, {5}
    };
    private static final String[] QUERY_NAME = {"资金", "股份", "当日委托", "当日成交", "可撤单", "股东代码"};

    private static final int[][] FIELD_IDS = {
            {2172, 2112, 2116, 2114, 2113, 2191, 2973},
            {2102, 2103, 2108, 2106, 2117, 2121, 2122, 2124, 2125, 2147},
            {2139, 2140, 2102, 2103, 2109, 2127, 2126, 2128, 2135, 2108, 2106, 2105},
            {2141, 2142, 2135, 2130, 2102, 2103, 2109, 2128, 2129, 2131, 2108, 2106},
            {2139, 2140, 2102, 2103, 2109, 2127, 2126, 2128, 2135, 2108, 2106, 2105},
            {2106, 2107, 2171, 2167, 2908, 2176, 2172}
    };
    private static final String[][] FIELD_TITLES = {
            {"货币单位", "资金余额", "可用金额", "可取金额", "冻结金额", "总资产", "总市值"},
            {"证券代码", "证券名称", "交易市场", "股东代码", "股票余额", "可用余额", "成本价", "市价", "市值", "盈亏"},
            {"委托日期", "委托时间", "证券代码", "证券名称", "操作", "委托价格", "委托数量", "成交数量", "委托编号", "交易市场", "股东代码", "备注"},
            {"成交日期", "成交时间", "委托编号", "成交编号", "证券代码", "证券名称", "操作", "成交数量", "成交均价", "成交金额", "交易市场", "股东代码"},
            {"委托日期", "委托时间", "证券代码", "证券名称", "操作", "委托价格", "委托数量", "成交数量", "委托编号", "交易市场", "股东代码", "备注"},
            {"股东代码", "股东名称", "市场名称", "市场代码", "资金帐户", "是否主帐号", "货币单位"}
    };

    public static void main(String[] args) {
        System.setProperty("jna.encoding", "GBK");
        ThsApi api = Native.load("thsapi.dll", ThsApi.class);

        byte[] errInfo = new byte[256];

        // qs_id 356、端口 8002、营业部 48 是同花顺模拟炒股站点。账号和密码换成模拟资金账号。
        String config = "{\"qs_id\":356,\"account_type\":0}";
        api.Init();
        api.EnableResultColumnIdSuffix(1);

        int clientId = api.Logon("trade.10jqka.com.cn", (short) 8002, config, (short) 48,
                "YOUR_ACCOUNT", "YOUR_ACCOUNT", "YOUR_PASSWORD", "", errInfo);
        if (clientId < 0) {
            System.out.println("Logon 失败: " + gbk(errInfo));
            api.UnInit();
            return;
        }

        System.out.println("Logon 成功 clientId=" + clientId);
        System.out.println();

        String shareholder = "";
        String exchangeId = "1";
        String entrustId = "";

        for (int i = 0; i < QUERY_ORDER.length; i++) {
            byte[] result = new byte[1024 * 1024];
            byte[] err = new byte[256];
            int category = QUERY_ORDER[i][0];
            int rc = api.QueryData(clientId, category, result, err);
            String errText = gbk(err);
            System.out.println("======== QueryData " + category + " " + QUERY_NAME[i] + "  rc=" + rc + " ========");
            if (errText.length() > 0)
                System.out.println("err=" + errText);
            Table table = parse(gbk(result));
            printProjected(table, FIELD_IDS[i], FIELD_TITLES[i]);
            if (category == 5 && !table.rows.isEmpty()) {
                shareholder = cell(table, 0, 2106);
                String market = cell(table, 0, 2171);
                if (market.length() == 0)
                    market = cell(table, 0, 2108);
                exchangeId = exchangeIdOf(market);
                System.out.println("  股东代码 2106=" + shareholder + "  市场=" + market + "  exchangeId=" + exchangeId);
            }
            if (category == 2 && !table.rows.isEmpty() && entrustId.length() == 0) {
                entrustId = cell(table, 0, 2135);
                System.out.println("  委托编号 2135=" + entrustId);
            }
            System.out.println();
            if (errText.contains("断开")) {
                System.out.println("连接已断开，停止后续查询。");
                break;
            }
        }

        // 下单、撤单默认不执行。股东代码、交易所、委托编号来自上面的查询。
        // byte[] orderResult = new byte[1024 * 1024];
        // byte[] orderErr = new byte[256];
        // api.SendOrder(clientId, 0, 0, shareholder, "600000", 10.00f, 100, orderResult, orderErr);
        // api.CancelOrder(clientId, exchangeId, entrustId, orderResult, orderErr);

        System.out.println("下单/撤单已注释。股东=" + shareholder + " exchangeId=" + exchangeId + " 委托编号=" + entrustId);
        api.Logoff(clientId);
        api.UnInit();
    }

    /** 按字段 ID 取单元格。表头须已打开 ID 后缀，形如「合同编号_2135」或纯「2135」。 */
    private static String cell(Table table, int rowIndex, int fieldId) {
        if (rowIndex < 0 || rowIndex >= table.rows.size())
            return "";
        String id = Integer.toString(fieldId);
        String suffix = "_" + id;
        List<String> row = table.rows.get(rowIndex);
        for (int i = 0; i < table.headers.size(); i++) {
            String header = table.headers.get(i).trim();
            if (header.endsWith(suffix) || header.equals(id))
                return i < row.size() ? row.get(i).trim() : "";
        }
        return "";
    }

    /** 按指定字段 ID 抽出常用列。没有该 ID 时单元格为空，列仍然保留。 */
    private static void printProjected(Table raw, int[] ids, String[] titles) {
        if (ids.length == 0) {
            System.out.println("(空)");
            return;
        }
        StringBuilder head = new StringBuilder();
        for (int c = 0; c < ids.length; c++) {
            if (c > 0)
                head.append(" | ");
            head.append(titles[c]).append('_').append(ids[c]);
        }
        System.out.println(head);
        for (int r = 0; r < raw.rows.size(); r++) {
            StringBuilder line = new StringBuilder();
            for (int c = 0; c < ids.length; c++) {
                if (c > 0)
                    line.append(" | ");
                line.append(cell(raw, r, ids[c]));
            }
            System.out.println(line);
        }
        System.out.println("行数=" + raw.rows.size());
    }

    /** 交易市场文字换成 CancelOrder 的交易所 ID。深圳默认 "0"。 */
    private static String exchangeIdOf(String market) {
        if (market.contains("沪") || market.contains("上") || market.equals("1"))
            return "1";
        if (market.contains("北") || market.equals("6"))
            return "6";
        if (market.equals("2"))
            return "2";
        return "0";
    }

    private static Table parse(String text) {
        Table table = new Table();
        if (text == null || text.trim().length() == 0)
            return table;
        String[] lines = text.replace("\r\n", "\n").replace('\r', '\n').split("\n");
        boolean header = false;
        for (String line : lines) {
            if (line.length() == 0)
                continue;
            String[] cols = line.split("\t", -1);
            List<String> row = new ArrayList<String>();
            for (String col : cols)
                row.add(col);
            if (!header) {
                table.headers = row;
                header = true;
            } else {
                table.rows.add(row);
            }
        }
        return table;
    }

    private static String gbk(byte[] buf) {
        int n = 0;
        while (n < buf.length && buf[n] != 0)
            n++;
        try {
            return new String(buf, 0, n, "GBK");
        } catch (java.io.UnsupportedEncodingException e) {
            return "";
        }
    }

    private static final class Table {
        List<String> headers = new ArrayList<String>();
        List<List<String>> rows = new ArrayList<List<String>>();
    }
}
