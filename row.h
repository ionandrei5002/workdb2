/*
 * row.h
 *
 *  Created on: Sep 27, 2016
 *      Author: andrei
 */

#ifndef SRC_ROW_H_
#define SRC_ROW_H_

#include "types.h"
#include "schema.h"
#include "primitive.h"
#include "printer.h"

#include <memory>
#include <vector>
#include <map>

class Row
{
public:
    std::vector<std::shared_ptr<PrimitiveValue>> columns;
    std::vector<std::shared_ptr<Printer>> printer;

	Row() :
            columns(std::vector<std::shared_ptr<PrimitiveValue>>()), printer(
                    std::vector<std::shared_ptr<Printer>>())
	{
	}

    Row(std::vector<std::shared_ptr<PrimitiveValue>> _columns,
            std::vector<std::shared_ptr<Printer>> _printer) :
            columns(_columns), printer(_printer)
	{
	}

	Row(const Row& other) noexcept
	{
		this->columns = other.columns;
		this->printer = other.printer;
	}

	Row(Row&& other) noexcept
	{
		this->columns = other.columns;
		this->printer = other.printer;
	}

	Row& operator=(const Row& other) noexcept
	{
		this->columns = other.columns;
		this->printer = other.printer;

		return *this;
	}

	Row& operator=(Row&& other) noexcept
	{
		this->columns = other.columns;
		this->printer = other.printer;

		return *this;
	}

	~Row()
	{
	}

	static std::shared_ptr<PrimitiveValue> Make(std::shared_ptr<Node> node);

	void toString()
	{
		auto it = columns.begin();
		auto end = columns.end();

		uint32_t pos = 0;

		for (; it != end; ++it)
		{
			std::shared_ptr<PrimitiveValue> chunk = (*it);
			std::shared_ptr<Printer> print = printer.at(pos);
			print->Print(std::cout, std::make_shared<Primitive>(*chunk->chunk));

			pos++;
		}
	}

	void getType(uint32_t column, void* value)
	{
		std::shared_ptr<Printer> print = printer.at(column);
		std::shared_ptr<PrimitiveValue> chunk = columns.at(column);

		print->getTyped(*(chunk.get()->chunk), value);
	}

	PrimitiveValue* get(uint32_t column)
	{
		std::shared_ptr<PrimitiveValue> chunk = columns.at(column);
		return chunk.get();
	}

	bool operator==(const Row& row) const
	{
		bool val = true;

		if (this->columns.size() != row.columns.size())
		{
			val = false;
		}
		else
		{
			auto it = row.columns.begin();
			auto end = row.columns.end();

			uint32_t pos = 0;

			for (; it != end; ++it)
			{
				std::shared_ptr<PrimitiveValue> chunkLhs = this->columns.at(
						pos);
				std::shared_ptr<PrimitiveValue> chunkRhs = row.columns.at(pos);
				if (*(chunkLhs.get()->chunk) != *(chunkRhs.get()->chunk))
				{
					val = false;
					break;
				}

				pos++;
			}
		}

		return val;
	}
};

namespace std
{

template<> struct hash<Row>
{
	typedef Row argument_type;
	typedef std::size_t result_type;

	result_type operator()(argument_type const& row) const
	{
        uint32_t _hash = 0;

		auto it = row.columns.begin();
		auto end = row.columns.end();
		for (; it != end; ++it)
		{
            _hash += (*it)->chunk->hash();
		}

        return _hash;
	}
};
}

#endif /* SRC_ROW_H_ */
