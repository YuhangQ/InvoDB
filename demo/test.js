
const fs = require('fs')

const invodb = require('..')

invodb.database('zzz.invodb')

let col = invodb.collection('blog')
if(!col.exist()) col.create();

for(let json of col.query({})) col.remove(json)

let list = fs.readFileSync(__dirname + "/list.txt").toString().split("\n")
for(let json of list) {
    col.insert(JSON.parse(json))
}

let result = col.query(
    {
        "title": {
          "$gte": "数据库设计(1)",
          "$lte": "数据库设计(5)",
        }
    }
)
console.log(result)