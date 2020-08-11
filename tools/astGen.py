#!/usr/bin/env python3
import sys
import argparse
import os

class Field:
    def __init__(self, identifier, type):
        self.identifier = identifier
        self.type = type

    def label(self):
        return self.type.lower()

    def member_type(self):
        return "std::unique_ptr<const {}>".format(self.type)

    def __repr__(self):
        return '{{name: {}, type: {} }}'.format(self.identifier, self.type)

    def member_name(self):
        return "m_{}".format(str.lower(self.identifier))

class Class:
    def __init__(self, name, fields):
        self.name = name
        self.fields = fields

    def __repr__(self):
        return '{{name: {}, fields: {} }}'.format(self.name, self.fields)

    def visitor_function_name(self):
        return "visit{}".format(self.name)

    def reference_type(self):
        return "const {}&".format(self.name)


class ReturnType:
    def __init__(self, name, type):
        self.name = name
        self.type = type

    def field_name(self):
        return str.lower(self.name)




def parseFields(types):
    classes = []
    for type in types:
        print("Looking at class {}".format(type))
        classname = type.split(":")[0].strip()
        strfields = type.split(":")[1].strip()
        fields = []
        for field in strfields.split(", "):
            name = field.split(" ")[1]
            type = field.split(" ")[0]
            fields.append(Field(name, type))
        classes.append(Class(classname, fields))
    import pprint
    pprint.pprint(classes)

    return classes

def defineVisitor(f, basename, returntypes, types):
    def writeline(line):
        f.write(line + '\n')

    for returntype in returntypes:
        writeline("    class Visitor{} {{".format(returntype.name))
        writeline("    public:")
        for type in types:
            writeline(
                "       virtual {returntype} {visitor_function_name}({typename} {basename_lower}) const = 0;".
                format(returntype=returntype.type,
                       visitor_function_name=type.visitor_function_name(),
                       typename=type.reference_type(),
                       basename_lower=basename.lower()))

        writeline("    };")
        writeline(
            "    virtual {} accept(const Visitor{}&) const = 0;".format(returntype.type, returntype.name))


def defineType(f, basename, type, returntypes):
    def writeline(line):
        f.write(line + '\n')

    writeline("class {} : public {} {{".format(type.name, basename))
    writeline("public:")

    for returntype in returntypes:
        writeline("    virtual {type} accept(const Visitor{name} &visitor) const override {{".format(type=returntype.type, name=returntype.name))
        writeline("        return visitor.{}(*this);".format(type.visitor_function_name()))
        writeline("    }")

    typed_fields = ""
    for field in type.fields:
        typed_fields = typed_fields + "{type} &&{name},".format(type=field.member_type(),name=field.identifier)
    if typed_fields[-1] == ',':
        typed_fields = typed_fields[:-1]

    writeline("   {}({}):".format(type.name, typed_fields))
    for field in type.fields:
        if field is type.fields[-1]:
            lineending = '{}'
        else:
            lineending = ','
        writeline("    {}(std::move({})){}".format(field.member_name(), field.identifier, lineending))

    # Virtual destructor
    writeline("   virtual ~{}() = default;".format(type.name))

    for field in type.fields:
        writeline("   virtual const {type} &{identifier}() const{{ return *({member_name}.get()); }}".format(type=field.type, identifier=field.identifier, member_name=field.member_name()))

    writeline("private:")

    for field in type.fields:
        writeline("    {type} {name};".format(type=field.member_type(), name=field.member_name()))

    writeline("};")


def defineAst(outputdir, filename, basename, types):
    def writeline(line):
        f.write(line + '\n')

    path = os.path.join(outputdir, filename + ".hpp")
    with open(path, 'w') as f:
        writeline("#pragma once")
        writeline("#include \"literal.hpp\"")
        writeline("#include \"token.hpp\"")
        writeline("#include <memory>")
        writeline("")
        writeline("namespace KeegMake {")
        writeline("")
        for type in types:
            writeline("class {};".format(type.name))

        writeline("")
        writeline("class {}{{".format(basename))
        writeline("public:")

        returntypes = [ReturnType("String","std::string")]
        defineVisitor(f, basename, returntypes, types)

        writeline("    virtual ~{}() = default;".format(basename))

        writeline("")
        writeline("};")

        for type in types:
            defineType(f, basename, type, returntypes)

        writeline("} //namespace KeegMake")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output_directory')
    args = parser.parse_args()
    print("Output directory is {}".format(args.output_directory))
    types = [
        "Binary : Expression left, Token token, Expression right",
        "Grouping : Expression expression", "Literal : LiteralVal value",
        "Unary : Token token, Expression right"
    ]
    parsed_types = parseFields(types)
    defineAst(args.output_directory, 'expression_ast', 'Expression', parsed_types)
    return 0


if __name__ == "__main__":
    sys.exit(main())
