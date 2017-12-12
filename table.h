/*
 * table.h
 *
 *  Created on: Sep 27, 2016
 *      Author: andrei
 */

#ifndef SRC_TABLE_H_
#define SRC_TABLE_H_

#include "types.h"
#include "schema.h"
#include "column.h"
#include "primitive.h"
#include "printer.h"
#include "row.h"

#include <climits>
#include <string>
#include <iostream>

class Table
{
private:
	Schema& schema;
    std::map<uint32_t, std::shared_ptr<Column>> columns;
    std::map<uint32_t, std::shared_ptr<Printer>> printers;
    std::map<uint32_t, std::shared_ptr<PrimitiveValue>> primitives;
	void Init()
	{
        std::vector<std::shared_ptr<Node>> nodes = schema.getNodes();
		auto it = nodes.begin();
		auto end = nodes.end();
		for (; it != end; ++it)
		{
			std::shared_ptr<Node> node = *it;
			columns.emplace(
					std::make_pair<uint32_t, std::shared_ptr<Column>>(
                            static_cast<uint32_t>(columns.size()), Column::Make(node)));
			printers.emplace(
					std::make_pair<uint32_t, std::shared_ptr<Printer>>(
                            static_cast<uint32_t>(printers.size()), Printer::Make(node)));
			primitives.emplace(
					std::make_pair<uint32_t, std::shared_ptr<PrimitiveValue>>(
                            static_cast<uint32_t>(primitives.size()), Row::Make(node)));
		}
	}

public:
    uint32_t walCounter = 0;
    Table(Schema& _schema) :
            schema(_schema)
	{
		Init();
	}
	~Table()
	{
	}
	void Debug();
	std::shared_ptr<Column> getColumn(uint32_t column);
	std::shared_ptr<Printer> getPrinter(uint32_t column);
	bool validate();
	uint32_t num_columns();
	uint32_t num_rows();
    std::map<uint32_t, std::shared_ptr<PrimitiveValue>>* getPrimitives();
    std::map<uint32_t, std::shared_ptr<Printer>>* getPrinters();
    std::map<uint32_t, std::shared_ptr<Column>>* getColumns();
    void buildFrom(std::vector<std::shared_ptr<Row>> rows);

	Schema& getSchema()
	{
		return schema;
	}

	void Release() {
		columns.clear();
		printers.clear();
		primitives.clear();
	}
};

#endif /* SRC_TABLE_H_ */
