#!/usr/bin/env python3
import sys
import argparse
import os
import datetime


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
    def __init__(self, name, fields, basename):
        self.name = name
        self.fields = fields
        self.basename = basename

    def __repr__(self):
        return '{{name: {}, fields: {} }}'.format(self.name, self.fields)

    def className(self):
        return '{}{}'.format(self.basename, self.name)

    def visitor_function_name(self):
        return "visit{}".format(self.className())

    def reference_type(self):
        return "const {}&".format(self.className())


class ReturnType:
    def __init__(self, name, type):
        self.name = name
        self.type = type

    def field_name(self):
        return str.lower(self.name)

    def qualifiers(self):
        if "void" in self.type:
            return "virtual"
        return "[[nodiscard]] virtual"


def parseFields(types, basename):
    classes = []
    for type in types:
        print("Looking at class {}".format(type))
        classname = type.split("|")[0].strip()
        strfields = type.split("|")[1].strip()
        fields = []
        for field in strfields.split(", "):
            name = field.split(" ")[1]
            type = field.split(" ")[0]
            fields.append(Field(name, type))
        classes.append(Class(classname, fields, basename))
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
            self.file.write("{}{}\n".format(
                self.indentlevel * self.indentspaces * ' ', line))
        else:
            self.file.write('\n')


def defineVisitor(w, basename, returntypes, types):
    for returntype in returntypes:
        w.write("class {}Visitor{} {{".format(basename, returntype.name))
        w.write("public:")
        w.increase()
        if "void" in returntype.type:
            qualifiers = "virtual"
        else:
            qualifiers = "[[nodiscard]] virtual"
        for type in types:

            w.write(
                "{qualifiers} {returntype} {visitor_function_name}({typename} {basename_lower}) const = 0;"
                .format(qualifiers=qualifiers,
                        returntype=returntype.type,
                        visitor_function_name=type.visitor_function_name(),
                        typename=type.reference_type(),
                        basename_lower=basename.lower()))

        w.decrease()
        w.write("};")
        w.write("")


def defineType(w, basename, type, returntypes):
    w.write("class {} : public {} {{".format(type.className(), basename))
    w.write("public:")
    w.increase()

    for returntype in returntypes:
        if "void" in returntype.type:
            qualifiers = ""
        else:
            qualifiers = "[[nodiscard]]"
        w.write(
            "{qualifiers} {type} accept(const {basename}Visitor{name} &visitor) const override {{"
            .format(basename=basename,
                    qualifiers=qualifiers,
                    type=returntype.type,
                    name=returntype.name))
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

    w.write("{}({}):".format(type.className(), typed_fields))
    w.increase()
    for field in type.fields:
        if field is type.fields[-1]:
            lineending = '{}'
        else:
            lineending = ','
        w.write("{}(std::move({})){}".format(field.member_name(),
                                             field.identifier, lineending))

    w.decrease()
    w.write("")
    # Virtual destructor
    w.write("~{}() override = default;".format(type.className()))

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


def defineAst(outputdir, fileheader, filename, basename, types, returntypes):
    path = os.path.join(outputdir, filename + ".hpp")
    with Writer(path) as w:
        for line in fileheader:
            w.write(line)
        w.write("")
        w.write("namespace KeegMake {")
        w.write("")
        for type in types:
            w.write("class {};".format(type.className()))
        w.write("")
        defineVisitor(w, basename, returntypes, types)
        w.write("")
        w.write("class {}{{".format(basename))
        w.write("public:")
        w.increase()
        w.write("virtual ~{}() = default;".format(basename))
        w.write("")
        for returntype in returntypes:
            if "void" in returntype.type:
                qualifiers = "virtual"
            else:
                qualifiers = "[[nodiscard]] virtual"
            w.write("{} {} accept(const {}Visitor{}&) const = 0;".format(
                qualifiers, returntype.type, basename, returntype.name))
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
        "Binary | Expression left, Token token, Expression right",
        "Grouping | Expression expression",
        "Literal | LiteralVal value",
        "Unary | Token token, Expression right",
        "Variable | Token name",
    ]
    # TODO: This is some really gross stuff going on here in the organization
    statement_types = [
        "Expression | Expression expression left",
        "Print | Expression expression",
        "Variable | Token name, Expression initializer",
    ]
    visitor_returns = [
        ReturnType("Void", "void"),
        ReturnType("String", "std::string"),
        ReturnType("LiteralVal", "std::unique_ptr<LiteralVal>")
    ]
    file_header = [
        "#pragma once",
        "// This file generated by tools/astGen.py at {}".format(
            datetime.datetime.now().isoformat()),
        "#include \"literal.hpp\"",
        "#include \"token.hpp\"",
        "#include <memory>",
    ]
    basename_expression = "Expression"
    basename_statement = "Statement"
    defineAst(args.output_directory, file_header, 'expression_ast',
              'Expression', parseFields(expression_types,
                                        basename_expression), visitor_returns)
    file_header.insert(3, '#include "expression_ast.hpp"')
    defineAst(args.output_directory, file_header, 'statement_ast', 'Statement',
              parseFields(statement_types, basename_statement),
              visitor_returns)
    return 0


if __name__ == "__main__":
    sys.exit(main())
