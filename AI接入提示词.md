# 交给 AI

复制下面整段，贴给正在改你项目的 AI。它会先问几个问题，你回答完，它再改代码。

初次使用，如果对产品还有疑虑，可以用同花顺模拟账户测试。登录账号填手机号、同花顺账户或股东账户，三选一，资金账号留空。填法看 [模拟账户.md](模拟账户.md)。

## 交易

```text
你在帮我把现有项目接到 thsapi.dll。先问我下面这些问题，我回答完再改代码。不要一次改很多无关的东西。

项目和示例在 https://github.com/jyApiZx/stockapi 。交易测试包 1.0.6.2，里面是 thsapi.dll、xdDemo.exe 和 ths.xml。64 位：https://github.com/jyApiZx/stockapi/releases/download/v1.0.6.2/thsapi_1.0.6.2_x64.zip 。32 位：https://github.com/jyApiZx/stockapi/releases/download/v1.0.6.2/thsapi_1.0.6.2_x32.zip 。能用 64 位就用 64 位。模拟账户怎么填：https://thsapi.com/sim.html 。

先问：
1. 项目是 C++、C#、Python 还是 Java？程序是不是 64 位？能用 64 位就用 64 位。32 位程序只能配 32 位 DLL。Python 先看本机 Python 是不是 64 位。
2. 我用的哪家券商？是不是信用账户？
3. 要不要接行情？我如果已经有行情，而且稳定，就不要换成这份。没有，或者现在的行情不稳定，再用 HqApi.dll。

登录不要问我券商 IP、端口、营业部、qs_id。这些到我下载包里的 ths.xml，按券商名字查。模拟炒股是 qs_id 356，地址 trade.10jqka.com.cn，端口 8002，营业部 48。

登录规则：
- 模拟炒股：登录账号填手机号、同花顺账户或股东账户，三选一。资金账号留空。通讯密码留空。填法见 https://thsapi.com/sim.html 。
- 大多数券商只要资金账号和交易密码。account_type 用 0。交易账号和通讯密码都传空字符串 ""。
- 华泰证券要客户号。资金账号一般和客户号相同。通讯密码和交易密码都要填。account_type 用 "k"。
- 信用账户：登录账号在普通资金账号开头加大写 X。资金账号填信用资金账号，不要加 X。普通账户不要加 X。

程序旁边放 thsapi.dll 就行，位数要和程序一样。只支持 Windows。ths.xml 只用来查券商，不必打进程序。

Init 和 UnInit 整个进程只调用一次。可以登录多个账户，每个账户一个 ClientID，用完各自 Logoff。同一个 ClientID 不要多线程一起调用。

查询前调用 EnableResultColumnIdSuffix(1)，按字段 ID 取列，不要按中文列名。常用：2112 资金余额，2116 可用金额，2102 证券代码，2106 股东代码，2135 委托编号。下单前要有股东代码。撤单要用委托编号。

普通账户只查资金、持仓、当日委托、当日成交、可撤单、股东代码。两融和新股先别调。中文结果是 GBK，不是 UTF-8。Result 开 1MB，ErrInfo 开 256 字节。登录失败先看 ErrInfo。

成交回报：登录成功后调用 SetPushCallback 注册一次。回调文本是 GBK，第一行表头，第二行一条记录。状态会出现「部分成交」「全部成交」，也可能是数字，不要只认「已成」。回调返回前把内容拷走。不要在回调里 Logoff 或 UnInit。字段和注意事项以 README、https://thsapi.com/trade.html 为准。

查询成功之前不要发实盘单。账号和密码不要写进会提交的代码。其余接口去读 https://github.com/jyApiZx/stockapi 的 README、交易目录里的示例，以及 https://thsapi.com/trade.html ，不要凭记忆编参数。我还没有项目时，再按示例新写一份。
```

## 行情

```text
你在帮我把现有项目接到 HqApi.dll。先问我项目是 C++、C#、Python 还是 Java，程序是不是 64 位。能用 64 位就用 64 位。32 位程序只能配 32 位 DLL。我回答完再改代码。

项目和示例在 https://github.com/jyApiZx/stockapi 。行情包里面是 HqApi.dll 和演示程序。64 位：https://github.com/jyApiZx/stockapi/releases/download/hqapi-1.0.7.0/hqapi_1.0.7.0_x64.zip 。32 位：https://github.com/jyApiZx/stockapi/releases/download/hqapi-1.0.7.0/hqapi_1.0.7.0_x32.zip 。能用 64 位就用 64 位。

只支持 Windows。程序旁边放 HqApi.dll，位数要和程序一样。授权含 L1 与扩展行情。L1 能取实时行情、Tick、买卖五档，以及一分钟、五分钟 K 线。扩展行情用 TdxExHq_*，常见端口 7727 / 7721。

L1 连 7709，账号写成 L1*账号，不能用 guest，也不能空着。密码填该账号的密码，不能是空串。不要接 L2，也不要按 L2 去配账号。

行情站点用仓库 行情/cpp/Demo.cpp 里的主站列表。A 股的连接号不要拿去调扩展行情。中文结果是 GBK。Result 开 1MB，ErrInfo 开 256 字节。

其余参数去读 https://github.com/jyApiZx/stockapi 里 行情 目录的示例和 https://thsapi.com/hq.html ，不要凭记忆编接口。我还没有项目时，再按示例新写一份。
```
