import qbs

Module {
    name: "distortos"
    Depends { name: "cpp" }
    cpp.linkerFlags: [
        "-Wl,-L/home/roman/git/NeuroFMU/output",
        "-Wl,--whole-archive",
        "-ldistortos",
        "-Wl,--no-whole-archive"
    ]
}
