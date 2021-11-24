const invodb = require('..')

invodb.database('hr.invodb')

let person = invodb.collection('persons')
if(!person.exist()) person.create();

person.insert({
    name: "张三",
    age: 22,
    salary: 3000,
    kpi: 0.6
})

person.insert({
    name: "李四",
    age: 35,
    salary: 6000,
    kpi: 0.8
})

person.insert({
    name: "王五",
    age: 37,
    salary: 20000,
    kpi: 0.95
})

person.insert({
    name: "李雷",
    age: 32,
    salary: 15000,
    kpi: 0.35
})


console.log("优化互联网人力资源结构专项名单:")
let hr = person.query({
    $or: [
        {
            age: { $gte: 35 },
            salary: { $lt: 10000 }
        },
        {
            kpi: { $lt: 0.4 },
            salary: { $gte: 10000 }
        }
    ]
})
console.log(hr)

