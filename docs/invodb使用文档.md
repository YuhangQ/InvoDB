文件 API：Ciel

语法解析器：徐小权、钱宇航

B+索引树：钱宇航、卢胤

测试：zyy，刘宇轩，徐小权

控制台开发：卢胤

文档：刘宇轩，钱宇航，zyy

挑毛病专员：zyy



ZYY：前置小知识：json、js、nodejs、invodb、invodb-node-api。实现了什么？

ZYY：【演示】nodejs 使用实例，展现易用性。插入删除更新 | 分发 invodb.exe

XXQ：索引算法的实现 && 数据库索引教程

LY：nodejs 演示索引(提前准备一个)

LYX：BenchMark跑分 vs sqlite

演示实际项目 钱宇航



# 简明 InvoDB 使用教程

通过阅读该教程，你会对 `InvoDB` 的使用和结构有所了解。

## 前置知识

### 什么是 JSON 格式

是一种轻量级资料交换格式，其内容由属性和值所组成，被设计用于在不同编程语言之间传输数据。

本质上就是键值对的集合，只不过值可以是数字、布尔值、字符串、对象和数组，甚至是 `null` 值。

`json` 可以同样被看作 `JavaScript` 对象的一个子集。

#### 一个典型的 JSON 对象

```json
{
  "id": 7,
  "title": "这是一篇文章",
  "content": "这是文章内容",
  "author": "YuhangQ",
  "category": "数据库",
  "original-url": null,
  "tags": ["数据库", "C++", "数据结构"],
  "parameters": {
    "hidden": true,
    "like": 25565,
  },
  "comment": [
    {"username": "YuhangQ", "content": "文章写的真不错！"},
    {"username": "TechCiel", "content": "能提供下打赏渠道吗？"},
  ]
}
```

### 什么是 JavaScript

一个运行在浏览器上的语言，为网页前端提供复杂的动态特性。

### 什么是 nodeJS

`JavaScript` 的使用人数众多，并且谷歌的 Chrome 浏览器内置的 V8 解释器性能非常好，人们将 V8 引擎提取出来，并且给 `JavaScript` 增加了与系统直接交互的标准库，使得 `JavaScript` 可以不依托浏览器直接解释执行，直接操作文件、进程、网络等资源，类似于 `Python` 。

### 什么是 InvoDB

`InvoDB` 是一个可以在 `1M` 内存内运行，能够存储 `JSON` 文档并且以极高的效率检索出来的数据库。

由 100% C++ 以及纯标准库编写，可以跨平台编译。

### 什么是 InvoDB-Node-API

`json` 可以被看作 `JavaScript` 对象的一个子集。

那么毫无疑问 `JavaScript` 来作为操作本数据库的语言就是最合适的。

`InvoDB-Node-API` 就是提供给 `nodeJS` 使用的一个中间件，以 `nodeJS` 原生的语法访问数据库，完全屏蔽了底层细节。

目前已上传 npm 公共库 [invodb - npm (npmjs.com)](https://www.npmjs.com/package/invodb)

## InvoDB 使用教程

### 前置要求

操作系统要求为 `Linux/macOS` 。

`Windows` 自动编译目前有一些问题，可以手动编译，也可以使用控制台版的 `InvoDB` 进行体验

电脑需要有 `gcc` 编译器需要支持 `C++17`，以及 `nodeJS` 环境，即 `node` 、`npm`、`g++` 命令应可用。

### 开始使用

创建一个新文件夹，并且 `cd` 进入，然后安装 InvoDB 库

```bash
npm install invodb
```

安装完毕，建立一个文件 `test.js` 就可以开始体验了

```javascript
// test.js
const invodb = require('invodb')

invodb.database('test.invodb')

let col = invodb.collection('blog')
if(!col.exist()) col.create();

col.insert({
    "id": 7,
    "title": "这是一篇文章",
    "content": "这是文章内容",
    "author": "YuhangQ",
    "category": "数据库",
    "original-url": null,
    "tags": ["数据库", "C++", "数据结构"],
    "parameters": {
      "hidden": true,
      "like": 25565,
    },
    "comment": [
      {"username": "YuhangQ", "content": "文章写的真不错！"},
      {"username": "TechCiel", "content": "能提供下打赏渠道吗？"},
    ]
})

let result = col.query({})
console.log(result)
```

开始运行，示例代码

```bash
node test.js
// output
[
	{
    __INVO_ID__: '1o9xpqncxldyhvci9crft2b2dj6kroea',
    author: 'YuhangQ',
    category: '数据库',
    comment: [ [Object], [Object] ],
    content: '这是文章内容',
    id: 7,
    'original-url': null,
    parameters: { hidden: true, like: 25565 },
    tags: [ '数据库', 'C++', '数据结构' ],
    title: '这是一篇文章'
  }
]
```

### 插入 JSON 文档

调用 `insert` 方法，传入一个合法的 `json` ，若语法错误，则提示插入失败，不是一个合法 `json`，数据库分配一个唯一的 `__INVO_ID__`。

### 删除 JSON 文档

调用 `delete` 方法，传入一个 `json` ，根据传入 `json` 的 `__INVO_ID__` 值删除掉该文档的原始信息以及索引

### 更新 JSON 文档

调用 `update`  或者  `insert` 方法，传入一个合法的 `json` ，根据传入 `json` 的 `__INVO_ID__` 值更新该文档的原始信息以及索引

### 检索技巧

#### 初级检索技巧

初级检索就是，查找某些特定的值，依旧采用上面的例子，检索博客信息。

也就是尝试更改 `col.query()` 函数里面传入的查询语法

```javascript
// 查询特定 __INVO_ID__ 的文档
{ "__INVO_ID__": "1o9xpqncxldyhvci9crft2b2dj6kroea" }

// 查询作者为 YuhangQ 的所有文档
{ "author": "YuhangQ" }

// 查询所有隐藏文章
{ "parameters": { "hidden": true } }
{ "parameters.hidden": true }

// 查询所有 TAGS 包含 C++ 的文章
{ "tags": ["C++"] }

// 查询所有 TAGS 包含 C++ 以及 数据结构 的文章
{ "tags": ["C++", "数据结构"] }

// 查询所有 YuhangQ 留过言的文章
{ "comment": [ { "username": "YuhangQ" } ] }

// 查询所有 YuhangQ 和 TechCiel 留过言的文章
{ "comment": [ { "username": "YuhangQ" }, { "username": "TechCiel" } ] }

// 查询所有留言内容是 你好 的文章
{ "comment": [ { "content": "你好" } ] }
```

#### 条件查询技巧

条件无非是与或非的排列组合，接下来依次介绍在 InvoDB 语法的实现。

```javascript
// 与: 同时满足条件
// 查询作者是 YuhangQ 并且分类是 数据库 的文章
{ 
	"author": "YuhangQ",
  "category": "数据库"
}

// 或: 满足其一
// 查询作者是 YuhangQ 或者 作者是 TechCiel 的所有文章
{ 
	"$or": [
    { "author": "YuhangQ" },
		{ "author": "TechCiel" }
  ]
}
{
  "author": {
    "$or": ["YuhangQ", "TechCiel"]
  }
}

// 非：不满足
// 查询作者不是 YuhangQ 的文章
{
  "author": { "$ne": "YuhangQ" }
}
```

#### 范围检索技巧

很多时候要返回符合给定值域的文档，进行一个范围查询。

请记住如下五个比较符号，配合前面的与或查询，可以实现任意值域的表示。

| 符号 | 意义                | 说明     |
| ---- | ------------------- | -------- |
| $gt  | greater than        | 大于     |
| $gte | greater than equals | 大于等于 |
| $lt  | less than           | 小于     |
| $lte | less than equals    | 小于等于 |
| $ne  | not equals          | 不等于   |

```javascript
// 查询 13 < id <= 17 并且不等于 15
{
  "id": {
    "$gt": 13,
    "$lte": 17,
    "$ne": 15
  }
}

// 查询 id < 13 或者 i >= 17
{
  "$or": [
    { "id": { "$lt": 13 } },
    { "id": { "$gte": 17 } },
  ]
}

// 也支持字符串比较检索
{
	"title": {
		"$gte": "数据库设计(1)",
		"$lte": "数据库设计(5)",
	}
}
```

### 排序、个数限制、分页等

对于 B+ 树中取出的直接数据进行加工，暂不支持，用户可以手动实现。

### SQL 多表连接等各种丰富功能如何实现

由程序员取出数据，用代码描述相关逻辑并拆分成几本查询步骤。

## InvoDB 优缺点分析

### 优点

- `C++` 编写，软件体积小，无依赖，不需要运行时环境。
- 作为开源项目代码简洁易懂，质量较好。
- 内存占用小到可以忽略。
- 查询效率非常高。
- 使用方法非常简洁，不需要学习就可以快速上手。
- 基本兼容 `MongoDB` 语法，方便现有项目迁入迁出。
- 数据是单个文件，方便备份和转移。
- 支持 `nodeJS` 等语言以原生方式使用。

### 缺点

- 文件系统十分初级，是一个简单 `1kb` 分页结构，很多时候无法装满一个页，造成大量空间浪费。
- 为了极致的易用性，无法自定义索引成员，全部索引，很多时候浪费 `CPU` 资源和硬盘空间。
- `B+` 树的代码功能有限，不支持模版指定任意类型键值对、自定义比较方法、重复键值对。导致树套树再拉链等方法大量浪费存储空间，降低了插入删除效率。

## InvoDB 技术总结

`InvoDB` 使用了 `B+` 索引树，采用了文件分页。

实现了一个简单的索引结构，一个简单的文件结构。

实现了一个 `MongoDB` 语法子集的一个语法解析器。

总之，`InvoDB` 更适合作为一个数据库的学习项目来使用，难以用于大型项目，会导致大量的资源浪费。

但是如果在个人项目中使用，可以快速开发出一个小项目并且没有任何数据库方面的部署成本，还是非常合适的。

## 基于 InvoDB 的示例项目

下面的这个开源项目是我在高中完成的，使用 `MongoDB` 作为数据库后端。

[YuhangQ/ReciteWords: 一个背单词的小网站 (github.com)](https://github.com/YuhangQ/ReciteWords)

我花了大概一个小时的时间让它迁移到了 `InvoDB` 上。

下面是演示网址。

[单词测试 (wento.icu)](https://word.wento.icu/)



演示 BenchMark



