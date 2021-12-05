const core = require("./build/Release/invodb.node");

function database(filename) {
    core.database(filename);
}

function collection(collectionName) {
    function exist() { return core.exists(collectionName); }
    function create() { core.create(collectionName); }
    function insert(object) {
        if(!core.exists(collectionName)) throw `Collection ${collectionName} doesn't exists!`;
        let json = JSON.stringify(object);
        core.insert(collectionName, json);
    }
    function remove(object) {
        if(!core.exists(collectionName)) throw `Collection ${collectionName} doesn't exists!`;
        let json = JSON.stringify(object);
        core.remove(collectionName, json);
    }
    function find(object) {
        if(!core.exists(collectionName)) throw `Collection ${collectionName} doesn't exists!`;
        let json = JSON.stringify(object);
        let res = [];
        for(let str of core.query(collectionName, json)) {
            res.push(JSON.parse(str));
        }
        return res;
    }
    function findOne(object) {
        let res = find(object);
        if(res.length >= 1) return res[0];
        return undefined;
    }
    return {
        exist: exist,
        create: create,
        insert: insert,
        remove: remove,
        find: find,
        findOne: findOne,
        update: insert
    }
}

module.exports = {
    database: database,
    collection: collection
};

