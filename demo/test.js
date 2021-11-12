const invodb = require('..')

invodb.database('zzz.invodb')

let col = invodb.colection('blog')
if(!col.exist()) col.create();

col.insert({ id: 1, title: "第一篇文章", author: "YuhangQ"})
col.insert({ id: 2, title: "第二篇文章", author: "Ciel"})
col.insert({ id: 3, title: "第三篇文章", author: "YuhangQ"})
col.insert({ id: 4, title: "第四篇文章", author: "By"})

let result = col.query({
    id: {
        $gte: 2,
        $lte: 3
    },
    author: "YuhangQ"
})

console.log(result)