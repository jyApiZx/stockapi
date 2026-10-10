# 行情 API 说明

`Api.dll` 提供 A 股行情（`TdxHq_*`）与扩展行情（`TdxExHq_*`）两套 stdcall 导出。本文面向接入方：参数、返回值、`Result` 列含义与调用注意。

接入重点是 L2：十档盘口（`TdxHq_GetSecurityQuotes10`）、买卖队列（`TdxHq_GetBuySellQueue`）、逐笔成交明细（`TdxHq_GetDetailTransactionData`、`TdxHq_GetDetailOriginalTransactionData`）和逐笔委托（`TdxHq_GetDetailOrderData`）。这些接口需要 L2 主机与相应授权，常见端口 `7719`。

---

## 1. 约定

### 1.1 缓冲区与编码

| 项 | 说明 |
|----|------|
| `Result` | 调用方分配的输出缓冲；多数接口写入 **GBK** 文本（表头 + TSV 行） |
| 建议大小 | 行情/列表类常用 **≥ 1～8 MB**；板块全量等更大结果需加大缓冲 |
| `ErrInfo` | 失败原因字符串；建议 ≥ 256 字节 |
| 成功/失败 | `bool` 接口：`true` 成功；`int` 连接接口：`≥0` 为连接 ID，`-1` 失败 |

### 1.2 市场与 K 线类别

| `Market` | 含义 |
|----------|------|
| `0` | 深圳 |
| `1` | 上海 |
| `2` | 北交所 |

北交所代码（`92xxxx` 及旧码 `43xxxx`/`83xxxx`）可传 `Market=0` 或 `2`。`TdxHq_GetSecurityQuotes` / `TdxHq_GetSecurityQuotes10` 会按代码将市场规范为 `2`。

扩展行情（`TdxExHq_*`）的市场号以 `TdxExHq_GetMarkets` 返回为准，**不要**套用上表。

| `Category` | K 线周期 |
|------------|----------|
| `0`～`3` | 5 / 15 / 30 / 60 分钟 |
| `4` | 日线 |
| `5` | 周线 |
| `6` | 月线 |
| `7`～`10` | 1 分钟及扩展周期（以主机为准） |

分页：`Start` 从最新往历史偏移；`Count` 为入出参（入=请求条数，出=实际条数）。

### 1.3 线程安全

- **同一连接 ID**：同一时刻只允许一条 I/O（`Connect` 之后的取数调用）。多线程共用同一 ID 时，请在调用方加锁。
- **不同连接 ID**：可并行使用。
- `TdxHq_*` 与 `TdxExHq_*` 的连接空间**分开**，勿混用 `ConnectionID`。

### 1.4 账号与密码

`Connect` 需要有效的 `Account` / `Password`（由授权方提供）。请使用正式下发的凭证，**不要**在业务代码或文档中写死测试口令。

- A 股常见端口：L1 `7709`，L2 `7719`（以实际主机为准）。
- 扩展行情常见端口：`7727` / `7721`。
- 未授权或凭证错误时，连接返回 `-1`，原因见 `ErrInfo`。

### 1.5 主机能力差异

不同行情主机能力不同，例如：

| 能力 | 说明 |
|------|------|
| L1 主机 | 五档、K 线、分时、分笔、板块等 |
| L2 主机 | 十档、买卖队列、逐笔成交/委托等 |
| 报表类主机 | `GetReportFile`、部分历史资金流等 |

同一接口在不同主机上结果可能不同。空表或失败时，先核对授权与主机能力，再检查参数。

---

## 2. 连接

### 2.1 A 股

```c
int TdxHq_Connect(const char* IP, u_short Port,
                  const char* Account, const char* Password,
                  char* Result, char* ErrInfo);
void TdxHq_Disconnect(int ConnectionID);
```

成功：`Result` 含服务器信息（如服务器名、交易日）；返回值即 `ConnectionID`。  
失败：返回 `-1`，查看 `ErrInfo`（超时、拒绝、鉴权失败等）。

```text
服务器名称	最后交易日期
HQ	20260911
```

### 2.2 扩展行情

```c
int  TdxExHq_Connect(char* IP, int Port, char* Account, char* Password,
                     char* Result, char* ErrInfo);
void TdxExHq_Disconnect(int ConnectionID);
```

`Port≤0` 时默认 `7727`。凭证规则与 A 股相同，使用授权方提供的扩展行情账号。

---

## 3. 专题：板块

### 3.1 流程建议

1. `ListBoards` — 浏览板块名与成分数量  
2. `ListBoardMembers` — 按板块名取成分代码  
3. 或 `GetBlockInfo` — 一次取「名称 + 全部代码」（结果可能很大）

`BoardName` 必须为 **GBK**，且与列表中的 `name` 一致。

### 3.2 板块文件

| 类型参数 | 文件 |
|----------|------|
| `concept` / `gn` / 空 | `block_gn.dat` |
| `style` / `fg` | `block_fg.dat` |
| `industry` / `index` / `zs` | `block_zs.dat` |

### 3.3 板块指数 K 线

板块指数代码多为 `880xxx` / `881xxx`，请用 **`TdxHq_GetIndexBars`**（与上证综指、深成指相同），不要用个股 `GetSecurityBars`。

```cpp
u_short n = 100;
TdxHq_GetIndexBars(id, 4, 1, "880506", 0, n, Result, ErrInfo);
```

---

## 4. A 股接口：`TdxHq_*`

### 4.1 `TdxHq_GetSecurityCount`

```c
bool TdxHq_GetSecurityCount(int ConnectionID, BYTE Market,
                            u_short& Result, char* ErrInfo);
```

此处 `Result` 为 **数量**（`u_short&`），不是字符串缓冲。

### 4.2 `TdxHq_GetSecurityList`

```c
bool TdxHq_GetSecurityList(int ConnectionID, BYTE Market,
                           u_short Start, u_short& Count,
                           char* Result, char* ErrInfo);
```

**列：** `代码 | 一手股数 | 名称 | 保留 | 价格小数位 | 昨收 | 保留 | 保留`

`Count` 出参 = 本页行数（不含表头）。数据量大时用 `Start` / `Count` 分页拉取。

```text
代码	一手股数	名称	保留	价格小数位数	昨收	保留	保留
600000	100	浦发银行	0	2	10.500000	0	0
```

### 4.3 `TdxHq_GetSecurityBars` / `TdxHq_GetIndexBars`

```c
bool TdxHq_GetSecurityBars(int ConnectionID, BYTE Category,
                           BYTE Market, const char* Zqdm,
                           u_short Start, u_short& Count,
                           char* Result, char* ErrInfo);

bool TdxHq_GetIndexBars(int ConnectionID, BYTE Category,
                        BYTE Market, const char* Zqdm,
                        u_short Start, u_short& Count,
                        char* Result, char* ErrInfo);
```

| API | 用途 | `Result` 列 |
|-----|------|-------------|
| `GetSecurityBars` | 个股 / 基金等 | `时间 \| 开 \| 收 \| 高 \| 低 \| 成交量 \| 成交额` |
| `GetIndexBars` | 指数 / 板块指数 | 同上 + **`涨家数 \| 跌家数`** |

选用规则：

- 上证综指 `1,"999999"`、深成指 `0,"399001"`、板块 `1,"880xxx"/"881xxx"` → **`GetIndexBars`**
- 普通股票（含北交所 `2,"92xxxx"`）→ **`GetSecurityBars`**
- 误用另一套解析会导致列错位

```text
时间	开盘价	收盘价	最高价	最低价	成交量	成交额
20260910	1480.000000	1495.500000	1502.000000	1475.000000	12345.000000	1800000000.000000
```

### 4.4 分时

```c
bool TdxHq_GetMinuteTimeData(int ConnectionID, BYTE Market,
                             const char* Zqdm, char* Result, char* ErrInfo);

bool TdxHq_GetHistoryMinuteTimeData(int ConnectionID, BYTE Market,
                                    const char* Zqdm, int Date,
                                    char* Result, char* ErrInfo);
```

`Date`：`YYYYMMDD`。  
**列：** `现价 | 成交量 | 均线`

### 4.5 分笔成交

```c
bool TdxHq_GetTransactionData(int ConnectionID, BYTE Market,
                              const char* Zqdm, u_short Start, u_short& Count,
                              char* Result, char* ErrInfo);

bool TdxHq_GetHistoryTransactionData(int ConnectionID, BYTE Market,
                                     const char* Zqdm, u_short Start,
                                     u_short& Count, int Date,
                                     char* Result, char* ErrInfo);
```

**当日 / 历史列（相同）：** `时间 | 价格 | 现量 | 笔数 | 买卖 | 保留`  
价格为元；历史分笔无笔数时 `笔数` 为 `0`。

`买卖` 为整数枚举（非 `B`/`S` 字符）：

| 值 | 含义 |
|----|------|
| `0` | 买 |
| `1` | 卖 |
| `4` / `8` 等 | 其它（方向不明，如中性/竞价等） |

### 4.6 五档 / 十档行情

```c
bool TdxHq_GetSecurityQuotes(int ConnectionID, BYTE Market[],
                             const char* Zqdm[], u_short& Count,
                             char* Result, char* ErrInfo);

bool TdxHq_GetSecurityQuotes10(int ConnectionID, BYTE Market[],
                               const char* Zqdm[], u_short& Count,
                               char* Result, char* ErrInfo);
```

`Market[]` / `Zqdm[]` 并行；`Count` 入=请求只数，出=成功写入行数。

**批量上限：每包最多 80 只。** 一次传入超过 80 时，DLL 会自动按 80 分片后合并结果；业务侧也可自行按 80 切片。

十档依赖主机/L2 能力；普通 L1 上 6～10 档常为 0。

表头含：市场、代码、活跃度、现价、昨收、开高低、时间、总量、现量、金额、内外盘、买卖五档价量、涨速等。

```cpp
BYTE mk[200];
const char* codes[200];
u_short cnt = 200;  // 内部自动分片
TdxHq_GetSecurityQuotes(id, mk, codes, cnt, Result, ErrInfo);
```

### 4.7 公司信息 / 除权 / 财务 / 集合竞价

```c
bool TdxHq_GetCompanyInfoCategory(...);
bool TdxHq_GetCompanyInfoContent(...);   // 正文写入 Result（GBK）
bool TdxHq_GetXDXRInfo(...);
bool TdxHq_GetFinanceInfo(...);
bool TdxHq_GetCallAuctionData(...);
```

Category 列：`类别 | 文件 | 开始 | 长度`，再按偏移调用 Content。  
部分接口需对应授权等级；失败时看 `ErrInfo`。

### 4.8 L2：买卖队列与逐笔

```c
bool TdxHq_GetBuySellQueue(...);
bool TdxHq_GetDetailTransactionData(...);
bool TdxHq_GetDetailOriginalTransactionData(...);
bool TdxHq_GetDetailOrderData(...);
```

均需 **L2** 主机与相应授权。

| 函数 | 表头要点 |
|------|----------|
| `GetBuySellQueue` | 买卖价量、笔数、队列（多笔用 `\|` 分隔） |
| `GetDetailTransactionData` | `成交时间 \| 价格 \| 成交量 \| 性质` |
| `GetDetailOriginalTransactionData` | 上表 + 买卖方委托号 |
| `GetDetailOrderData` | `委托时间 \| 价格 \| 股数 \| 性质` |

### 4.9 板块：`GetBlockInfo` / `ListBoards` / `ListBoardMembers`

```c
bool TdxHq_GetBlockInfo(int ConnectionID, const char* BlockFile,
                        char* Result, char* ErrInfo);
bool TdxHq_ListBoards(int ConnectionID, const char* BoardType,
                      char* Result, char* ErrInfo);
bool TdxHq_ListBoardMembers(int ConnectionID, const char* BoardName,
                            const char* BlockFile, char* Result, char* ErrInfo);
```

| 接口 | 列 |
|------|----|
| `GetBlockInfo` | `name \| type \| count \| codes`（codes 逗号分隔） |
| `ListBoards` | `name \| type \| count` |
| `ListBoardMembers` | `code`（一行一个） |

全量概念板块可能超过默认缓冲；失败时加大 `Result` 或改用 `ListBoards` + `ListBoardMembers`。

### 4.10 `TdxHq_GetReportFile`

```c
bool TdxHq_GetReportFile(int ConnectionID, const char* FileName,
                         const char* SavePath, char* Result, char* ErrInfo);
```

| 参数 | 说明 |
|------|------|
| `FileName` | 服务器相对路径，如 `tdxfin/gpcw.txt` |
| `SavePath` | 本地保存路径 |
| `Result` | 成功时为十进制字节数字符串 |

需主机开通报表下载能力；以授权方说明为准。

### 4.11 `TdxHq_GetMarketStat`

```c
bool TdxHq_GetMarketStat(int ConnectionID, char* Result, char* ErrInfo);
```

**列：** `up | down | neutral | suspended | total | amount | volume`（涨跌家数统计，盘中变化）。

### 4.12 资金流

```c
bool TdxHq_GetFundFlow(int ConnectionID, BYTE Market, const char* Zqdm,
                       char* Result, char* ErrInfo);

bool TdxHq_GetHistoryFundFlow(int ConnectionID, BYTE Market, const char* Zqdm,
                              u_short Start, u_short Count,
                              char* Result, char* ErrInfo);
```

当日资金流按分笔成交额分档（超大/大/中/小流入流出及净流入）。可能较慢（多页分笔）。  
历史资金流依赖主机能力；无数据行时查看 `ErrInfo`，或改用历史分笔接口按日汇总。

---

## 5. 扩展行情：`TdxExHq_*`

用于期货 / 港股 / 美股等。先 `GetMarkets` 再按返回的市场号调用其它接口。

### 5.1 `TdxExHq_GetMarkets`

**列：** `市场 | 商品类别 | 市场名称 | 市场简称`

### 5.2 合约数量与资料

```c
bool TdxExHq_GetInstrumentCount(int ConnectionID, int& Result, char* ErrInfo);
bool TdxExHq_GetInstrumentInfo(int ConnectionID, int Start, short Count,
                               char* Result, char* ErrInfo);
```

Info 列：`商品类别 | 市场 | 代码 | 名称 | 说明`。`Count≤0`→100；上限约 511。

### 5.3 K 线 / 分时 / 分笔 / 报价

| 函数 | 说明 |
|------|------|
| `GetInstrumentBars` | 列同个股 K 线；`Category` 见 §1.2 |
| `GetMinuteTimeData` / `GetHistoryMinuteTimeData` | `时间 \| 价格 \| 均价 \| 成交量 \| 持仓` |
| `GetTransactionData` / `GetHistoryTransactionData` | 扩展分笔 |
| `GetInstrumentQuote` | 单品种报价（昨收、开高低现、开平仓、量、内外盘、五档等） |

---

## 6. 最小示例

```cpp
char Result[8 * 1024 * 1024] = {};
char ErrInfo[256] = {};

int id = TdxHq_Connect("行情主机IP", 7709,
                       "your_account", "your_password",
                       Result, ErrInfo);
if (id < 0) { /* 查看 ErrInfo */ return; }

BYTE mk[] = { 1, 0 };
const char* codes[] = { "600519", "000001" };
u_short cnt = 2;
TdxHq_GetSecurityQuotes(id, mk, codes, cnt, Result, ErrInfo);

TdxHq_GetMarketStat(id, Result, ErrInfo);

TdxHq_ListBoards(id, "concept", Result, ErrInfo);
// 从 Result 解析 GBK 板块名后：
// TdxHq_ListBoardMembers(id, boardName, "block_gn.dat", Result, ErrInfo);

TdxHq_Disconnect(id);
```

报表下载（需支持该功能的主机）：

```cpp
int calc = TdxHq_Connect("报表主机IP", 7709,
                         "your_account", "your_password",
                         Result, ErrInfo);
if (calc >= 0) {
    TdxHq_GetReportFile(calc, "tdxfin/gpcw.txt",
                        "C:\\temp\\gpcw.txt", Result, ErrInfo);
    TdxHq_Disconnect(calc);
}
```

---

## 7. 接入检查清单

1. 使用授权方提供的 DLL、主机列表与账号密码。  
2. `Result` / `ErrInfo` 缓冲足够大；字符串按 **GBK** 解析。  
3. 批量行情单次逻辑包 ≤ **80** 只（或交给 DLL 自动分片）。  
4. 指数 / 板块指数用 `GetIndexBars`，个股用 `GetSecurityBars`。  
5. A 股与扩展行情连接 ID 不要混用；同 ID 避免并发 I/O。  
6. 空表或失败时先换主机/核对授权，再查参数。
