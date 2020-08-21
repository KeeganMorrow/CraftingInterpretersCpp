#!/usr/bin/env python3
import sys
import argparse
import abc
import enum
import os


class Writer(abc.ABC):
    def __init__(self, indentspaces=4):
        self.file = None
        self.indentlevel = 0
        self.indentspaces = 4

    def open(self):
        pass

    def close(self):
        pass

    def _do_write(self, line):
        pass

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

    def write(self, line=""):
        if line:
            self._do_write("{}{}\n".format(
                self.indentlevel * self.indentspaces * ' ', line))
        else:
            self._do_write('\n')


class FileWriter(Writer):
    def __init__(self, filepath, indentspaces=4):
        super().__init__(indentspaces)
        self.filepath = filepath
        self.file = None

    def open(self):
        self.file = open(self.filepath, 'w')

    def close(self):
        if self.file:
            self.file.close()

    def _do_write(self, line):
        self.file.write(line)


class StdoutWriter(Writer):
    def _do_write(self, line):
        print(line, end='')


class ValType(enum.Enum):
    VALUE = 1
    REFERENCE = 2
    AST_NODE = 3
    STATEMENT_VEC = 3


class MemberVariable:
    def __init__(self, name, type, val_type):
        self.name = name
        self.type = type
        self.val_type = val_type

    @property
    def membername(self):
        return f"m_{self.name}".format().lower()

    @property
    def localname(self):
        return f"{self.name}".format().lower()

    @property
    def gettername(self):
        return f"get{self.name}".format()


class AstVisitor:
    def __init__(self, base, name, ret):
        self.base = base
        self.name = name
        self.ret = ret

    @property
    def classname(self):
        return f"{self.base.classname}Visitor{self.name}".format()


class AstBase:
    def __init__(self, classname):
        self.classname = classname
        self.inherited = []
        self.visitors = []

    def addInherited(self, name, members, copyable=True):
        self.inherited.append(AstInherited(self, name, members, copyable))

    def addVisitor(self, name, ret):
        self.visitors.append(AstVisitor(self, name, ret))


class AstInherited:
    def __init__(self, base, name, members, copyable):
        self.base = base
        self.name = name
        self.members = members
        self.copyable = copyable

    @property
    def classname(self):
        return "{}{}".format(self.base.classname, self.name)

    @property
    def visitmethodname(self):
        return f"visit{self.classname}".format()


def declare_inherited_prototypes(w, base):
    for inh in base.inherited:
        w.write(f"class {inh.classname};".format())


def declare_visitors(w, base):
    for v in base.visitors:
        w.write(f"class {v.classname}".format() + "{")
        w.write("public:")
        w.increase()
        for inh in base.inherited:
            w.write(
                    f"virtual {v.ret} visit{inh.classname}({inh.classname}&) = 0;".
                    format())
            w.decrease()
        w.write("};")


def declare_inherited(w, base):
    def define_constructor():
        args = ""
        for m in inh.members:
            if m.val_type == ValType.VALUE:
                args = args + f"{m.type} {m.localname}"
            elif m.val_type == ValType.REFERENCE:
                args = args + f"{m.type} &{m.localname}"
            elif m.val_type == ValType.AST_NODE:
                args = args + f"std::unique_ptr<{m.type}> &&{m.localname}"
            elif m.val_type == ValType.STATEMENT_VEC:
                args = args + f"{m.type} &&{m.localname}"
            if not m is inh.members[-1]:
                args = args + ', '
        # Don't use these constructors for implicit conversions
        if (len(inh.members) == 1):
            qualifiers = "explicit "
        else:
            qualifiers = ""
        w.write(f"{qualifiers}{inh.classname}({args}):".format())
        w.increase()
        for m in inh.members:
            if not m is inh.members[-1]:
                lineend = ','
            else:
                lineend = '{}'
            if m.val_type == ValType.REFERENCE:
                w.write(f"{m.membername}({m.localname})".format() + lineend)
            elif m.val_type == ValType.AST_NODE or m.val_type == ValType.VALUE or m.val_type == ValType.STATEMENT_VEC:
                w.write(f"{m.membername}(std::move({m.localname}))".format() +
                        lineend)
                w.decrease()

    def define_copy_constructor():
        if not inh.copyable:
            w.write(f"{inh.classname}(const {inh.classname}& other) = delete;".format())
            return
        w.write(f"{inh.classname}(const {inh.classname}& other)".format() +
                ":")
        w.increase()
        for m in inh.members:
            if not m is inh.members[-1]:
                lineend = ','
            else:
                lineend = '{}'

            if m.val_type == ValType.VALUE or m.val_type == ValType.REFERENCE:
                w.write(f"{m.membername}(other.{m.membername})".format() +
                        lineend)
            elif m.val_type == ValType.AST_NODE:
                w.write(
                    f"{m.membername}(other.{m.membername}->clone())".format() +
                    lineend)
        w.decrease()

    def define_clone():
        w.write("// Method for allowing polymorphic copy")
        w.write(
            f"std::unique_ptr<{base.classname}> clone() override".format() +
            "{")
        w.increase()
        # TODO: Temporary hack - Should probably actually implement this
        if not inh.copyable:
            w.write("return nullptr;");
            w.decrease()
            w.write("}")
            return
        args = ""
        for m in inh.members:
            if m.val_type == ValType.REFERENCE or m.val_type == ValType.VALUE:
                args = args + f"{m.membername}"
            if m.val_type == ValType.AST_NODE:
                args = args + f"std::move({m.membername})"
            if not m is inh.members[-1]:
                args = args + ', '
        w.write(f"return std::make_unique<{inh.classname}>({args});")
        w.decrease()
        w.write("}")

    def define_accepts():
        w.write("// Visitor accept methods")
        for v in base.visitors:
            w.write(
                f"{v.ret} accept({v.classname}& visitor) override".format() +
                "{")
            w.increase()
            w.write(f"return visitor.{inh.visitmethodname}(*this);".format())
            w.decrease()
            w.write("}")

    def define_accessors():
        w.write("// Accessor functions")
        for m in inh.members:
            if (m.val_type == ValType.REFERENCE):
                rtype = f"{m.type}&".format()
                rexpr = m.membername
            elif (m.val_type == ValType.VALUE):
                rtype = m.type
                rexpr = m.membername
            elif (m.val_type == ValType.AST_NODE):
                rtype = f"{m.type}*".format()
                rexpr = f"{m.membername}.get()".format()
            w.write(f"{rtype} {m.gettername}()".format() + "{")
            w.increase()
            w.write(f"return {rexpr};".format())
            w.decrease()
            w.write("}")

    def define_member_vars():
        for mem in inh.members:
            if (mem.val_type == ValType.REFERENCE
                    or mem.val_type == ValType.VALUE):
                w.write(f"{mem.type} {mem.membername};".format())
            elif (mem.val_type == ValType.AST_NODE):
                w.write(
                    f"std::unique_ptr<{mem.type}> {mem.membername};".format())

    for inh in base.inherited:
        w.write(f"class {inh.classname} : public {base.classname}".format() +
                "{")
        w.write("public:")
        w.increase()
        w.write("// Constructors")
        define_constructor()
        define_copy_constructor()
        w.write()
        w.write("// Destructor")
        # Virtual destructor
        w.write(f"~{inh.classname}() override = default;".format())
        w.write()
        # Clone method
        define_clone()
        w.write()
        # Accept methods
        define_accepts()
        w.write()
        # Accessor methods
        define_accessors()
        w.write()
        w.decrease()
        w.write("private:")
        w.increase()
        # Member variables
        define_member_vars()
        w.decrease()
        w.write("};")


def declare_baseclass(w, base):
    w.write(f"class {base.classname}".format() + "{")
    w.write("public:")
    w.increase()
    w.write(f"{base.classname}() = default;".format())
    w.write(f"{base.classname}(const {base.classname}&) = delete;".format())
    w.write(f"virtual ~{base.classname}() = default;".format())
    w.write()
    w.write(f"virtual std::unique_ptr<{base.classname}> clone() = 0;".format())
    w.write()
    for v in base.visitors:
        w.write(f"virtual {v.ret} accept({v.classname}&) = 0;".format())
    w.decrease()
    w.write("};")


def file_header(w, includes, namespace):
    w.write("#pragma once")
    w.write()
    for i in includes:
        w.write("#include " + i)
    w.write()
    w.write("namespace {} {{".format(namespace))


def file_footer(w, namespace):
    w.write("}} //namespace {}".format(namespace))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output_directory')
    args = parser.parse_args()
    print("Output directory is {}".format(args.output_directory))

    expression_includes = [
        '"literal.hpp"', '"token.hpp"', '<memory>', '<utility>'
    ]

    # Set up the actual data we'll be using
    expression_base = AstBase('Expression')
    expression_base.addVisitor("LiteralVal", "std::unique_ptr<LiteralVal>")
    expression_base.addVisitor("String", "std::string")
    expression_base.addInherited('Assign', [
        MemberVariable('Name', 'Token', ValType.VALUE),
        MemberVariable('Value', 'Expression', ValType.AST_NODE)
    ])
    expression_base.addInherited('Binary', [
        MemberVariable('Left', 'Expression', ValType.AST_NODE),
        MemberVariable('Token', 'Token', ValType.VALUE),
        MemberVariable('Right', 'Expression', ValType.AST_NODE)
    ])
    expression_base.addInherited(
        'Grouping',
        [MemberVariable('Expression', 'Expression', ValType.AST_NODE)])
    expression_base.addInherited(
        'Literal', [MemberVariable('Value', 'LiteralVal', ValType.VALUE)])
    expression_base.addInherited('Logical', [
        MemberVariable('Left', 'Expression', ValType.AST_NODE),
        MemberVariable('Token', 'Token', ValType.VALUE),
        MemberVariable('Right', 'Expression', ValType.AST_NODE)
    ])
    expression_base.addInherited('Unary', [
        MemberVariable('Token', 'Token', ValType.VALUE),
        MemberVariable('Expression', 'Expression', ValType.AST_NODE)
    ])
    expression_base.addInherited(
        'Variable', [MemberVariable('Name', 'Token', ValType.VALUE)])

    with FileWriter(os.path.join(args.output_directory,
                                 "expression_ast.hpp")) as w:
        file_header(w, expression_includes, "lox")
        declare_inherited_prototypes(w, expression_base)
        w.write()
        declare_visitors(w, expression_base)
        w.write()
        declare_baseclass(w, expression_base)
        w.write()
        declare_inherited(w, expression_base)
        w.write()
        file_footer(w, "lox")

    statement_includes = [
        '"literal.hpp"', '"token.hpp"', '<memory>', '<utility>',
        '"expression_ast.hpp"'
    ]

    # Set up the actual data we'll be using
    statement_base = AstBase('Statement')
    statement_base.addVisitor("Void", "void")
    statement_base.addVisitor("String", "std::string")
    statement_base.addInherited('Block', [
        MemberVariable('Statements', 'std::vector<std::unique_ptr<Statement>>',
                       ValType.STATEMENT_VEC)
    ],
                                copyable=False)
    statement_base.addInherited(
        'Expression',
        [MemberVariable('Expression', 'Expression', ValType.AST_NODE)])
    statement_base.addInherited('If', [
        MemberVariable('Condition', 'Expression', ValType.AST_NODE),
        MemberVariable('thenBranch', 'Statement', ValType.AST_NODE),
        MemberVariable('elseBranch', 'Statement', ValType.AST_NODE),
    ])
    statement_base.addInherited(
        'Print',
        [MemberVariable('Expression', 'Expression', ValType.AST_NODE)])
    statement_base.addInherited('Variable', [
        MemberVariable('Name', 'Token', ValType.VALUE),
        MemberVariable('Initializer', 'Expression', ValType.AST_NODE),
    ])

    with FileWriter(os.path.join(args.output_directory,
                                 "statement_ast.hpp")) as w:
        file_header(w, statement_includes, "lox")
        declare_inherited_prototypes(w, statement_base)
        w.write()
        declare_visitors(w, statement_base)
        w.write()
        declare_baseclass(w, statement_base)
        w.write()
        declare_inherited(w, statement_base)
        w.write()
        file_footer(w, "lox")
    return 0


if __name__ == "__main__":
    sys.exit(main())
