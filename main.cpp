/*
 * main.cpp
 *
 *  Created on: Dec 19, 2016
 *      Author: andrei
 */

#include <chrono>

#include "schema.h"
#include "table.h"
#include "reader.h"
#include "dataset.h"

int main()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

    start = std::chrono::high_resolution_clock::now();

    std::vector<std::shared_ptr<Node>> nodes;

    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c1"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c2"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c3"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c4"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c5"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c6"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c7"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c8"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c9"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c10"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c11"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c12"));
    nodes.push_back(std::make_shared<TypedNode<ByteArrayType>>("c13"));
    Schema schema(nodes);
    schema.Debug();

    Table table(schema);
    table.Debug();
    
    std::ifstream is;
    is.open("/home/andrei/Desktop/oldies/desktop4/waa_android.csv", std::ios_base::in);

    CsvReader reader(table);
    reader.read(is);
    
    std::cout << "valid: " << table.validate() << std::endl;
    std::cout << "rows: " << table.num_rows() << std::endl;

//    uint16_t launches = 10;
//    uint16_t country = 44;
//    int32_t level = 10;

    std::vector<std::shared_ptr<Row>> rows;
    Dataset set = Dataset::Init(table);

    std::vector<uint32_t> columns {};

//    set.Fields(columns).Filter(
//                Predicate::Make(table, 3, (uint8_t*) &launches,
//                                Condition::EQUAL_MORE, false)).Filter(
//                Predicate::Make(table, 2, (uint8_t*) &country, Condition::EQUAL,
//                                false)).Filter(
//                Predicate::Make(table, 6, (uint8_t*) &level, Condition::EQUAL,
//                                false)).getRows(rows);

    set.Fields(columns).getRows(rows);

    std::cout << " ======================== " << std::endl;
    std::cout << "rows = " << rows.size() << std::endl;

    auto it = rows.begin();
    auto endIt = rows.end();
    for (; it != endIt; ++it)
    {
        std::shared_ptr<Row> row = (*it);
//        row->toString();

//        std::cout << std::endl;
    }

    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_time = end - start;

    std::cout << "duration = " << elapsed_time.count() << "s" << std::endl;

    return 0;
}

