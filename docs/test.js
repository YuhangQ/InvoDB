const invodb = require('..')

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