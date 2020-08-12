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

class Writer:

    def __init__(self, filepath, indentspaces=4):
        self.filepath = filepath
        self.file = None
        self.indentlevel = 0
        self.indentspaces = 4

    def open(self):
        self.file = open(self.filepath, 'w')

    def close(self):
        if self.file:
            self.file.close()

    def decrease(self):
        self.indentlevel = self.indentlevel - 1
        if self.indentlevel < 0:
            self.indentlevel = 0

    def increase(self):
        self.indentlevel = self.indentlevel + 1

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def write(self, line):
        if line:
            self.file.write("{}{}\n".format(self.indentlevel * self.indentspaces * ' ', line))
        else:
            self.file.write('\n')

def defineVisitor(w, basename, returntypes, types):
    for returntype in returntypes:
        w.write("class Visitor{} {{".format(returntype.name))
        w.write("public:")
        w.increase()
        for type in types:
            w.write(
                "[[nodiscard]] virtual {returntype} {visitor_function_name}({typename} {basename_lower}) const = 0;"
                .format(returntype=returntype.type,
                        visitor_function_name=type.visitor_function_name(),
                        typename=type.reference_type(),
                        basename_lower=basename.lower()))

        w.decrease()
        w.write("};")
        w.write("")
        w.write("[[nodiscard]] virtual {} accept(const Visitor{}&) const = 0;".format(
            returntype.type, returntype.name))
        w.write("")


def defineType(w, basename, type, returntypes):
    w.write("class {} : public {} {{".format(type.name, basename))
    w.write("public:")
    w.increase()

    for returntype in returntypes:
        w.write(
            "[[nodiscard]] {type} accept(const Visitor{name} &visitor) const override {{"
            .format(type=returntype.type, name=returntype.name))
        w.increase()
        w.write("return visitor.{}(*this);".format(
            type.visitor_function_name()))
        w.decrease()
        w.write("}")
        w.write("")

    typed_fields = ""
    for field in type.fields:
        typed_fields = typed_fields + "{type} &&{name},".format(
            type=field.member_type(), name=field.identifier)
    if typed_fields[-1] == ',':
        typed_fields = typed_fields[:-1]

    w.write("{}({}):".format(type.name, typed_fields))
    w.increase()
    for field in type.fields:
        if field is type.fields[-1]:
            lineending = '{}'
        else:
            lineending = ','
        w.write("{}(std::move({})){}".format(field.member_name(),
                                                   field.identifier,
                                                   lineending))

    w.decrease()
    w.write("")
    # Virtual destructor
    w.write("~{}() override = default;".format(type.name))

    for field in type.fields:
        w.write(
            "[[nodiscard]]virtual const {type} &{identifier}() const{{ return *({member_name}.get()); }}"
            .format(type=field.type,
                    identifier=field.identifier,
                    member_name=field.member_name()))
        w.write("")

    w.write("")
    w.write("private:")
    for field in type.fields:
        w.write("{type} {name};".format(type=field.member_type(),
                                              name=field.member_name()))

    w.decrease()
    w.write("};")


def defineAst(outputdir, filename, basename, types, returntypes):
    path = os.path.join(outputdir, filename + ".hpp")
    with Writer(path) as w:
        w.write("#pragma once")
        w.write("#include \"literal.hpp\"")
        w.write("#include \"token.hpp\"")
        w.write("#include <memory>")
        w.write("")
        w.write("namespace KeegMake {")
        w.write("")
        for type in types:
            w.write("class {};".format(type.name))

        w.write("")
        w.write("class {}{{".format(basename))
        w.write("public:")
        w.increase()

        defineVisitor(w, basename, returntypes, types)

        w.write("virtual ~{}() = default;".format(basename))

        w.write("")
        w.decrease()
        w.write("};")
        w.write("")

        for type in types:
            defineType(w, basename, type, returntypes)

        w.write("} //namespace KeegMake")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output_directory')
    args = parser.parse_args()
    print("Output directory is {}".format(args.output_directory))
    expression_types = [
        "Binary : Expression left, Token token, Expression right",
        "Grouping : Expression expression", "Literal : LiteralVal value",
        "Unary : Token token, Expression right"
    ]
    statement_types = [
        "Expression : Expression expression left",
        "Print : Expression expression",
    ]
    visitor_returns = [
        ReturnType("String", "std::string"),
        ReturnType("LiteralVal", "std::unique_ptr<LiteralVal>")
    ]
    defineAst(args.output_directory, 'expression_ast', 'Expression',
              parseFields(expression_types), visitor_returns)
    #defineAst(args.output_directory, 'statement_ast', 'Statement',
    #          parseFields(statement_types), visitor_returns)
    return 0


if __name__ == "__main__":
    sys.exit(main())
