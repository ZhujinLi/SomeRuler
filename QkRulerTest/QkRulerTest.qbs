import qbs

CppApplication {
    Depends { name: "Qt.testlib" }
    consoleApplication: true
    files: [
        "../geometrycalculator.cpp",
        "tst_geometrycalculator.cpp",
    ]

}
