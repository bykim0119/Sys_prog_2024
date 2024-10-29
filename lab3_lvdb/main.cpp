#include <iostream>
#include <leveldb/db.h>

int main()
{
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::Status status = leveldb::DB::Open(options, "mydb", &db);

    if (!status.ok()) {
        std::cerr << "Unable to open/create database 'mydb'" << status.ToString() << std::endl;
        return -1;
    }

    std::string key = "Eunji";
    std::string value = "Seoul";

    status = db->Put(leveldb::WriteOptions(), key, value);
    if (!status.ok()) {
        std::cerr << "Failed to write key and value" << status.ToString() << std::endl;
    }

    std::string read_value;
    status = db->Get(leveldb::ReadOptions(), key, &read_value);
    if (!status.ok()) {
        std::cerr << "Failed to get" << status.ToString() << std::endl;
    } else {
        std::cout << "Read value = " << read_value << std::endl;
    }

    status = db->Delete(leveldb::WriteOptions(), key);
    if (!status.ok()) {
        std::cerr << "Failed to delete" << status.ToString() << std::endl;
    }
    
    return 0;
}
