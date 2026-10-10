# stockapi

面向 A 股的**交易 + 行情**接口方案。QMT 无法使用时，可用同一 `Api.dll` 对接券商交易，并接入通达信行情栈（含 **L2**：十档、逐笔成交、逐笔委托）。支持大部分券商。

本仓库公开的是**调用示例**（C++ / C# / Python / Java），运行时加载 `Api.dll`。DLL 本体与 L2 授权不在仓库内；接入与授权请咨询 **Telegram**：`JyApi888`。最新说明以本仓库主页为准。

本仓库上传的代码放弃版权，可自由使用，详见 [LICENSE](LICENSE)。`Api.dll` 不在此许可范围内。

## 接口概览

| 类别 | 说明 |
| --- | --- |
| **通达信交易接口** | 运行时初始化与释放（`Init` / `UnInit`） |
| **同花顺交易接口** | 登录、查询、下单、撤单、打新、场内基金等；`Result` 为 GBK TSV |
| **通达信行情接口** | `TdxHq_*` A 股行情、`TdxExHq_*` 扩展行情；L2 主机常见端口 `7719`，L1 常见 `7709` |
| **同花顺行情接口** | 交易会话上的证券**五档快照**（`GetQuote` 等），与交易共用 `ClientID` |

进程位数须与 `Api.dll` 一致（x86 或 x64）。除连接类接口外，成功返回多为 `true` 或 `0`；`Result`、`ErrInfo` 为 **GBK** 文本，`Result` 行以 `\n` 分隔、列以 `\t` 分隔。

---

## 通达信交易接口

在调用同花顺交易函数前，先初始化运行时；全部会话结束后再释放。

| API | 用途 |
| --- | --- |
| `Init` | 初始化交易运行时，进程内调用一次，须在 `Logon` 之前 |
| `UnInit` | 释放全部会话并关闭运行时；调用后不可再使用此前的 `ClientID` |

---

## 同花顺交易接口

典型流程：`Init` → `Logon` → 查询 / 下单 → `Logoff` → `UnInit`。

`Logon` 的 `Config` 只填账户类型，例如 `{"account_type":0}`：`0` 资金帐户，`1` 深圳，`2` 上海，`3` 基金，`4` 深圳Ｂ股，`5` 上海Ｂ股，`k` 客户号。

### 会话与查询

| API | 用途 |
| --- | --- |
| `Logon` | 登录券商；成功返回 `ClientID`（≥0），失败 -1 |
| `Logoff` | 注销指定 `ClientID` |
| `QueryData` | 当日数据：资金、持仓、委托、成交、可撤单、股东代码、新股等 |
| `QueryHistoryData` | 历史委托、历史成交、交割单、资金明细、对账单（日期 `yyyyMMdd`） |
| `QueryDatas` | 同一账户、多 `Category` 批量查询 |
| `QueryMultiAccountsDatas` | 多账户批量查询 |

### 下单与撤单

| API | 用途 |
| --- | --- |
| `SendOrder` | 下单（买卖、融资融券、新股申购等） |
| `SendOrders` | 同一账户批量下单 |
| `SendMultiAccountsOrders` | 多账户批量下单 |
| `CancelOrder` | 撤单（交易所 + 委托编号） |
| `CancelOrders` | 同一账户批量撤单 |
| `CancelMultiAccountsOrders` | 多账户批量撤单 |

### 其它

| API | 用途 |
| --- | --- |
| `GetTradableQuantity` | 可交易数量（可买 / 可卖） |
| `GetCanBuySell` | 可买可卖数量查询 |
| `GetShareholderCodes` | 股东代码 |
| `Repay` | 融资融券直接还款 |
| `OneClickIpo` | 一键打新 |
| `FundOrder` | 场内基金 / ETF 申购赎回 |
| `EnableResultColumnOrder` | 按列序重排 `Result`（可选） |
| `EnableResultColumnIdSuffix` | 表头附带字段 ID 后缀（可选） |

---

## 同花顺行情接口

通过**已登录**的交易连接取证券五档快照，无需单独行情 `ConnectionID`。

| API | 用途 |
| --- | --- |
| `GetQuote` | 单只证券五档行情（代码可带 `sz` / `sh` / `bj` 前缀） |
| `GetQuotes` | 同一账户批量五档 |
| `GetMultiAccountsQuotes` | 多账户批量五档 |

---

## 通达信行情接口

A 股：`TdxHq_Connect` → 各类查询 → `TdxHq_Disconnect`。扩展行情使用 `TdxExHq_*`，**连接号不要与 A 股混用**。市场：`0` 深圳，`1` 上海，`2` 北交所。

### 连接

| API | 用途 |
| --- | --- |
| `TdxHq_Connect` | 连接 A 股行情服务器 |
| `TdxHq_Disconnect` | 断开 A 股连接 |
| `TdxHq_LoadHostCfg` | 从本机通达信目录读取站点（Channel：1 L1，2 L2，3 扩展） |

### 列表与 K 线、分时、分笔

| API | 用途 |
| --- | --- |
| `TdxHq_GetSecurityCount` | 指定市场证券数量 |
| `TdxHq_GetSecurityList` | 证券列表 |
| `TdxHq_GetSecurityBars` | 个股 / 基金 K 线 |
| `TdxHq_GetIndexBars` | 指数 K 线（含涨跌家数） |
| `TdxHq_GetMinuteTimeData` | 当日分时 |
| `TdxHq_GetHistoryMinuteTimeData` | 历史分时 |
| `TdxHq_GetTransactionData` | 当日分笔成交 |
| `TdxHq_GetHistoryTransactionData` | 历史分笔成交 |

### 快照、资料、板块、资金

| API | 用途 |
| --- | --- |
| `TdxHq_GetSecurityQuotes` | 五档行情（批量，每包最多 80 只） |
| `TdxHq_GetCompanyInfoCategory` | 公司信息目录 |
| `TdxHq_GetCompanyInfoContent` | 公司信息正文 |
| `TdxHq_GetXDXRInfo` | 除权除息 |
| `TdxHq_GetFinanceInfo` | 财务简表 |
| `TdxHq_GetCallAuctionData` | 集合竞价 |
| `TdxHq_GetBlockInfo` | 板块文件（概念等） |
| `TdxHq_ListBoards` | 板块列表 |
| `TdxHq_ListBoardMembers` | 板块成分 |
| `TdxHq_GetMarketStat` | 全市场涨跌统计 |
| `TdxHq_GetFundFlow` | 当日资金流向 |
| `TdxHq_GetHistoryFundFlow` | 历史资金流向 |
| `TdxHq_GetReportFile` | 下载报表文件到本地 |

### L2（重点）

连接 **L2 主机**（常见端口 `7719`，需授权）后可使用：

| API | 用途 |
| --- | --- |
| `TdxHq_GetSecurityQuotes10` | 十档盘口 |
| `TdxHq_GetBuySellQueue` | 买卖队列（价、量、笔数、队列） |
| `TdxHq_GetDetailTransactionData` | 逐笔成交明细 |
| `TdxHq_GetDetailOriginalTransactionData` | 逐笔成交（含买卖方委托号） |
| `TdxHq_GetDetailOrderData` | 逐笔委托 |

### 扩展行情 `TdxExHq_*`

| API | 用途 |
| --- | --- |
| `TdxExHq_Connect` | 连接扩展行情（常见端口 `7727` / `7721`） |
| `TdxExHq_Disconnect` | 断开扩展行情 |
| `TdxExHq_GetMarkets` | 扩展市场列表 |
| `TdxExHq_GetInstrumentCount` | 合约数量 |
| `TdxExHq_GetInstrumentInfo` | 合约信息 |
| `TdxExHq_GetInstrumentBars` | 合约 K 线 |
| `TdxExHq_GetMinuteTimeData` | 当日分时 |
| `TdxExHq_GetHistoryMinuteTimeData` | 历史分时 |
| `TdxExHq_GetTransactionData` | 分笔成交 |
| `TdxExHq_GetHistoryTransactionData` | 历史分笔 |
| `TdxExHq_GetInstrumentQuote` | 合约快照行情 |

更细的参数与 `Result` 列说明见 [HQ_API.md](HQ_API.md)。

---

## 调用示例

**交易**（目录 `交易/`）：

- `交易/cpp/Demo.cpp`
- `交易/csharp/Demo.cs`
- `交易/python/demo.py`
- `交易/java/Demo.java`

**行情**（目录 `行情/`）：

- `行情/cpp/Demo.cpp`
- `行情/csharp/Demo.cs`
- `行情/python/demo.py`
- `行情/java/Demo.java`

示例中主机、账号、密码均为占位符，请替换为授权方提供的正式参数。
