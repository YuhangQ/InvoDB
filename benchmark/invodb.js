const invodb = require("invodb")
const { execSync } = require("child_process")

execSync("rm -rf benchmark.invodb")

let time = 0
function clock() {
    let last = time
    time = new Date().getTime()
    return (time - last) / 1000
}

invodb.database("benchmark.invodb")
let User = invodb.collection("user")
User.create()

let arr = []
  
clock()
for(let i=0; i<10000; i++) {
    let json = {
        qq: randomString(11),
        phone: randomString(11)
    }
    User.insert(json)
    arr.push(json)
}
console.log("[TEST]插入" + clock())

for(let i=0; i<10000; i++) {
    let res = User.findOne({
        qq: arr[i].qq
    })
    console.log(res)
}

console.log("[TEST]查询" + clock())

function randomString(e) {    
    e = e || 32;
    var t = "ABCDEFGHJKMNPQRSTWXYZabcdefhijkmnprstwxyz2345678",
    a = t.length,
    n = "";
    for (i = 0; i < e; i++) n += t.charAt(Math.floor(Math.random() * a));
    return n
}
