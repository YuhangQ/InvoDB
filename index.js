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
    function query(object) {
        if(!core.exists(collectionName)) throw `Collection ${collectionName} doesn't exists!`;
        let json = JSON.stringify(object);
        let res = [];
        for(let str of core.query(collectionName, json)) {
            res.push(JSON.parse(str));
        }
        return res;
    }
    return {
        exist: exist,
        create: create,
        insert: insert,
        remove: remove,
        query: query
    }
}

module.exports = {
    database: database,
    collection: collection
};

