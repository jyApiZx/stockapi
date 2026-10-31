# 同花顺交易接口  |ths  trade接口 | ths auto trade交易接口 | 通达信交易接口| 通达信auto trade交易即可

**同花顺交易接口，L1 与扩展行情接口。直连券商，无需启动客户端，支持所有券商。产品介绍见 [thsapi.com](https://thsapi.com/)。技术支持请联系 [Telegram：@Thsapi](https://t.me/Thsapi)**

交易库是 `thsapi.dll`，行情库是 `HqApi.dll`，都有 32 位和 64 位。Python、Java、C#、C/C++ 都可以接。下面按使用顺序说明授权、安全和安装。调用参数见后文，也可以看 [thsapi.com](https://thsapi.com/)。

## 下载试用

交易测试包 1.0.6.2，里面是 `thsapi.dll`、`xdDemo.exe` 和券商站点列表：[64 位](https://github.com/jyApiZx/stockapi/releases/download/v1.0.6.2/thsapi_1.0.6.2_x64.zip)、[32 位](https://github.com/jyApiZx/stockapi/releases/download/v1.0.6.2/thsapi_1.0.6.2_x32.zip)。

行情测试包 1.0.7.0，里面是 `HqApi.dll` 和行情测试程序：[64 位](https://github.com/jyApiZx/stockapi/releases/download/hqapi-1.0.7.0/hqapi_1.0.7.0_x64.zip)、[32 位](https://github.com/jyApiZx/stockapi/releases/download/hqapi-1.0.7.0/hqapi_1.0.7.0_x32.zip)。授权含 L1 与扩展行情。

初次使用，如果您对产品还有疑虑，可以用同花顺模拟账户测试。登录账号填手机号、同花顺账户或股东账户，三选一，资金账号留空。[模拟账户怎么填](https://thsapi.com/sim.html)，仓库说明见 [模拟账户.md](模拟账户.md)。

## 调用示例

交易：

- [C++](交易/cpp/Demo.cpp)
- [C#](交易/csharp/Demo.cs)
- [Python](交易/python/demo.py)
- [Java](交易/java/Demo.java)

行情：

- [C++](行情/cpp/Demo.cpp)
- [C#](行情/csharp/Demo.cs)
- [Python](行情/python/demo.py)
- [Java](行情/java/Demo.java)

## 重要提醒

这是给有编程基础、要把已有策略接到券商上的接口。还在找买卖点、没有准备写程序或交给 AI 改程序的，这套接口帮不上。

授权是先付款、后开通。付款后把记录发给 [Telegram：@Thsapi](https://t.me/Thsapi)，按账户开通。服务时间周一至周六 9:00–20:00，其他时间可以在网页在线咨询里先留言。技术细节以本文为准，价格以 [官网授权方式](https://thsapi.com/#license) 为准。

## 一、这是什么接口

把交易和行情调用放进自己的程序：登录后查询资金、持仓、委托和成交，再下单、撤单。成交发生时，登录后注册 `SetPushCallback`，分笔成交会逐条回调。行情仍由程序主动查询。

### （一）产品功能

- 直连券商交易服务器，中间不经过客户端。支持普通账户和融资融券账户的下单、撤单、查询和成交回报，以及融资买入、融券卖出、担保品买卖、买券还券、卖券还款、直接还款和新股申购。
- 可以单账户操作，也可以多个账户同时保持连接。同一账户有批量查询、批量下单、批量撤单；多个账户有 `QueryMultiAccountsDatas`、`SendMultiAccountsOrders`、`CancelMultiAccountsOrders`。同一个 `ClientID` 请串行调用，不同账户可以并行。
- 交易连接有心跳，断线会重连。下单耗时主要取决于到券商的网络和券商限速。64 位大约比 32 位快 30%。
- 交易连接上可以直接取五档快照：`GetQuote`、`GetQuotes`、`GetMultiAccountsQuotes`。独立行情库 `HqApi.dll` 提供 L1 与扩展行情。L1 是实时行情、Tick、五档和分钟 K 线；扩展行情是期货、期权等（`TdxExHq_*`）。
- 导出函数是 `stdcall`。C++、C#、Python、Java 有现成示例；Delphi、VB、易语言按同样的函数名调用即可。
- 信用账户登录时，在普通资金账号开头加 `X`，交易账号仍用信用资金账号。
- 已有项目时，可以把 [AI接入提示词.md](AI接入提示词.md) 交给正在改项目的 AI，或打开 [AI 代做提示词](https://thsapi.com/ai.html)。

### （二）授权

授权按账户开通，不需要提供交易密码。

网络授权只在登录授权时发送经过哈希处理的账号标识。授权服务器只保存这个哈希值和使用时间，不接收交易密码、持仓、委托或成交。校验通过后不再连接授权服务器，也不下发 `thsapi.lic`，程序只连接券商交易站点或行情站点。

本地授权是把 `thsapi.lic` 直接放在你的电脑上，DLL 读取本地文件完成授权，不连接授权服务器。运行期间除券商交易或行情网络外，不连接其他服务。

本地授权的 `thsapi.lic` 文件名和内容不可修改，否则无法登录。

普通账户和信用账户是两个账户，各自授权。行情授权含 L1 与扩展行情。

### （三）用户安全

DLL 只在你的程序调用时工作，读不到策略源码。证券账户的银证转账受券商和银行的三方存管规则限制，资金只能在证券账户和已绑定的本人同名银行账户之间划转，不能直接转给第三方。这只说明资金去向。程序仍会执行你发出的下单指令，交易密码请自己保管，并先用模拟账户把流程跑通。

可以自己核对，不必只看说明：

- 用资源监视器或 `netstat` 看这个进程连了哪些地址。
- 在 Windows 防火墙里限制程序能访问的地址，只放行券商交易或行情站点。网络授权只在登录时校验一次，不会一直连着授权服务器；本地授权本来就不连。
- 这些检查都可以交给 AI：让它跑模拟账户、列出网络连接、写防火墙出站规则。
- 先在单独目录里用模拟账户登录、查询、下单和撤单，再接到正式策略上。

完整说明见 [账户安全与授权](https://thsapi.com/security.html)。请使用正版系统和正常安装的安全软件。破解版软件有被植入后门的风险，由此造成的损失需要自行承担。

### （四）接口性能

一次下单包含打包、发送、接收和解包。发送和接收的时间取决于你的网络和券商处理速度，接口改不了这段网络时间。同一账户的调用请串行。批量接口可以一次提交多笔，实际速度以券商限制为准，使用前请向券商确认允许的频率。

下面是单次调用从发出到返回的往返时间，不是平均值。交易在同花顺模拟站点实测，行情在 L1 站点实测。不同网络环境、不同券商站点，速度会有差异。建议下载上面的测试包，在自己的网络里再测一次。

交易接口 `thsapi.dll`：下单 `SendOrder` 45 毫秒。查询资金 `QueryData(0)` 38 毫秒，查询股份 `QueryData(1)` 40 毫秒，查询当日委托 `QueryData(2)` 36 毫秒，查询股东代码 `QueryData(5)` 42 毫秒。直连券商交易服务器，中间没有客户端和中转。券商不限速时，批量下单约 200 单/秒。成交回报由券商主动推送，不用轮询。

行情接口 `HqApi.dll`：单只五档 `GetSecurityQuotes` 33 毫秒，K 线 `GetSecurityBars` 32 毫秒，分时 `GetMinuteTimeData` 33 毫秒，集合竞价 `GetCallAuctionData` 35 毫秒，板块 `ListBoards` 41 毫秒，F10 `GetCompanyInfoContent` 48 毫秒。真实环境下，一次调用批量函数拉取全市场五档，5224 只全部返回，含数据解码，185 毫秒取回。授权含 L1 与扩展行情。

#### 1. 行情

行情都是程序主动查询。`HqApi.dll` 连接有心跳，断了会重连。

- L1：端口 `7709`，账号写成 `L1*账号`，密码用该账号的密码，不能为空。
- 扩展行情：常见端口 `7727` / `7721`，使用 `TdxExHq_*`，连接号不要与 A 股混用。
- 普通五档可以按主机规则多点连接。

#### 2. 交易

`thsapi.dll` 的调用顺序是 `Init`、`Logon`、业务函数、`Logoff`、`UnInit`。登录后可以查询资金、持仓、当日委托、当日成交、可撤单和股东代码，也可以查历史委托、历史成交、交割单和资金明细。

下单用 `SendOrder`，撤单用 `CancelOrder`。需要一次提交多笔时，用 `SendOrders`、`CancelOrders`；多个账户一起提交时，用 `SendMultiAccountsOrders`、`CancelMultiAccountsOrders`。批量调用不用等上一笔返回后再发下一笔，实际成交速度仍受券商限速约束。可买可卖用 `GetTradableQuantity` 或 `GetCanBuySell`，打新用 `OneClickIpo`，场内基金和 ETF 用 `FundOrder`，信用账户直接还款用 `Repay`。

通讯密码多数券商填空字符串，少数券商要填。先在 `xdDemo.exe` 里用同一组参数登录成功，再写进程序。模拟站点是 `trade.10jqka.com.cn`，端口 `8002`，营业部 `48`，`qs_id` 为 `356`。登录账号填手机号、同花顺账户或股东账户，三选一，资金账号留空。

## 二、授权价格

价格不写在这里，以 [官网授权方式](https://thsapi.com/#license) 为准。交易接口和行情接口都适用。付款后把记录发给 [Telegram：@Thsapi](https://t.me/Thsapi)。

网络授权可以更换账户。本地授权对应具体账户，更换资金账号后需要重新开通。行情授权含 L1 与扩展行情。Linux `.so` 和其他功能单独约定。联系 [Telegram：@Thsapi](https://t.me/Thsapi)。

## 三、安装调试

### （一）运行环境

- Windows 10、Windows 11，或同等的 Windows Server。32 位和 64 位都可以，程序位数必须和 DLL 一致。优先使用 64 位。
- Python、Java、C#、C/C++ 按对应位数调用。
- `xdDemo.exe` 需要 .NET Framework 4.8，Windows 10/11 通常已经带有。
- Linux 不在标准包内，`.so` 单独定制。

### （二）获取软件

试用包下载在上面的 [下载试用](#下载试用)。技术支持和开通联系 [Telegram：@Thsapi](https://t.me/Thsapi)。

四个语言的调用代码在 [调用示例](#调用示例)。DLL 在测试包里，不在 git 仓库中。

### （三）先用模拟盘

初次使用可以先用同花顺模拟账户。登录账号填手机号、同花顺账户或股东账户，三选一，资金账号留空。通讯密码留空。先打开 `xdDemo.exe` 登录成功，再写进自己的程序。填法看 [模拟账户.md](模拟账户.md)。

## 四、服务支持

### （一）服务承诺

券商协议会变化。能继续拿到相应协议时，会更新接口，保证已开通的账户可以继续使用。交易接口本身不能消除下单错误和市场风险。正式使用前，请先用模拟账户把查询、下单和撤单跑通。

### （二）服务范围

技术支持只覆盖 `thsapi.dll`、`HqApi.dll` 和随包的演示程序，包括登录、调用参数和接口返回的错误。不负责调试你的策略逻辑，也不把远程桌面代装作为常规服务。接口自身导致的登录或调用故障，通过 Telegram 处理。

### （三）服务渠道

稳定联系方式是 [Telegram：@Thsapi](https://t.me/Thsapi)。没有 Telegram 时，打开 [thsapi.com](https://thsapi.com/) 使用在线咨询。

### （四）服务时间

技术咨询、支持服务时间：周一至周六 9:00–20:00。授权购买时间：周一至周六 10:00–20:00，周日 19:00–21:00。需要在开盘前开通授权的，请提前一天晚上通过 Telegram 发送付款记录和资金账户号。

## 五、常见问题

**找不到 DLL。** 把 `thsapi.dll` 或 `HqApi.dll` 放在程序目录，文件名保持小写，并确认 32/64 位和程序一致。`xdDemo.exe`、`ths.xml`、`thsapi.dll` 要放在同一目录。

**登录返回 -1。** 看 `ErrInfo`。先核对接的地址、端口、账号和密码。模拟盘用 `trade.10jqka.com.cn:8002`，`{"qs_id":356,"account_type":0}`，营业部 `48`。同一组参数先在 `xdDemo.exe` 里登录。

**Config 要写满吗。** 不用。只写 `qs_id` 和 `account_type`，空项不要写进去。

**授权服务器会看到密码和持仓吗。** 不会。网络授权只上传账号哈希，服务端只保存哈希值和使用时间。本地授权不连接授权服务器。

**资金能转到别人的账户吗。** 银证转账只能在证券账户和已绑定的本人同名银行账户之间进行。这不能代替你自己保管密码。

**行情账号怎么填。** L1 写成 `L1*账号`。扩展行情用 `TdxExHq_*`。

**返回乱码。** `Result` 和 `ErrInfo` 是 GBK。`Result` 以换行分行、以制表符分列。

---

## 接口功能概览

| 类别 | 说明 |
| --- | --- |
| **同花顺交易接口** | 登录、查询、下单、撤单、打新、场内基金等；`Result` 为 GBK TSV |
| **通达信行情接口** | `TdxHq_*` A 股 L1 行情、`TdxExHq_*` 扩展行情；L1 常见端口 `7709`，扩展常见 `7727` / `7721` |
| **同花顺行情接口** | 交易会话上的证券**五档快照**（`GetQuote` 等），与交易共用 `ClientID` |

进程位数须与 `thsapi.dll`、`HqApi.dll` 一致（x86 或 x64）。除连接类接口外，成功返回多为 `true` 或 `0`；`Result`、`ErrInfo` 为 **GBK** 文本，`Result` 行以 `\n` 分隔、列以 `\t` 分隔。

---

## 同花顺交易接口 API 介绍

典型流程：`Init` → `Logon` → 查询 / 下单 → `Logoff` → `UnInit`。

`Logon` 的 `Config` 只含 `qs_id` 和 `account_type`。`qs_id` 是券商 ID，模拟炒股为 `356`，须与登录地址是同一家。`account_type`：`0` 资金帐户，`1` 深圳，`2` 上海，`3` 基金，`4` 深圳Ｂ股，`5` 上海Ｂ股，`k` 客户号。模拟站点示例：`{"qs_id":356,"account_type":0}`，地址 `trade.10jqka.com.cn`，端口 `8002`，营业部 `48`。

### 会话与查询

| API | 用途 |
| --- | --- |
| `Logon` | 登录券商；成功返回 `ClientID`（≥0），失败 -1 |
| `Logoff` | 注销指定 `ClientID` |
| `QueryData` | 当日数据：资金、持仓、委托、成交、可撤单、股东代码、新股等 |
| `QueryHistoryData` | 历史委托、历史成交、交割单、资金明细、对账单（日期 `yyyyMMdd`） |
| `QueryDatas` | 同一账户、多 `Category` 批量查询 |
| `QueryMultiAccountsDatas` | 多账户批量查询 |
| `SetPushCallback` | 登录成功后注册成交回报；空回调表示注销 |

登录成功后调用一次 `SetPushCallback`。券商有推送时，接口在单独线程里调用回调。文本是 GBK，第一行表头，第二行一条记录。列依次是 `ClientID`、`推送类型`、`证券代码`、`方向`、`价格`、`数量`、`金额`、`状态`、`成交时间`、`股东代码`、`成交编号`、`剩余数量`、`摘要`。指针只在回调返回前有效。`FunctionID` 当前为 2304。返回 0 成功，-1 表示 `ClientID` 无效。

推送类型有 `成交`、`公告`、`登录`、`系统状态`。成交的方向：`1` 为 `买入`，`2` 为 `卖出`。成交的状态：`1` 为 `部分成交`，`2` 为 `全部成交`，其他状态保留数字。分笔成交会逐条回调。数量、价格、金额按券商原文。剩余数量在纯数字且能被 100 整除时除以 100，按股给出，否则保留原文。

这是通知，不能代替 `QueryData` 的当日成交。推送里的成交编号不是柜台合同编号。回调里可以再查 `QueryData`，不要在回调里 `Logoff` 或 `UnInit`。处理得慢时，还没送出的推送最多留 64 条，更早的会被丢掉。C#、Python、Java 要自己留住回调对象，释放前先注销。

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

### L2

DLL 中仍保留下列导出。L2 定制联系 [Telegram：@Thsapi](https://t.me/Thsapi)。

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

交易示例已填同花顺模拟炒股站点。账号换成手机号、同花顺账户或股东账户，资金账号留空，密码换成模拟交易密码，通讯密码多数留空。语言导航在上面的 [调用示例](#调用示例)。
