# coding=utf-8
# Api.dll 行情调用示例。进程位数须与 DLL 一致（x86 或 x64）。调用约定是 stdcall。
# Result、ErrInfo 为 GBK。Result 多为 TSV：行以 \n 分隔，列以 \t 分隔。
# ErrInfo 建议 >= 256 字节，Result 建议 >= 1MB。
# 连接返回 >=0 为 ConnectionID，失败返回 -1。其余接口 true 成功。
# Market：0 深圳  1 上海  2 北交所。扩展行情的市场号以 TdxExHq_GetMarkets 为准。
# K 线 Category：0~3 为 5/15/30/60 分钟，4 日，5 周，6 月，7 起为 1 分钟及扩展周期。
# A 股常见端口 7709，L2 常见 7719。扩展行情常见 7727 / 7721。

from ctypes import (
    WinDLL, c_int, c_ushort, c_short, c_byte, c_char_p, c_bool, POINTER,
    create_string_buffer,
)

dll = WinDLL(".\\Api.dll")


def gbk(text):
    return text.encode("gbk")


# 连接 A 股行情。账号由授权方提供。
TdxHq_Connect = dll.TdxHq_Connect
TdxHq_Connect.argtypes = [c_char_p, c_ushort, c_char_p, c_char_p, c_char_p, c_char_p]
TdxHq_Connect.restype = c_int

# 断开 A 股连接。
TdxHq_Disconnect = dll.TdxHq_Disconnect
TdxHq_Disconnect.argtypes = [c_int]
TdxHq_Disconnect.restype = None

# 市场证券数量。Result 是数量，不是字符串。
TdxHq_GetSecurityCount = dll.TdxHq_GetSecurityCount
TdxHq_GetSecurityCount.argtypes = [c_int, c_byte, POINTER(c_ushort), c_char_p]
TdxHq_GetSecurityCount.restype = c_bool

# 证券列表。Count 入为请求条数，出为实际条数。
TdxHq_GetSecurityList = dll.TdxHq_GetSecurityList
TdxHq_GetSecurityList.argtypes = [c_int, c_byte, c_ushort, POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetSecurityList.restype = c_bool

# 个股 K 线。指数请用 TdxHq_GetIndexBars。
TdxHq_GetSecurityBars = dll.TdxHq_GetSecurityBars
TdxHq_GetSecurityBars.argtypes = [c_int, c_byte, c_byte, c_char_p, c_ushort, POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetSecurityBars.restype = c_bool

# 指数 K 线，含涨跌家数。
TdxHq_GetIndexBars = dll.TdxHq_GetIndexBars
TdxHq_GetIndexBars.argtypes = [c_int, c_byte, c_byte, c_char_p, c_ushort, POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetIndexBars.restype = c_bool

# 当日分时。
TdxHq_GetMinuteTimeData = dll.TdxHq_GetMinuteTimeData
TdxHq_GetMinuteTimeData.argtypes = [c_int, c_byte, c_char_p, c_char_p, c_char_p]
TdxHq_GetMinuteTimeData.restype = c_bool

# 历史分时。Date 为 yyyyMMdd。
TdxHq_GetHistoryMinuteTimeData = dll.TdxHq_GetHistoryMinuteTimeData
TdxHq_GetHistoryMinuteTimeData.argtypes = [c_int, c_byte, c_char_p, c_int, c_char_p, c_char_p]
TdxHq_GetHistoryMinuteTimeData.restype = c_bool

# 分笔成交。
TdxHq_GetTransactionData = dll.TdxHq_GetTransactionData
TdxHq_GetTransactionData.argtypes = [c_int, c_byte, c_char_p, c_ushort, POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetTransactionData.restype = c_bool

# 历史分笔。
TdxHq_GetHistoryTransactionData = dll.TdxHq_GetHistoryTransactionData
TdxHq_GetHistoryTransactionData.argtypes = [c_int, c_byte, c_char_p, c_ushort, POINTER(c_ushort), c_int, c_char_p, c_char_p]
TdxHq_GetHistoryTransactionData.restype = c_bool

# 五档行情。Market 与 Zqdm 等长，Count 入出参。
TdxHq_GetSecurityQuotes = dll.TdxHq_GetSecurityQuotes
TdxHq_GetSecurityQuotes.argtypes = [c_int, POINTER(c_byte), POINTER(c_char_p), POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetSecurityQuotes.restype = c_bool

# 公司信息目录。
TdxHq_GetCompanyInfoCategory = dll.TdxHq_GetCompanyInfoCategory
TdxHq_GetCompanyInfoCategory.argtypes = [c_int, c_byte, c_char_p, c_char_p, c_char_p]
TdxHq_GetCompanyInfoCategory.restype = c_bool

# 公司信息正文。
TdxHq_GetCompanyInfoContent = dll.TdxHq_GetCompanyInfoContent
TdxHq_GetCompanyInfoContent.argtypes = [c_int, c_byte, c_char_p, c_char_p, c_int, c_int, c_char_p, c_char_p]
TdxHq_GetCompanyInfoContent.restype = c_bool

# 除权除息。
TdxHq_GetXDXRInfo = dll.TdxHq_GetXDXRInfo
TdxHq_GetXDXRInfo.argtypes = [c_int, POINTER(c_byte), POINTER(c_char_p), c_int, c_char_p, c_char_p]
TdxHq_GetXDXRInfo.restype = c_bool

# 财务简表。
TdxHq_GetFinanceInfo = dll.TdxHq_GetFinanceInfo
TdxHq_GetFinanceInfo.argtypes = [c_int, POINTER(c_byte), POINTER(c_char_p), c_int, c_char_p, c_char_p]
TdxHq_GetFinanceInfo.restype = c_bool

# 集合竞价。
TdxHq_GetCallAuctionData = dll.TdxHq_GetCallAuctionData
TdxHq_GetCallAuctionData.argtypes = [c_int, c_byte, c_char_p, c_int, POINTER(c_int), c_char_p, c_char_p]
TdxHq_GetCallAuctionData.restype = c_bool

# 十档行情。需要 L2。
TdxHq_GetSecurityQuotes10 = dll.TdxHq_GetSecurityQuotes10
TdxHq_GetSecurityQuotes10.argtypes = [c_int, POINTER(c_byte), POINTER(c_char_p), POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetSecurityQuotes10.restype = c_bool

# 买卖队列。需要 L2。
TdxHq_GetBuySellQueue = dll.TdxHq_GetBuySellQueue
TdxHq_GetBuySellQueue.argtypes = [c_int, POINTER(c_byte), POINTER(c_char_p), c_int, c_char_p, c_char_p]
TdxHq_GetBuySellQueue.restype = c_bool

# 逐笔成交。需要 L2。
TdxHq_GetDetailTransactionData = dll.TdxHq_GetDetailTransactionData
TdxHq_GetDetailTransactionData.argtypes = [c_int, c_byte, c_char_p, c_int, POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetDetailTransactionData.restype = c_bool

# 逐笔成交原始数据。需要 L2。
TdxHq_GetDetailOriginalTransactionData = dll.TdxHq_GetDetailOriginalTransactionData
TdxHq_GetDetailOriginalTransactionData.argtypes = [c_int, c_byte, c_char_p, c_int, POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetDetailOriginalTransactionData.restype = c_bool

# 逐笔委托。需要 L2。
TdxHq_GetDetailOrderData = dll.TdxHq_GetDetailOrderData
TdxHq_GetDetailOrderData.argtypes = [c_int, c_byte, c_char_p, c_int, POINTER(c_ushort), c_char_p, c_char_p]
TdxHq_GetDetailOrderData.restype = c_bool

# 板块文件。BlockFile 空则默认概念板块。名称是 GBK。
TdxHq_GetBlockInfo = dll.TdxHq_GetBlockInfo
TdxHq_GetBlockInfo.argtypes = [c_int, c_char_p, c_char_p, c_char_p]
TdxHq_GetBlockInfo.restype = c_bool

# 下载报表文件到 SavePath。
TdxHq_GetReportFile = dll.TdxHq_GetReportFile
TdxHq_GetReportFile.argtypes = [c_int, c_char_p, c_char_p, c_char_p, c_char_p]
TdxHq_GetReportFile.restype = c_bool

# 板块列表。BoardType: concept / style / industry / index。
TdxHq_ListBoards = dll.TdxHq_ListBoards
TdxHq_ListBoards.argtypes = [c_int, c_char_p, c_char_p, c_char_p]
TdxHq_ListBoards.restype = c_bool

# 板块成分。BoardName 为 GBK。
TdxHq_ListBoardMembers = dll.TdxHq_ListBoardMembers
TdxHq_ListBoardMembers.argtypes = [c_int, c_char_p, c_char_p, c_char_p, c_char_p]
TdxHq_ListBoardMembers.restype = c_bool

# 全市场涨跌统计。
TdxHq_GetMarketStat = dll.TdxHq_GetMarketStat
TdxHq_GetMarketStat.argtypes = [c_int, c_char_p, c_char_p]
TdxHq_GetMarketStat.restype = c_bool

# 当日资金流向。
TdxHq_GetFundFlow = dll.TdxHq_GetFundFlow
TdxHq_GetFundFlow.argtypes = [c_int, c_byte, c_char_p, c_char_p, c_char_p]
TdxHq_GetFundFlow.restype = c_bool

# 历史资金流向。
TdxHq_GetHistoryFundFlow = dll.TdxHq_GetHistoryFundFlow
TdxHq_GetHistoryFundFlow.argtypes = [c_int, c_byte, c_char_p, c_ushort, c_ushort, c_char_p, c_char_p]
TdxHq_GetHistoryFundFlow.restype = c_bool

# 从本机通达信目录读取站点。Channel：1 L1，2 L2，3 扩展。
TdxHq_LoadHostCfg = dll.TdxHq_LoadHostCfg
TdxHq_LoadHostCfg.argtypes = [c_int, c_char_p, c_char_p, c_char_p]
TdxHq_LoadHostCfg.restype = c_bool

# 连接扩展行情。与 A 股 ConnectionID 不要混用。
TdxExHq_Connect = dll.TdxExHq_Connect
TdxExHq_Connect.argtypes = [c_char_p, c_int, c_char_p, c_char_p, c_char_p, c_char_p]
TdxExHq_Connect.restype = c_int

# 断开扩展行情。
TdxExHq_Disconnect = dll.TdxExHq_Disconnect
TdxExHq_Disconnect.argtypes = [c_int]
TdxExHq_Disconnect.restype = None

# 扩展市场列表。
TdxExHq_GetMarkets = dll.TdxExHq_GetMarkets
TdxExHq_GetMarkets.argtypes = [c_int, c_char_p, c_char_p]
TdxExHq_GetMarkets.restype = c_bool

# 扩展品种数量。Result 是数量。
TdxExHq_GetInstrumentCount = dll.TdxExHq_GetInstrumentCount
TdxExHq_GetInstrumentCount.argtypes = [c_int, POINTER(c_int), c_char_p]
TdxExHq_GetInstrumentCount.restype = c_bool

# 扩展品种信息。
TdxExHq_GetInstrumentInfo = dll.TdxExHq_GetInstrumentInfo
TdxExHq_GetInstrumentInfo.argtypes = [c_int, c_int, c_short, c_char_p, c_char_p]
TdxExHq_GetInstrumentInfo.restype = c_bool

# 扩展 K 线。
TdxExHq_GetInstrumentBars = dll.TdxExHq_GetInstrumentBars
TdxExHq_GetInstrumentBars.argtypes = [c_int, c_byte, c_byte, c_char_p, c_int, POINTER(c_short), c_char_p, c_char_p]
TdxExHq_GetInstrumentBars.restype = c_bool

# 扩展当日分时。
TdxExHq_GetMinuteTimeData = dll.TdxExHq_GetMinuteTimeData
TdxExHq_GetMinuteTimeData.argtypes = [c_int, c_byte, c_char_p, c_char_p, c_char_p]
TdxExHq_GetMinuteTimeData.restype = c_bool

# 扩展分笔。
TdxExHq_GetTransactionData = dll.TdxExHq_GetTransactionData
TdxExHq_GetTransactionData.argtypes = [c_int, c_byte, c_char_p, c_int, POINTER(c_short), c_char_p, c_char_p]
TdxExHq_GetTransactionData.restype = c_bool

# 扩展单品种行情。
TdxExHq_GetInstrumentQuote = dll.TdxExHq_GetInstrumentQuote
TdxExHq_GetInstrumentQuote.argtypes = [c_int, c_byte, c_char_p, c_char_p, c_char_p]
TdxExHq_GetInstrumentQuote.restype = c_bool

# 扩展历史分笔。
TdxExHq_GetHistoryTransactionData = dll.TdxExHq_GetHistoryTransactionData
TdxExHq_GetHistoryTransactionData.argtypes = [c_int, c_byte, c_char_p, c_int, c_int, POINTER(c_short), c_char_p, c_char_p]
TdxExHq_GetHistoryTransactionData.restype = c_bool

# 扩展历史分时。
TdxExHq_GetHistoryMinuteTimeData = dll.TdxExHq_GetHistoryMinuteTimeData
TdxExHq_GetHistoryMinuteTimeData.argtypes = [c_int, c_byte, c_char_p, c_int, c_char_p, c_char_p]
TdxExHq_GetHistoryMinuteTimeData.restype = c_bool


def text(buf):
    return buf.value.decode("gbk", errors="replace")


def main():
    result = create_string_buffer(1024 * 1024)
    err_info = create_string_buffer(256)

    connection_id = TdxHq_Connect(
        gbk("YOUR_HQ_HOST"), 7709, gbk("YOUR_ACCOUNT"), gbk("YOUR_PASSWORD"), result, err_info)
    print("TdxHq_Connect %s %s %s" % (connection_id, text(result), text(err_info)))
    if connection_id >= 0:
        market = (c_byte * 1)(0)
        zqdm = (c_char_p * 1)(gbk("000001"))
        count = c_ushort(1)
        TdxHq_GetSecurityQuotes(connection_id, market, zqdm, count, result, err_info)
        print("TdxHq_GetSecurityQuotes")
        print(text(result))
        print(text(err_info))

        # n = c_ushort(100)
        # TdxHq_GetSecurityBars(connection_id, 4, 0, gbk("000001"), 0, n, result, err_info)
        # TdxHq_GetMinuteTimeData(connection_id, 0, gbk("000001"), result, err_info)

        TdxHq_Disconnect(connection_id)


if __name__ == "__main__":
    main()
