# coding=utf-8
# Api.dll 交易调用示例。进程位数须与 DLL 一致（x86 或 x64）。
# 字符串按 GBK 传入。Result 是 TSV：行以 \n 分隔，列以 \t 分隔。
# ErrInfo 建议 >= 256 字节，Result 建议 >= 1MB。

from ctypes import (
    WinDLL, c_int, c_short, c_float, c_char_p, POINTER,
    create_string_buffer,
)

dll = WinDLL(".\\Api.dll")


def gbk(text):
    return text.encode("gbk")


# 初始化交易运行时。进程内调用一次，须在 Logon 之前。
Init = dll.Init
Init.argtypes = []
Init.restype = None

# 释放全部会话并关闭运行时。调用后不可再使用此前的 ClientID。
UnInit = dll.UnInit
UnInit.argtypes = []
UnInit.restype = None

# 登录。成功返回 ClientID（>=0），失败返回 -1。
# Config 只填账户类型：
# 0 资金帐户  1 深圳帐户  2 上海帐户  3 基金帐户  4 深圳Ｂ股  5 上海Ｂ股  k 客户号
# 示例: {"account_type":0}  或 {"account_type":"k"}
Logon = dll.Logon
Logon.argtypes = [c_char_p, c_short, c_char_p, c_short, c_char_p, c_char_p, c_char_p, c_char_p, c_char_p]
Logon.restype = c_int

# 注销指定 ClientID。
Logoff = dll.Logoff
Logoff.argtypes = [c_int]
Logoff.restype = None

# 查询当日数据。成功返回 0，失败返回 -1。
# Category: 0资金 1股份 2当日委托 3当日成交 4可撤单 5股东代码
#           6融资负债 7融券负债 8可融证券 9实时合约流水
#           12可申购新股 13新股申购额度 14配号 15中签
QueryData = dll.QueryData
QueryData.argtypes = [c_int, c_int, c_char_p, c_char_p]
QueryData.restype = c_int

# 查询历史数据。Category: 0历史委托 1历史成交 2交割单 3资金明细 4对账单。日期 yyyyMMdd。
QueryHistoryData = dll.QueryHistoryData
QueryHistoryData.argtypes = [c_int, c_int, c_char_p, c_char_p, c_char_p, c_char_p]
QueryHistoryData.restype = None

# 同一账户批量查询。各数组长度均为 Count。
QueryDatas = dll.QueryDatas
QueryDatas.argtypes = [c_int, POINTER(c_int), c_int, POINTER(c_char_p), POINTER(c_char_p)]
QueryDatas.restype = None

# 多账户批量查询。各数组长度均为 Count。
QueryMultiAccountsDatas = dll.QueryMultiAccountsDatas
QueryMultiAccountsDatas.argtypes = [POINTER(c_int), POINTER(c_int), c_int, POINTER(c_char_p), POINTER(c_char_p)]
QueryMultiAccountsDatas.restype = None

# 下单。
# Category: 0买入 1卖出 2融资买入 3融券卖出 4买券还券 5卖券还款 6现券还券 7新股申购
# PriceType: 0限价；深市市价 1/2/3/4/5，沪市市价 1/2/4/6。Price 为限价或保护限价。
SendOrder = dll.SendOrder
SendOrder.argtypes = [c_int, c_int, c_int, c_char_p, c_char_p, c_float, c_int, c_char_p, c_char_p]
SendOrder.restype = None

# 同一账户批量下单。各数组长度均为 Count。
SendOrders = dll.SendOrders
SendOrders.argtypes = [
    c_int, POINTER(c_int), POINTER(c_int), POINTER(c_char_p), POINTER(c_char_p),
    POINTER(c_float), POINTER(c_int), c_int, POINTER(c_char_p), POINTER(c_char_p),
]
SendOrders.restype = None

# 多账户批量下单。各数组长度均为 Count。
SendMultiAccountsOrders = dll.SendMultiAccountsOrders
SendMultiAccountsOrders.argtypes = [
    POINTER(c_int), POINTER(c_int), POINTER(c_int), POINTER(c_char_p), POINTER(c_char_p),
    POINTER(c_float), POINTER(c_int), c_int, POINTER(c_char_p), POINTER(c_char_p),
]
SendMultiAccountsOrders.restype = None

# 撤单。ExchangeID：上海 "1"，深圳 "0"。hth 为委托编号。
CancelOrder = dll.CancelOrder
CancelOrder.argtypes = [c_int, c_char_p, c_char_p, c_char_p, c_char_p]
CancelOrder.restype = None

# 同一账户批量撤单。各数组长度均为 Count。
CancelOrders = dll.CancelOrders
CancelOrders.argtypes = [c_int, POINTER(c_char_p), POINTER(c_char_p), c_int, POINTER(c_char_p), POINTER(c_char_p)]
CancelOrders.restype = None

# 多账户批量撤单。各数组长度均为 Count。
CancelMultiAccountsOrders = dll.CancelMultiAccountsOrders
CancelMultiAccountsOrders.argtypes = [POINTER(c_int), POINTER(c_char_p), POINTER(c_char_p), c_int, POINTER(c_char_p), POINTER(c_char_p)]
CancelMultiAccountsOrders.restype = None

# 五档行情。Zqdm 可带 sz/sh/bj 前缀。
GetQuote = dll.GetQuote
GetQuote.argtypes = [c_int, c_char_p, c_char_p, c_char_p]
GetQuote.restype = None

# 同一账户批量五档。各数组长度均为 Count。
GetQuotes = dll.GetQuotes
GetQuotes.argtypes = [c_int, POINTER(c_char_p), c_int, POINTER(c_char_p), POINTER(c_char_p)]
GetQuotes.restype = None

# 多账户批量五档。各数组长度均为 Count。
GetMultiAccountsQuotes = dll.GetMultiAccountsQuotes
GetMultiAccountsQuotes.argtypes = [POINTER(c_int), POINTER(c_char_p), c_int, POINTER(c_char_p), POINTER(c_char_p)]
GetMultiAccountsQuotes.restype = None

# 融资融券直接还款。Amount 为金额字符串。
Repay = dll.Repay
Repay.argtypes = [c_int, c_char_p, c_char_p, c_char_p]
Repay.restype = None

# 查询可买/可卖数量。成功 >=0，失败为负数。Category、PriceType 同下单。
GetCanBuySell = dll.GetCanBuySell
GetCanBuySell.argtypes = [c_int, c_int, c_int, c_char_p, c_char_p, c_float, c_char_p, c_char_p]
GetCanBuySell.restype = c_int

# 查询可交易数量。返回值优先取可买/可卖数量列。
GetTradableQuantity = dll.GetTradableQuantity
GetTradableQuantity.argtypes = [c_int, c_int, c_int, c_char_p, c_char_p, c_float, c_char_p, c_char_p]
GetTradableQuantity.restype = c_int

# 非 0 时按列序重排 Result，默认关闭。
EnableResultColumnOrder = dll.EnableResultColumnOrder
EnableResultColumnOrder.argtypes = [c_int]
EnableResultColumnOrder.restype = None

# 非 0 时表头附带字段 ID 后缀，默认关闭。
EnableResultColumnIdSuffix = dll.EnableResultColumnIdSuffix
EnableResultColumnIdSuffix.argtypes = [c_int]
EnableResultColumnIdSuffix.restype = None

# 查询股东代码。先查 Category 5，无数据时回落登录缓存。
GetShareholderCodes = dll.GetShareholderCodes
GetShareholderCodes.argtypes = [c_int, c_char_p, c_char_p]
GetShareholderCodes.restype = None

# 一键打新。DryRun 非 0 只出计划不下单。失败返回 -1。
OneClickIpo = dll.OneClickIpo
OneClickIpo.argtypes = [c_int, c_int, c_char_p, c_char_p]
OneClickIpo.restype = c_int

# 场内基金 / ETF。Category: 0场内申购(金额) 1场内赎回(份额) 2ETF申购(份额) 3ETF赎回(份额)。
# ExchangeID: 0深圳 1上海 6北交所。Amount 为金额或份额字符串。
FundOrder = dll.FundOrder
FundOrder.argtypes = [c_int, c_int, c_char_p, c_char_p, c_int, c_char_p, c_char_p, c_char_p]
FundOrder.restype = None


def text(buf):
    return buf.value.decode("gbk", errors="replace")


def main():
    result = create_string_buffer(1024 * 1024)
    err_info = create_string_buffer(256)

    Init()

    # Config 只写账户类型。
    client_id = Logon(
        gbk("YOUR_BROKER_IP"), 0, gbk('{"account_type":0}'), 0,
        gbk("YOUR_ACCOUNT"), gbk("YOUR_ACCOUNT"), gbk("YOUR_PASSWORD"), gbk(""), err_info)
    print("Logon ClientID=%s %s" % (client_id, text(err_info)))

    if client_id >= 0:
        QueryData(client_id, 0, result, err_info)
        print("QueryData funds:")
        print(text(result))
        print(text(err_info))

        # SendOrder(client_id, 0, 0, gbk("股东代码"), gbk("证券代码"), 0.0, 100, result, err_info)
        # CancelOrder(client_id, gbk("1"), gbk("委托编号"), result, err_info)
        # Repay(client_id, gbk("0"), result, err_info)

        Logoff(client_id)

    UnInit()


if __name__ == "__main__":
    main()
