# 简明 invoDB 使用教程


## 前置知识

### 什么是 JSON 格式

是一种轻量级资料交换格式，其内容由属性和值所组成。

`json` 可以同样被看作 `JavaScript` 对象的一个子集。

#### 一个典型的 JSON 对象

```json
{
  "id": 7,
  "title": "这是一篇文章",
  "content": "这是文章内容",
  "author": "YuhangQ",
  "category": "数据库",
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

