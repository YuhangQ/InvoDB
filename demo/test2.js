const invodb = require('..')

invodb.database('db.invodb')

let person = invodb.collection('persons')
if(!person.exist()) person.create();

person.insert({
    name: "张三",
    age: 19,
    courses: ["数据库", "计组", "编译原理", "操作系统"]
})

person.insert({
    name: "李四",
    age: 18,
    courses: ["数据库", "线代", "数学分析", "操作系统", "数电"]
})

person.insert({
    name: "王五",
    age: 21,
    courses: ["数据库", "嵌入式", "微机", "计网", "STP"]
})

person.insert({
    name: "李雷",
    age: 22,
    courses: ["吃", "喝", "玩", "乐"]
})


console.log("看看谁学习 数据库 和 操作系统:")
let good = person.query({
    courses: ["数据库", "操作系统"]
})
console.log(good)


console.log("看看谁姓李:")
let li = person.query({
    name: { 
        $gte: "李",
        $lt: "杏"
    }
})
console.log(li)