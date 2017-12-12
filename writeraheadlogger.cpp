/*
 * Copyright 2017 <copyright holder> <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "writeraheadlogger.h"

WriterAheadLogger::WriterAheadLogger(RawBuffer& _buffer, std::string _path) : buffer(_buffer), path(_path)
{
}
WriterAheadLogger::~WriterAheadLogger()
{
}
void WriterAheadLogger::write(uint32_t walCounter)
{
    uint32_t size = buffer.getSize();
    uint8_t* data = buffer.getBuffer();

    uint64_t compressed_size = snappy_max_compressed_length(size);
    uint8_t* compressed_data = new uint8_t[compressed_size];
    snappy_compress((const char*)data, size, (char*)compressed_data, &compressed_size);

    std::ofstream out;
    
    std::string local = path;
    
    out.open(local.append("/").append(std::to_string(walCounter)).append(".snappy.bin"), std::ofstream::binary);

    out.write((const char*)compressed_data, compressed_size);
    out.flush();

    out.close();

    delete[] compressed_data;
}
void WriterAheadLogger::read(uint32_t walCounter)
{
    uint32_t compressed_size;
    uint8_t* compressed_data = nullptr;

    std::ifstream in;
    
    std::string local = path;
    
    in.open(local.append("/").append(std::to_string(walCounter)).append(".snappy.bin"), std::ifstream::binary);
    
    in.seekg(0, std::ios::end);
    compressed_size = static_cast<uint32_t>(in.tellg());
    in.seekg(0, std::ios::beg);

    compressed_data = new uint8_t[compressed_size];
    in.read((char*)compressed_data, compressed_size);

    uint64_t uncompressed_size = 0;
    snappy_uncompressed_length((const char*)compressed_data, compressed_size, &uncompressed_size);

    uint8_t* data = new uint8_t[uncompressed_size];
    snappy_uncompress((const char*)compressed_data, compressed_size, (char*)data, &uncompressed_size);

    buffer.Append(data, static_cast<uint32_t>(uncompressed_size));

    delete[] compressed_data;
    delete[] data;

    in.close();
}




