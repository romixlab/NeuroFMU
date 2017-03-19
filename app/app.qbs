import qbs
import STM32F4
import qbs.FileInfo
import qbs.ModUtils

Project {
    STM32F4 {
        Depends { name: "distortos" }
        cpp.includePaths: [
            "../output/include",
            "../distortos/include",
            "../distortos/source/chip/STM32/include/",
            "../distortos/external/CMSIS/",
            "../distortos/external/CMSIS-STM32F4/"
        ]

        Group {
            name: "neurofmu"
            prefix: "./"
            files: ["*.cpp", "*.hpp"]
        }
    }
}
