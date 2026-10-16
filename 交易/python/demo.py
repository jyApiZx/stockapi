# coding=utf-8
# thsapi.dll 交易调用示例。进程位数须与 DLL 一致（x86 或 x64）。
# 字符串按 GBK 传入。Result 是 TSV：行以 \n 分隔，列以 \t 分隔。
# ErrInfo 建议 >= 256 字节，Result 建议 >= 1024*1024 字节。没出错时 ErrInfo 为空字符串。

from ctypes import (
    WinDLL, c_int, c_short, c_float, c_char_p, POINTER,
    create_string_buffer,
)

dll = WinDLL(".\\thsapi.dll")


def gbk(text):
    return text.encode("gbk")


def text(buf):
    return buf.value.decode("gbk", errors="replace")


# 初始化交易运行时。进程内调用一次，须在 Logon 之前。
Init = dll.Init
Init.argtypes = []
Init.restype = None

# 释放全部会话并关闭运行时。调用后不可再使用此前的 ClientID。
UnInit = dll.UnInit
UnInit.argtypes = []
UnInit.restype = None

# 交易账户登录。成功返回 ClientID（>=0），失败返回 -1。
# ip：交易服务器 IP 或域名。port：端口。
# config 只含两个字段：
#   qs_id：券商 ID。模拟炒股为 356，须与 ip 是同一家。
#   account_type：0 资金帐户，1 深圳，2 上海，3 基金，4 深圳Ｂ股，5 上海Ｂ股，k 客户号。
#   示例: {"qs_id":356,"account_type":0}
# yyb_id：营业部代码。模拟炒股为 48。
# account_no：登录账号。模拟账户的登录号和资金账号都填模拟资金账号。
# trade_account：交易账号，一般与登录账号相同。空则按登录账号处理。
# jy_password：交易密码。
# tx_password：通讯密码。大部分券商传空字符串，个别券商需要填写。
# err_info：出错说明，建议 256 字节。没出错时为空字符串。
Logon = dll.Logon
Logon.argtypes = [c_char_p, c_short, c_char_p, c_short, c_char_p, c_char_p, c_char_p, c_char_p, c_char_p]
Logon.restype = c_int

# 注销指定 ClientID。
Logoff = dll.Logoff
Logoff.argtypes = [c_int]
Logoff.restype = None

# 查询当日交易数据。成功返回 0，失败返回 -1。
# category：
#   0 资金  1 股份  2 当日委托  3 当日成交  4 可撤单  5 股东代码
#   6 融资余额  7 融券余额  8 可融证券  9 实时合约流水
#   12 可申购新股  13 新股申购额度  14 配号  15 中签
# 普通账户上 6 以后的类别可能失败，甚至把连接踢掉。本示例只查 0–5。
# result：表格。行以 \n 分隔，列以 \t 分隔，首行是表头。建议 1024*1024 字节。
# 打开表头 ID 后，列名形如「合同编号_2135」。
# 常用：2102 证券代码，2103 证券名称，2106 股东代码，2108 交易市场，
# 2112 资金余额，2116 可用金额，2121 可用余额，2135 合同编号（撤单用的委托编号）。
QueryData = dll.QueryData
QueryData.argtypes = [c_int, c_int, c_char_p, c_char_p]
QueryData.restype = c_int

# 历史查询。category：0 历史委托  1 历史成交  2 交割单  3 资金明细  4 对账单。
# start_date / end_date 为 yyyyMMdd。无整型返回值，成败看 err_info。
QueryHistoryData = dll.QueryHistoryData
QueryHistoryData.argtypes = [c_int, c_int, c_char_p, c_char_p, c_char_p, c_char_p]
QueryHistoryData.restype = None

# 同一账户批量查询。category、result、err_info 长度都是 count，下标一一对应。
QueryDatas = dll.QueryDatas
QueryDatas.argtypes = [c_int, POINTER(c_int), c_int, POINTER(c_char_p), POINTER(c_char_p)]
QueryDatas.restype = None

# 多账户批量查询。四个数组长度都是 count。
QueryMultiAccountsDatas = dll.QueryMultiAccountsDatas
QueryMultiAccountsDatas.argtypes = [POINTER(c_int), POINTER(c_int), c_int, POINTER(c_char_p), POINTER(c_char_p)]
QueryMultiAccountsDatas.restype = None

# 下委托。
# category：0 买入  1 卖出  2 融资买入  3 融券卖出  4 买券还券  5 卖券还款  6 现券还券  7 新股申购。
# price_type：0 限价；1 对方最优  2 本方最优  3 即时成交剩余撤销
#   4 五档即成剩撤  5 全额成交或撤销（深圳）  6 五档即成转限价（上海）。
# gddm：股东代码。上海股票填上海股东代码，深圳股票填深圳股东代码。
# price：委托价格。市价时为保护限价。
# result 中含委托编号（字段 2135 合同编号）。
SendOrder = dll.SendOrder
SendOrder.argtypes = [c_int, c_int, c_int, c_char_p, c_char_p, c_float, c_int, c_char_p, c_char_p]
SendOrder.restype = None

# 同一账户批量下单。各数组长度都是 count，语义同 SendOrder。
SendOrders = dll.SendOrders
SendOrders.argtypes = [
    c_int, POINTER(c_int), POINTER(c_int), POINTER(c_char_p), POINTER(c_char_p),
    POINTER(c_float), POINTER(c_int), c_int, POINTER(c_char_p), POINTER(c_char_p),
]
SendOrders.restype = None

# 多账户批量下单。client_id 与各参数数组长度都是 count。
SendMultiAccountsOrders = dll.SendMultiAccountsOrders
SendMultiAccountsOrders.argtypes = [
    POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_char_p), POINTER(c_char_p),
    POINTER(c_float), POINTER(c_int), c_int, POINTER(c_char_p), POINTER(c_char_p),
]
SendMultiAccountsOrders.restype = None

# 撤委托。
# exchange_id：上海 "1"，深圳 "0"（个别券商普通账户深圳是 "2"），北交所 "6"。
# hth：要撤的委托编号，对应查询结果里的合同编号（字段 2135）。
CancelOrder = dll.CancelOrder
CancelOrder.argtypes = [c_int, c_char_p, c_char_p, c_char_p, c_char_p]
CancelOrder.restype = None

# 同一账户批量撤单。exchange_id、hth、result、err_info 长度都是 count。
CancelOrders = dll.CancelOrders
CancelOrders.argtypes = [c_int, POINTER(c_char_p), POINTER(c_char_p), c_int, POINTER(c_char_p), POINTER(c_char_p)]
CancelOrders.restype = None

# 多账户批量撤单。各数组长度都是 count。
CancelMultiAccountsOrders = dll.CancelMultiAccountsOrders
CancelMultiAccountsOrders.argtypes = [POINTER(c_int), POINTER(c_char_p), POINTER(c_char_p), c_int, POINTER(c_char_p), POINTER(c_char_p)]
CancelMultiAccountsOrders.restype = None

# 五档行情。zqdm 可带 sz/sh/bj 前缀。与交易共用 ClientID。
GetQuote = dll.GetQuote
GetQuote.argtypes = [c_int, c_char_p, c_char_p, c_char_p]
GetQuote.restype = None

# 同一账户批量五档。zqdm、result、err_info 长度都是 count。
GetQuotes = dll.GetQuotes
GetQuotes.argtypes = [c_int, POINTER(c_char_p), c_int, POINTER(c_char_p), POINTER(c_char_p)]
GetQuotes.restype = None

# 多账户批量五档。各数组长度都是 count。
GetMultiAccountsQuotes = dll.GetMultiAccountsQuotes
GetMultiAccountsQuotes.argtypes = [POINTER(c_int), POINTER(c_char_p), c_int, POINTER(c_char_p), POINTER(c_char_p)]
GetMultiAccountsQuotes.restype = None

# 融资融券直接还款。amount 为金额字符串。
Repay = dll.Repay
Repay.argtypes = [c_int, c_char_p, c_char_p, c_char_p]
Repay.restype = None

# 查询可买/可卖数量。返回值 >=0 为数量，负数表示失败。category、price_type 同 SendOrder（不含新股申购）。
GetCanBuySell = dll.GetCanBuySell
GetCanBuySell.argtypes = [c_int, c_int, c_int, c_char_p, c_char_p, c_float, c_char_p, c_char_p]
GetCanBuySell.restype = c_int

# 查询可交易数量。Result 为明细 TSV，返回值优先取「可买数量/可卖数量」。
GetTradableQuantity = dll.GetTradableQuantity
GetTradableQuantity.argtypes = [c_int, c_int, c_int, c_char_p, c_char_p, c_float, c_char_p, c_char_p]
GetTradableQuantity.restype = c_int

# 是否按列序配置重排 Result。enable 非 0 打开，默认关。与 EnableResultColumnIdSuffix 互不影响。
EnableResultColumnOrder = dll.EnableResultColumnOrder
EnableResultColumnOrder.argtypes = [c_int]
EnableResultColumnOrder.restype = None

# 打开/关闭结果表头字段 ID 后缀（全局生效，默认关）。enable 非 0 时表头为「资金余额_2112」。
EnableResultColumnIdSuffix = dll.EnableResultColumnIdSuffix
EnableResultColumnIdSuffix.argtypes = [c_int]
EnableResultColumnIdSuffix.restype = None

# 股东代码列表。先走 QueryData(5)；没有行则回落登录时缓存的股东号。
GetShareholderCodes = dll.GetShareholderCodes
GetShareholderCodes.argtypes = [c_int, c_char_p, c_char_p]
GetShareholderCodes.restype = None

# 一键打新。dry_run 非 0 只出计划不下单。
# 返回值：试算为计划笔数，实盘为成功笔数，查询失败为 -1。
OneClickIpo = dll.OneClickIpo
OneClickIpo.argtypes = [c_int, c_int, c_char_p, c_char_p]
OneClickIpo.restype = c_int

# 场内基金 / ETF 申购赎回。
# category：0 场内申购(金额)  1 场内赎回(份额)  2 ETF申购(份额)  3 ETF赎回(份额)。
# exchange_id：0 深圳，1 上海，6 北交所。amount 为金额或份额字符串。
FundOrder = dll.FundOrder
FundOrder.argtypes = [c_int, c_int, c_char_p, c_char_p, c_int, c_char_p, c_char_p, c_char_p]
FundOrder.restype = None


# 本示例只查 0–5。两融和新股类别在普通账户上可能失败，甚至把连接踢掉。
QUERY_ORDER = (
    (0, "资金"),
    (1, "股份"),
    (2, "当日委托"),
    (3, "当日成交"),
    (4, "可撤单"),
    (5, "股东代码"),
)

# 各类查询要留下的常用字段。顺序就是打印的列序。
COMMON_FIELDS = {
    0: ((2172, "货币单位"), (2112, "资金余额"), (2116, "可用金额"), (2114, "可取金额"), (2113, "冻结金额"), (2191, "总资产"), (2973, "总市值")),
    1: ((2102, "证券代码"), (2103, "证券名称"), (2108, "交易市场"), (2106, "股东代码"), (2117, "股票余额"), (2121, "可用余额"), (2122, "成本价"), (2124, "市价"), (2125, "市值"), (2147, "盈亏")),
    2: ((2139, "委托日期"), (2140, "委托时间"), (2102, "证券代码"), (2103, "证券名称"), (2109, "操作"), (2127, "委托价格"), (2126, "委托数量"), (2128, "成交数量"), (2135, "委托编号"), (2108, "交易市场"), (2106, "股东代码"), (2105, "备注")),
    3: ((2141, "成交日期"), (2142, "成交时间"), (2135, "委托编号"), (2130, "成交编号"), (2102, "证券代码"), (2103, "证券名称"), (2109, "操作"), (2128, "成交数量"), (2129, "成交均价"), (2131, "成交金额"), (2108, "交易市场"), (2106, "股东代码")),
    4: ((2139, "委托日期"), (2140, "委托时间"), (2102, "证券代码"), (2103, "证券名称"), (2109, "操作"), (2127, "委托价格"), (2126, "委托数量"), (2128, "成交数量"), (2135, "委托编号"), (2108, "交易市场"), (2106, "股东代码"), (2105, "备注")),
    5: ((2106, "股东代码"), (2107, "股东名称"), (2171, "市场名称"), (2167, "市场代码"), (2908, "资金帐户"), (2176, "是否主帐号"), (2172, "货币单位")),
}


def parse_table(raw):
    lines = [line for line in raw.replace("\r\n", "\n").replace("\r", "\n").split("\n") if line]
    if not lines:
        return [], []
    headers = lines[0].split("\t")
    rows = [line.split("\t") for line in lines[1:]]
    return headers, rows


def cell(headers, row, field_id):
    """按字段 ID 取单元格。表头须已打开 ID 后缀，形如「合同编号_2135」或纯「2135」。"""
    suffix = "_" + str(field_id)
    ident = str(field_id)
    for i, header in enumerate(headers):
        name = header.strip()
        if name.endswith(suffix) or name == ident:
            return row[i].strip() if i < len(row) else ""
    return ""


def project(headers, rows, fields):
    """按指定字段 ID 抽出常用列。没有该 ID 时单元格为空，列仍然保留。"""
    titles = ["%s_%s" % (title, field_id) for field_id, title in fields]
    view = []
    for row in rows:
        view.append([cell(headers, row, field_id) for field_id, _title in fields])
    return titles, view


def print_table(titles, rows):
    if not titles:
        print("(空)")
        return
    print(" | ".join(titles))
    for row in rows:
        print(" | ".join(row))
    print("行数=%s" % len(rows))


def exchange_id_of(market):
    """交易市场文字换成 CancelOrder 的交易所 ID。深圳默认 "0"。"""
    if ("沪" in market) or ("上" in market) or market == "1":
        return "1"
    if ("北" in market) or market == "6":
        return "6"
    if market == "2":
        return "2"
    return "0"


def query(client_id, category, name):
    result = create_string_buffer(1024 * 1024)
    err = create_string_buffer(256)
    rc = QueryData(client_id, category, result, err)
    err_text = text(err)
    print("======== QueryData %s %s  rc=%s ========" % (category, name, rc))
    if err_text:
        print("err=" + err_text)
    headers, rows = parse_table(text(result))
    titles, view = project(headers, rows, COMMON_FIELDS[category])
    print_table(titles, view)
    disconnected = "断开" in err_text
    if disconnected:
        print("连接已断开，停止后续查询。")
    return headers, rows, disconnected


def main():
    err_info = create_string_buffer(256)

    # qs_id 356、端口 8002、营业部 48 是同花顺模拟炒股站点。账号和密码换成模拟资金账号。
    config = '{"qs_id":356,"account_type":0}'

    Init()
    EnableResultColumnIdSuffix(1)

    client_id = Logon(
        gbk("trade.10jqka.com.cn"), 8002, gbk(config), 48,
        gbk("YOUR_ACCOUNT"), gbk("YOUR_ACCOUNT"), gbk("YOUR_PASSWORD"), gbk(""), err_info)
    if client_id < 0:
        print("Logon 失败: " + text(err_info))
        UnInit()
        return

    print("Logon 成功 clientId=%s" % client_id)
    print("")

    shareholder = ""
    exchange_id = "1"
    entrust_id = ""

    for category, name in QUERY_ORDER:
        headers, rows, disconnected = query(client_id, category, name)
        if category == 5 and rows:
            shareholder = cell(headers, rows[0], 2106)
            market = cell(headers, rows[0], 2171) or cell(headers, rows[0], 2108)
            exchange_id = exchange_id_of(market)
            print("  股东代码 2106=%s  市场=%s  exchangeId=%s" % (shareholder, market, exchange_id))
        if category == 2 and rows and not entrust_id:
            entrust_id = cell(headers, rows[0], 2135)
            print("  委托编号 2135=%s" % entrust_id)
        print("")
        if disconnected:
            break

    # 下单、撤单默认不执行。股东代码、交易所、委托编号来自上面的查询。
    # result = create_string_buffer(1024 * 1024)
    # order_err = create_string_buffer(256)
    # SendOrder(client_id, 0, 0, gbk(shareholder), gbk("600000"), 10.00, 100, result, order_err)
    # print("SendOrder err=" + text(order_err))
    # cancel_err = create_string_buffer(256)
    # CancelOrder(client_id, gbk(exchange_id), gbk(entrust_id), result, cancel_err)
    # print("CancelOrder err=" + text(cancel_err))

    print("下单/撤单已注释。股东=%s exchangeId=%s 委托编号=%s" % (shareholder, exchange_id, entrust_id))
    Logoff(client_id)
    UnInit()


if __name__ == "__main__":
    main()
