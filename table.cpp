/*
 * table.cpp
 *
 *  Created on: Sep 27, 2016
 *      Author: andrei
 */

#include "table.h"

void Table::Debug()
{
	std::cout << "Table : " << std::endl;
	auto it = columns.begin();
	auto end = columns.end();
	for (; it != end; ++it)
	{
		std::pair<uint32_t, std::shared_ptr<Column>> column = *it;

		std::cout << column.second->toString() << std::endl;
	}
}

std::shared_ptr<Column> Table::getColumn(uint32_t column)
{
	return columns.at(column);
}

std::shared_ptr<Printer> Table::getPrinter(uint32_t column)
{
	return printers.at(column);
}

bool Table::validate()
{
	bool valid = false;

	uint32_t min_entries = INT_MAX;
	uint32_t max_entries = 0;
	uint32_t num_columns = this->num_columns();
	uint32_t i = 0;
	for (; i < num_columns; i++)
	{
		uint32_t entries = columns.at(i)->entries;
		if (min_entries > entries)
		{
			min_entries = entries;
		}
		if (max_entries < entries)
		{
			max_entries = entries;
		}
	}

	if (max_entries == min_entries)
	{
		valid = true;
	}

	return valid;
}

uint32_t Table::num_columns()
{
    return static_cast<uint32_t>(this->columns.size());
}

uint32_t Table::num_rows()
{
	if (this->validate())
	{
		return this->getColumn(0)->entries;
	}
	else
	{
		return 0;
	}
}

std::map<uint32_t, std::shared_ptr<PrimitiveValue>>* Table::getPrimitives()
{
	return &primitives;
}

std::map<uint32_t, std::shared_ptr<Printer>>* Table::getPrinters()
{
	return &printers;
}

std::map<uint32_t, std::shared_ptr<Column>>* Table::getColumns()
{
	return &columns;
}

void Table::buildFrom(std::vector<std::shared_ptr<Row>> rows)
{
	auto it = rows.begin();
	auto end = rows.end();
	for (; it != end; ++it)
	{
		std::shared_ptr<Row> row = (*it);
		auto it_row = row->columns.begin();
		auto end_row = row->columns.end();

		uint32_t pos = 0;

		for (; it_row != end_row; ++it_row)
		{
			auto value = (*it_row);
            char* chunk = new char[value->typeSize()];
			memcpy((void*) chunk, (void*) value->chunk, value->typeSize());

			columns.at(pos)->Append((unsigned char*) chunk);

			pos++;
		}
	}
}
