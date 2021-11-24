const { Sequelize, Model, DataTypes } = require('sequelize');
const { execSync } = require("child_process")

execSync("rm -rf benchmark.sqlite")

let time = 0
function clock() {
    let last = time
    time = new Date().getTime()
    return (time - last) / 1000
}

const sequelize = new Sequelize({
    dialect: 'sqlite',
    storage: 'benchmark.sqlite'
});


const User = sequelize.define('user', {
    qq: Sequelize.STRING(11),
    phone: Sequelize.STRING(11)
});
  
(async () => {
    clock()
    await sequelize.sync();

    let arr = []
    for(let i=0; i<10000; i++) {
        let json = {
            qq: randomString(11),
            phone: randomString(11)
        }
        const jane = await User.create(json);
        arr.push(json)
    }
    console.log("[TEST]插入" + clock())
    for(let i=0; i<10000; i++) {
        let test = await User.findOne({where:{qq: arr[i].qq}})
        console.log(test)
    }
    console.log("[TEST]查询" + clock())
})();


function randomString(e) {    
    e = e || 32;
    var t = "ABCDEFGHJKMNPQRSTWXYZabcdefhijkmnprstwxyz2345678",
    a = t.length,
    n = "";
    for (i = 0; i < e; i++) n += t.charAt(Math.floor(Math.random() * a));
    return n
}
