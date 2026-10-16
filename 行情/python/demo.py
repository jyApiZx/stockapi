# coding=utf-8
# HqApi.dll L1 调用说明。DLL 与脚本放在同一目录。进程位数须一致（x86 或 x64）。
# Result、ErrInfo 里的中文是 GBK。表格行与行之间是 \n，列与列之间是 \t。
# ErrInfo 一般分配 256 字节，Result 一般分配 1024*1024 字节。
# L1 账号必须写成 L1*账号，不能用 guest，也不能留空。密码不能是空串。

from ctypes import (
    WinDLL, c_int, c_ushort, c_byte, c_char_p, c_bool, POINTER, byref,
    create_string_buffer,
)

dll = WinDLL(".\\HqApi.dll")


def gbk(text):
    return text.encode("gbk")


def text(buf):
    return buf.value.decode("gbk", errors="replace")


# 连接通达信行情服务器。成功返回 ConnectionID（>=0），失败返回 -1。
# Port：L1 主站是 7709。Account：L1*账号。Password：该账号的密码。
# Result 成功时是表格，列：服务器名称、最后交易日期。
TdxHq_Connect = dll.TdxHq_Connect
TdxHq_Connect.argtypes = [c_char_p, c_ushort, c_char_p, c_char_p, c_char_p, c_char_p]
TdxHq_Connect.restype = c_int

# 断开连接。断开后该 ConnectionID 不能再使用。
TdxHq_Disconnect = dll.TdxHq_Disconnect
TdxHq_Disconnect.argtypes = [c_int]
TdxHq_Disconnect.restype = None

# 获取某个市场里证券的总数量。Market：0 深圳，1 上海，2 北京。Result 写入数量。
TdxHq_GetSecurityCount = dll.TdxHq_GetSecurityCount
TdxHq_GetSecurityCount.argtypes = [c_int, c_byte, POINTER(c_ushort), c_char_p]
TdxHq_GetSecurityCount.restype = c_bool

# 获取市场内从 Start 开始的一批证券代码。一次最多约 1000 条。
# Count 调用前是希望返回的条数，调用后是实际条数。
# Result 列：代码、一手股数、名称、保留、价格小数位数、昨收、保留、保留。
TdxHq_GetSecurityList = dll.TdxHq_GetSecurityList
TdxHq_GetSecurityList.argtypes = [c_int, c_byte, c_ushort, POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetSecurityList.restype = c_bool

# 获取证券 K 线。指数请用 TdxHq_GetIndexBars。
# Category：0=5分钟，1=15分钟，2=30分钟，3=60分钟，4=日K，5=周K，6=月K，7=1分钟，8=1分钟，9=日K，10=季K，11=年K。
# Start 从最新一根往前数，最新一根是 0。Count 最大 800。
# Result 列：时间、开盘价、收盘价、最高价、最低价、成交量、成交额。
TdxHq_GetSecurityBars = dll.TdxHq_GetSecurityBars
TdxHq_GetSecurityBars.argtypes = [c_int, c_byte, c_byte, c_char_p, c_ushort, POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetSecurityBars.restype = c_bool

# 获取指数 K 线。比个股多涨家数、跌家数两列。
TdxHq_GetIndexBars = dll.TdxHq_GetIndexBars
TdxHq_GetIndexBars.argtypes = [c_int, c_byte, c_byte, c_char_p, c_ushort, POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetIndexBars.restype = c_bool

# 获取当日分时。列：现价、成交量、均价。
TdxHq_GetMinuteTimeData = dll.TdxHq_GetMinuteTimeData
TdxHq_GetMinuteTimeData.argtypes = [c_int, c_byte, c_char_p, c_char_p, c_char_p]
TdxHq_GetMinuteTimeData.restype = c_bool

# 获取历史某一天的分时。Date 为 YYYYMMDD。
TdxHq_GetHistoryMinuteTimeData = dll.TdxHq_GetHistoryMinuteTimeData
TdxHq_GetHistoryMinuteTimeData.argtypes = [c_int, c_byte, c_char_p, c_int, c_char_p, c_char_p]
TdxHq_GetHistoryMinuteTimeData.restype = c_bool

# 获取当日分笔。列：时间、价格、现量、笔数、买卖、保留。买卖：0=买，1=卖。
TdxHq_GetTransactionData = dll.TdxHq_GetTransactionData
TdxHq_GetTransactionData.argtypes = [c_int, c_byte, c_char_p, c_ushort, POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetTransactionData.restype = c_bool

# 获取历史某一天的分笔。列与当日相同。Date 为 YYYYMMDD。
TdxHq_GetHistoryTransactionData = dll.TdxHq_GetHistoryTransactionData
TdxHq_GetHistoryTransactionData.argtypes = [c_int, c_byte, c_char_p, c_ushort, POINTER(c_ushort), c_int, c_char_p, c_char_p]
TdxHq_GetHistoryTransactionData.restype = c_bool

# 批量五档报价。Market 和 Zqdm 一一对应。DLL 按 80 只一包拆开再合并。
TdxHq_GetSecurityQuotes = dll.TdxHq_GetSecurityQuotes
TdxHq_GetSecurityQuotes.argtypes = [c_int, POINTER(c_byte), POINTER(c_char_p), POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetSecurityQuotes.restype = c_bool

# 获取 F10 分类目录。列：类别、文件、开始、长度。
TdxHq_GetCompanyInfoCategory = dll.TdxHq_GetCompanyInfoCategory
TdxHq_GetCompanyInfoCategory.argtypes = [c_int, c_byte, c_char_p, c_char_p, c_char_p]
TdxHq_GetCompanyInfoCategory.restype = c_bool

# 获取 F10 某一分类的正文。FileName、Start、Length 必须来自目录的同一行。
TdxHq_GetCompanyInfoContent = dll.TdxHq_GetCompanyInfoContent
TdxHq_GetCompanyInfoContent.argtypes = [c_int, c_byte, c_char_p, c_char_p, c_int, c_int, c_char_p, c_char_p]
TdxHq_GetCompanyInfoContent.restype = c_bool

# 获取除权除息。可以一次查多只。Count 调用后不会改成返回行数。
TdxHq_GetXDXRInfo = dll.TdxHq_GetXDXRInfo
TdxHq_GetXDXRInfo.argtypes = [c_int, POINTER(c_byte), POINTER(c_char_p), c_int, c_char_p, c_char_p]
TdxHq_GetXDXRInfo.restype = c_bool

# 获取财务信息。可以一次查多只。
TdxHq_GetFinanceInfo = dll.TdxHq_GetFinanceInfo
TdxHq_GetFinanceInfo.argtypes = [c_int, POINTER(c_byte), POINTER(c_char_p), c_int, c_char_p, c_char_p]
TdxHq_GetFinanceInfo.restype = c_bool

# 获取集合竞价。整段早盘和尾盘建议 Count 设到 2000。
TdxHq_GetCallAuctionData = dll.TdxHq_GetCallAuctionData
TdxHq_GetCallAuctionData.argtypes = [c_int, c_byte, c_char_p, c_int, POINTER(c_int), c_char_p, c_char_p]
TdxHq_GetCallAuctionData.restype = c_bool


# L1 行情主站，端口都是 7709。名称|IP|端口
SERVERS = [
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
]


def main():
    result = create_string_buffer(1024 * 1024)
    err = create_string_buffer(256)
    account = gbk("L1*YOUR_ACCOUNT")
    password = gbk("YOUR_PASSWORD")

    connection_id = -1
    for item in SERVERS:
        name, ip, port = item.split("|")
        print("连接%s[%s:%s]" % (name, ip, port))
        connection_id = TdxHq_Connect(gbk(ip), int(port), account, password, result, err)
        print("TdxHq_Connect:" + text(result) + text(err))
        if connection_id != -1:
            break
    if connection_id == -1:
        return

    market = 0
    count = c_ushort(0)
    TdxHq_GetSecurityCount(connection_id, market, byref(count), err)
    print("TdxHq_GetSecurityCount:%s%s" % (count.value, text(err)))

    count = c_ushort(10)
    TdxHq_GetSecurityList(connection_id, market, 0, byref(count), result, err)
    print("TdxHq_GetSecurityList:" + text(result) + text(err))

    count = c_ushort(10)
    TdxHq_GetSecurityBars(connection_id, 4, market, gbk("000001"), 0, byref(count), result, err)
    print("TdxHq_GetSecurityBars:" + text(result) + text(err))

    count = c_ushort(10)
    TdxHq_GetIndexBars(connection_id, 4, market, gbk("399001"), 0, byref(count), result, err)
    print("TdxHq_GetIndexBars:" + text(result) + text(err))

    TdxHq_GetMinuteTimeData(connection_id, market, gbk("000001"), result, err)
    print("TdxHq_GetMinuteTimeData:" + text(result) + text(err))

    TdxHq_GetHistoryMinuteTimeData(connection_id, market, gbk("000001"), 20261016, result, err)
    print("TdxHq_GetHistoryMinuteTimeData:" + text(result) + text(err))

    count = c_ushort(10)
    TdxHq_GetTransactionData(connection_id, market, gbk("000001"), 0, byref(count), result, err)
    print("TdxHq_GetTransactionData:" + text(result) + text(err))

    count = c_ushort(10)
    TdxHq_GetHistoryTransactionData(connection_id, market, gbk("000001"), 0, byref(count), 20261016, result, err)
    print("TdxHq_GetHistoryTransactionData:" + text(result) + text(err))

    markets = (c_byte * 2)(0, 1)
    codes = (c_char_p * 2)(gbk("000001"), gbk("600000"))
    quote_count = c_ushort(2)
    TdxHq_GetSecurityQuotes(connection_id, markets, codes, byref(quote_count), result, err)
    print("TdxHq_GetSecurityQuotes:" + text(result) + text(err))

    TdxHq_GetCompanyInfoCategory(connection_id, market, gbk("000001"), result, err)
    print("TdxHq_GetCompanyInfoCategory:" + text(result) + text(err))

    TdxHq_GetCompanyInfoContent(connection_id, market, gbk("000001"), gbk("000001.txt"), 0, 4096, result, err)
    print("TdxHq_GetCompanyInfoContent:" + text(result) + text(err))

    TdxHq_GetXDXRInfo(connection_id, markets, codes, 2, result, err)
    print("TdxHq_GetXDXRInfo:" + text(result) + text(err))

    TdxHq_GetFinanceInfo(connection_id, markets, codes, 2, result, err)
    print("TdxHq_GetFinanceInfo:" + text(result) + text(err))

    auction_count = c_int(2000)
    TdxHq_GetCallAuctionData(connection_id, market, gbk("000001"), 0, byref(auction_count), result, err)
    print("TdxHq_GetCallAuctionData:" + text(result) + text(err))

    TdxHq_Disconnect(connection_id)


if __name__ == "__main__":
    print("股票行情演示 L1:")
    main()
