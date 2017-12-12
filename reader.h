/*
 * reader.h
 *
 *  Created on: Sep 27, 2016
 *      Author: andrei
 */

#ifndef SRC_READER_H_
#define SRC_READER_H_

#include "types.h"
#include "column.h"
#include "table.h"
#include "rawbuffer.h"
#include "writeraheadlogger.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <netinet/in.h>

class PrimitiveReader
{
public:
    std::shared_ptr<Column> column;

    PrimitiveReader(std::shared_ptr<Column> _column) :
        column(_column)
    {
    }
    virtual ~PrimitiveReader()
    {
    }
    static std::shared_ptr<PrimitiveReader> Make(std::shared_ptr<Column> column);
    virtual uint32_t read(uint8_t* buff) = 0;
};

template<typename T>
class TypedPrimitiveReader: public PrimitiveReader
{
public:
    TypedPrimitiveReader(std::shared_ptr<Column> _column) :
        PrimitiveReader(_column)
    {
    }

    inline uint32_t read(uint8_t * buff)
    {
        uint32_t size = column->Append((uint8_t*) buff);

        return size;
    }
}
;

template<>
class TypedPrimitiveReader<ByteArrayType> : public PrimitiveReader
{
public:
    TypedPrimitiveReader(std::shared_ptr<Column> _column) :
        PrimitiveReader(_column)
    {
    }

    uint32_t read(uint8_t* buff)
    {
        uint32_t size = column->Append((uint8_t*) buff);

        return size;
    }
};

class RowReader
{
private:
    Table& table;
    std::vector<std::shared_ptr<PrimitiveReader>> reader;
public:

    RowReader(Table& _table) : table(_table)
    {
        uint32_t num_columns = table.num_columns();
        uint32_t i = 0;
        for (; i < num_columns; i++)
        {
            reader.push_back(PrimitiveReader::Make(table.getColumn(i)));
        }
    }
    std::vector<std::shared_ptr<PrimitiveReader>> getReaders() {
        return this->reader;
    }
    void read(std::ifstream& in)
    {
        in.seekg(0, std::ios::end);
        uint32_t file_size = static_cast<uint32_t>(in.tellg());
        in.seekg(0, std::ios::beg);

        std::cout << "file size = " << file_size << std::endl;

        uint8_t* buffer = new uint8_t[file_size];

        in.read((char*) buffer, file_size);

        uint32_t bytes_read = 0;
        while (bytes_read < file_size)
        {
            auto it = reader.begin();
            auto end = reader.end();
            for (; it != end; ++it)
            {
                std::shared_ptr<PrimitiveReader> read = (*it);
                bytes_read += read->read(&buffer[bytes_read]);
            }
        }

        delete[] buffer;
    }
};

class StringReader
{
public:
    std::shared_ptr<Column> column;
    StringReader(std::shared_ptr<Column> _column) :
        column(_column) {
    }
    virtual ~StringReader() {
    }
    static std::shared_ptr<StringReader> Make(std::shared_ptr<Column> column);
    virtual void read(RawBuffer& row, std::string* in) = 0;
};

template<typename T>
class TypedStringReader: public StringReader {
public:
    TypedStringReader(std::shared_ptr<Column> _column) :
        StringReader(_column) {
    }

    void read(RawBuffer& row, std::string* in) override {
        uint32_t size = type_traits<T::type_num>::value_byte_size;

        switch (T::type_num) {
        case Type::UINT8:
        case Type::INT8: {
            int8_t p = (int8_t) std::stoi(*in);
            row.Append(reinterpret_cast<uint8_t*>(&p), size);
        }
            break;
        case Type::UINT16:
        case Type::INT16: {
            short p = (short) std::stoi(*in);
            row.Append(reinterpret_cast<uint8_t*>(&p), size);
        }
            break;
        case Type::UINT32:
        case Type::INT32: {
            int p = (int) std::stoi(*in);
            row.Append(reinterpret_cast<uint8_t*>(&p), size);
        }
            break;
        case Type::UINT64:
        case Type::INT64: {
            long p = (long) std::stol(*in);
            row.Append(reinterpret_cast<uint8_t*>(&p), size);
        }
            break;
        case Type::FLOAT: {
            float p = (float) std::stof(*in);
            row.Append(reinterpret_cast<uint8_t*>(&p), size);
        }

            break;
        case Type::DOUBLE: {
            double p = (double) std::stod(*in);
            row.Append(reinterpret_cast<uint8_t*>(&p), size);
        }
            break;
        default:
            break;
        }
    }
};

template<>
class TypedStringReader<ByteArrayType> : public StringReader {
public:
    TypedStringReader(std::shared_ptr<Column> _column) :
        StringReader(_column) {
    }

    void read(RawBuffer& row, std::string* in) override {
        uint32_t size = static_cast<uint32_t>(in->size());

        row.Append(reinterpret_cast<uint8_t*>(&size), sizeof(uint32_t));
        row.Append((uint8_t*) in->data(), size);
    }
};

class CsvReader
{
private:
    Table& table;
    std::vector<std::shared_ptr<StringReader>> reader;
public:
    CsvReader(Table& _table) :
        table(_table) {
        uint32_t num_columns = table.num_columns();
        for (uint32_t i = 0; i < num_columns; i++) {
            reader.push_back(StringReader::Make(table.getColumn(i)));
        }
    }
    std::vector<std::shared_ptr<StringReader>> getReaders() {
        return this->reader;
    }
    void read(std::ifstream& in) {
        RowReader rowReader(this->table);

        RawBuffer buff(64 * 1024 * 1024);
        RawBuffer row(1024 * 1024);

        std::vector<std::shared_ptr<PrimitiveReader>> rowReaders = rowReader.getReaders();

        std::string line;

        std::string token;

        std::stringstream lineStream;

        while (true) {
            std::getline(in, line);

            if (in.eof()) {
                break;
            }

            lineStream.seekg(0);
            lineStream.str(line);

            for (auto it = reader.begin(); it != reader.end(); ++it) {
                std::shared_ptr<StringReader> read = (*it);
                std::getline(lineStream, token, ',');

//                token.erase(token.find_last_not_of("\"") + 1);
//                token.erase(0,1);

                read->read(row, &token);
            }

            if (true == buff.Append(row.getBuffer(), row.getSize())) {
                WriterAheadLogger wal(buff, "/home/andrei/Desktop");
                wal.write(table.walCounter);
                buff.resetSize();

                wal.read(table.walCounter);

                uint32_t bytes_read = 0;
                uint32_t buff_size = buff.getSize();
                while (bytes_read < buff_size)
                {
                    auto it = rowReaders.begin();
                    auto end = rowReaders.end();
                    for (; it != end; ++it)
                    {
                        std::shared_ptr<PrimitiveReader> read = (*it);
                        bytes_read += read->read(&buff.getBuffer()[bytes_read]);
                    }
                }
                table.walCounter++;

                buff.resetSize();
                buff.Append(row.getBuffer(), row.getSize());
            }

            row.resetSize();
        }

        WriterAheadLogger wal(buff, "/home/andrei/Desktop");
        wal.write(table.walCounter);
        buff.resetSize();

        wal.read(table.walCounter);

        uint32_t bytes_read = 0;
        uint32_t buff_size = buff.getSize();
        while (bytes_read < buff_size)
        {
            auto it = rowReaders.begin();
            auto end = rowReaders.end();
            for (; it != end; ++it)
            {
                std::shared_ptr<PrimitiveReader> read = (*it);
                bytes_read += read->read(&buff.getBuffer()[bytes_read]);
            }
        }

        table.walCounter++;

        buff.resetSize();
        row.resetSize();
    }
};

#endif /* SRC_READER_H_ */
