
const fs = require('fs')

const invodb = require('..')

invodb.database('zzz.invodb')

let col = invodb.colection('blog')
if(!col.exist()) col.create();

for(let json of col.query({})) col.remove(json)

let list = fs.readFileSync(__dirname + "/list.txt").toString().split("\n")
for(let json of list) {
    col.insert(JSON.parse(json))
}

let result = col.query({
    id: {
        $gte: 1,
        $lte: 8,
        $or: [
            { $ne: 2 },
            { $ne: 3 }
        ]
    }
})

console.log(">>>>>>>>>>>>>>>>>>>>")
console.log(result)
console.log(result.length)