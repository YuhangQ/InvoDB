const invodb = require('..')

invodb.database('zzz.invodb')

let col = invodb.colection('blog')
if(!col.exist()) col.create();

for(let i=0; i<1; i++) {
    col.insert({ id: "1s", title: "第一篇文章", author: "YuhangQ"})
    col.insert({ id: "2s", title: "第二篇文章", author: "Ciel"})
    col.insert({ id: "3s", title: "第三篇文章", author: "YuhangQ"})
    col.insert({ id: "4s", title: "第四篇文章", author: "By"})
}

let result = col.query({id: {}})

// for(let json of result) {
//     col.remove(json)
// }

console.log(result)
console.log(result.length)