const core = require("./build/Release/core.node");

function database(filename) {
    core.database(filename);
}

function colection(collectionName) {
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
        return core.query(collectionName, json);
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
    colection: colection
};
