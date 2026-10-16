// HqApi.dll L1 调用说明。DLL 与程序放在同一目录。进程位数须一致（x86 或 x64）。
// 依赖 JNA。Result、ErrInfo 里的中文是 GBK。表格行与行之间是 \n，列与列之间是 \t。
// ErrInfo 一般分配 256 字节，Result 一般分配 1024*1024 字节。
// L1 账号必须写成 L1*账号，不能用 guest，也不能留空。密码不能是空串。

import com.sun.jna.Native;
import com.sun.jna.ptr.IntByReference;
import com.sun.jna.ptr.ShortByReference;
import com.sun.jna.win32.StdCallLibrary;

public class Demo {
    public interface HqApi extends StdCallLibrary {
        // 连接通达信行情服务器。成功返回 ConnectionID（>=0），失败返回 -1。
        // L1 主站端口是 7709。account 必须写成 L1*账号。password 填该账号的密码。
        // result 成功时是表格，列：服务器名称、最后交易日期。
        int TdxHq_Connect(String ip, short port, String account, String password, byte[] result, byte[] errInfo);

        // 断开连接。断开后该 ConnectionID 不能再使用。
        void TdxHq_Disconnect(int connectionId);

        // 获取某个市场里证券的总数量。market：0 深圳，1 上海，2 北京。
        byte TdxHq_GetSecurityCount(int connectionId, byte market, ShortByReference result, byte[] errInfo);

        // 获取市场内从 start 开始的一批证券代码。一次最多约 1000 条。
        // count 调用前是希望返回的条数，调用后是实际条数。
        byte TdxHq_GetSecurityList(int connectionId, byte market, short start, ShortByReference count, byte[] result, byte[] errInfo);

        // 获取证券 K 线。指数请用 TdxHq_GetIndexBars。
        // category：0=5分钟，1=15分钟，2=30分钟，3=60分钟，4=日K，5=周K，6=月K，7=1分钟，8=1分钟，9=日K，10=季K，11=年K。
        // start 从最新一根往前数。count 最大 800。
        byte TdxHq_GetSecurityBars(int connectionId, byte category, byte market, String zqdm, short start, ShortByReference count, byte[] result, byte[] errInfo);

        // 获取指数 K 线。比个股多涨家数、跌家数两列。
        byte TdxHq_GetIndexBars(int connectionId, byte category, byte market, String zqdm, short start, ShortByReference count, byte[] result, byte[] errInfo);

        // 获取当日分时。列：现价、成交量、均价。
        byte TdxHq_GetMinuteTimeData(int connectionId, byte market, String zqdm, byte[] result, byte[] errInfo);

        // 获取历史某一天的分时。date 为 YYYYMMDD。
        byte TdxHq_GetHistoryMinuteTimeData(int connectionId, byte market, String zqdm, int date, byte[] result, byte[] errInfo);

        // 获取当日分笔。列：时间、价格、现量、笔数、买卖、保留。买卖：0=买，1=卖。
        byte TdxHq_GetTransactionData(int connectionId, byte market, String zqdm, short start, ShortByReference count, byte[] result, byte[] errInfo);

        // 获取历史某一天的分笔。列与当日相同。date 为 YYYYMMDD。
        byte TdxHq_GetHistoryTransactionData(int connectionId, byte market, String zqdm, short start, ShortByReference count, int date, byte[] result, byte[] errInfo);

        // 批量五档报价。market 和 zqdm 一一对应。DLL 按 80 只一包拆开再合并。
        byte TdxHq_GetSecurityQuotes(int connectionId, byte[] market, String[] zqdm, ShortByReference count, byte[] result, byte[] errInfo);

        // 获取 F10 分类目录。列：类别、文件、开始、长度。
        byte TdxHq_GetCompanyInfoCategory(int connectionId, byte market, String zqdm, byte[] result, byte[] errInfo);

        // 获取 F10 某一分类的正文。fileName、start、length 必须来自目录的同一行。
        byte TdxHq_GetCompanyInfoContent(int connectionId, byte market, String zqdm, String fileName, int start, int length, byte[] result, byte[] errInfo);

        // 获取除权除息。可以一次查多只。count 调用后不会改成返回行数。
        byte TdxHq_GetXDXRInfo(int connectionId, byte[] market, String[] zqdm, int count, byte[] result, byte[] errInfo);

        // 获取财务信息。可以一次查多只。
        byte TdxHq_GetFinanceInfo(int connectionId, byte[] market, String[] zqdm, int count, byte[] result, byte[] errInfo);

        // 获取集合竞价。整段早盘和尾盘建议 count 设到 2000。
        byte TdxHq_GetCallAuctionData(int connectionId, byte market, String zqdm, int start, IntByReference count, byte[] result, byte[] errInfo);
    }

    // L1 行情主站，端口都是 7709。名称|IP|端口
    private static final String[] SERVERS = {
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

    public static void main(String[] args) {
        System.setProperty("jna.encoding", "GBK");
        System.out.println("股票行情演示 L1:");
        HqApi api = Native.load("HqApi.dll", HqApi.class);

        byte[] result = new byte[1024 * 1024];
        byte[] err = new byte[256];
        int connectionId = -1;
        for (String item : SERVERS) {
            String[] parts = item.split("\\|");
            System.out.println("连接" + parts[0] + "[" + parts[1] + ":" + parts[2] + "]");
            clear(result);
            clear(err);
            connectionId = api.TdxHq_Connect(parts[1], Short.parseShort(parts[2]), "L1*YOUR_ACCOUNT", "YOUR_PASSWORD", result, err);
            System.out.println("TdxHq_Connect:" + gbk(result) + gbk(err));
            if (connectionId != -1)
                break;
        }
        if (connectionId == -1)
            return;

        ShortByReference count = new ShortByReference((short) 0);
        clear(err);
        api.TdxHq_GetSecurityCount(connectionId, (byte) 0, count, err);
        System.out.println("TdxHq_GetSecurityCount:" + (count.getValue() & 0xFFFF) + gbk(err));

        count.setValue((short) 10);
        clear(result);
        clear(err);
        api.TdxHq_GetSecurityList(connectionId, (byte) 0, (short) 0, count, result, err);
        System.out.println("TdxHq_GetSecurityList:" + gbk(result) + gbk(err));

        count.setValue((short) 10);
        api.TdxHq_GetSecurityBars(connectionId, (byte) 4, (byte) 0, "000001", (short) 0, count, result, err);
        System.out.println("TdxHq_GetSecurityBars:" + gbk(result) + gbk(err));

        count.setValue((short) 10);
        api.TdxHq_GetIndexBars(connectionId, (byte) 4, (byte) 0, "399001", (short) 0, count, result, err);
        System.out.println("TdxHq_GetIndexBars:" + gbk(result) + gbk(err));

        api.TdxHq_GetMinuteTimeData(connectionId, (byte) 0, "000001", result, err);
        System.out.println("TdxHq_GetMinuteTimeData:" + gbk(result) + gbk(err));

        api.TdxHq_GetHistoryMinuteTimeData(connectionId, (byte) 0, "000001", 20261016, result, err);
        System.out.println("TdxHq_GetHistoryMinuteTimeData:" + gbk(result) + gbk(err));

        count.setValue((short) 10);
        api.TdxHq_GetTransactionData(connectionId, (byte) 0, "000001", (short) 0, count, result, err);
        System.out.println("TdxHq_GetTransactionData:" + gbk(result) + gbk(err));

        count.setValue((short) 10);
        api.TdxHq_GetHistoryTransactionData(connectionId, (byte) 0, "000001", (short) 0, count, 20261016, result, err);
        System.out.println("TdxHq_GetHistoryTransactionData:" + gbk(result) + gbk(err));

        ShortByReference quoteCount = new ShortByReference((short) 2);
        api.TdxHq_GetSecurityQuotes(connectionId, new byte[] {0, 1}, new String[] {"000001", "600000"}, quoteCount, result, err);
        System.out.println("TdxHq_GetSecurityQuotes:" + gbk(result) + gbk(err));

        api.TdxHq_GetCompanyInfoCategory(connectionId, (byte) 0, "000001", result, err);
        System.out.println("TdxHq_GetCompanyInfoCategory:" + gbk(result) + gbk(err));

        api.TdxHq_GetCompanyInfoContent(connectionId, (byte) 0, "000001", "000001.txt", 0, 4096, result, err);
        System.out.println("TdxHq_GetCompanyInfoContent:" + gbk(result) + gbk(err));

        api.TdxHq_GetXDXRInfo(connectionId, new byte[] {0, 1}, new String[] {"000001", "600000"}, 2, result, err);
        System.out.println("TdxHq_GetXDXRInfo:" + gbk(result) + gbk(err));

        api.TdxHq_GetFinanceInfo(connectionId, new byte[] {0, 1}, new String[] {"000001", "600000"}, 2, result, err);
        System.out.println("TdxHq_GetFinanceInfo:" + gbk(result) + gbk(err));

        IntByReference auctionCount = new IntByReference(2000);
        api.TdxHq_GetCallAuctionData(connectionId, (byte) 0, "000001", 0, auctionCount, result, err);
        System.out.println("TdxHq_GetCallAuctionData:" + gbk(result) + gbk(err));

        api.TdxHq_Disconnect(connectionId);
    }

    private static void clear(byte[] buf) {
        for (int i = 0; i < buf.length; i++)
            buf[i] = 0;
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
}
