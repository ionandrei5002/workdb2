/*
 * printer.h
 *
 *  Created on: Sep 27, 2016
 *      Author: andrei
 */

#ifndef SRC_PRINTER_H_
#define SRC_PRINTER_H_

#include "types.h"
#include "primitive.h"
#include "schema.h"

class Printer
{
protected:
    std::shared_ptr<Node> node;
public:

    Printer(std::shared_ptr<Node> _node) :
        node(_node)
    {
    }
    virtual ~Printer()
    {
    }
    static std::shared_ptr<Printer> Make(std::shared_ptr<Node> node);
    virtual void Print(std::ostream& out, std::shared_ptr<Primitive> value) = 0;
    virtual void getTyped(Primitive& value, void* val) = 0;
    virtual std::string getName() = 0;
};

template<typename T>
class TypedPrinter: public Printer
{
private:
    typedef typename T::c_type value_type;
public:

    TypedPrinter(std::shared_ptr<Node> _node) :
        Printer(_node)
    {
    }

    void Print(std::ostream& out, std::shared_ptr<Primitive> value) override
    {
        out << *((value_type*) value->values) << "\t";
    }

    void getTyped(Primitive& value, void* val) override
    {
        memcpy(val, (void*) value.values, value.bytes);
    }

    std::string getName() override
    {
        return node->getName();
    }
};

template<>
class TypedPrinter<ByteArrayType> : public Printer
{
private:
    typedef ByteArrayType::c_type value_type;
public:

    TypedPrinter(std::shared_ptr<Node> _node) :
        Printer(_node)
    {
    }

    void Print(std::ostream& out, std::shared_ptr<Primitive> value) override
    {
        out << value_type(value->values, value->bytes) << "\t";
    }

    void getTyped(Primitive& value, void* val) override
    {
        val = (void*) new value_type(value.values, value.bytes);
        val = val;
    }

    std::string getName() override
    {
        return node->getName();
    }
};

#endif /* SRC_PRINTER_H_ */
