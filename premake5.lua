local assemblerGeneratedDir = "build/ember-assembler/generated/%{cfg.buildcfg}"
local assemblerGeneratedMakeDir = "generated/%{cfg.buildcfg}"

workspace "ember"
    configurations { "Debug", "Release" }
    location "build"
    cdialect "C17"

    filter "system:windows"
        platforms { "x64", "ARM64" }

    filter "platforms:x64"
        architecture "x86_64"

    filter "platforms:ARM64"
        architecture "ARM64"

    filter "configurations:Debug"
        defines {
            "DEBUG",
            "EMBER_BUILD_FLAVOR=\"Debug\""
        }
        symbols "On"

    filter "configurations:Release"
        defines {
            "NDEBUG",
            "EMBER_BUILD_FLAVOR=\"Release\""
        }
        optimize "On"

    filter "system:windows"
        defines {
            "_CRT_SECURE_NO_WARNINGS",
            "YY_NO_UNISTD_H"
        }

    filter {}

project "ember-cli"
    kind "ConsoleApp"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"
    location "build/ember-cli"

    files {
        "src/cli/**.c",
        "src/cli/**.h",
    }

    includedirs {
        "include",
        "src/cli/internal",
    }

    externalincludedirs {
        "third_party/vrg/dist"
    }

    links {
        "ember-assembler",
        "ember-vm",
        "ember-core"
    }

project "ember-core"
    kind "StaticLib"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"
    location "build/ember-core"

    files {
        "src/core/**.c",
        "src/core/**.h",
    }

    includedirs {
        "include",
        "src",
    }

project "ember-assembler"
    kind "StaticLib"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"
    location "build/ember-assembler"

    files {
        "src/assembler/**.c",
        "src/assembler/**.h",
        assemblerGeneratedDir .. "/parser.tab.c",
        assemblerGeneratedDir .. "/lexer.yy.c"
    }

    includedirs {
        "include",
        "src",
        assemblerGeneratedDir,
    }

    filter "system:windows"
        prebuildcommands {
            "{MKDIR} \"" .. assemblerGeneratedMakeDir .. "\"",
            "win_bison -d -o \"" .. assemblerGeneratedMakeDir .. "/parser.tab.c\" \"../../src/assembler/phases/parse/parser.y\"",
            "win_flex -o \"" .. assemblerGeneratedMakeDir .. "/lexer.yy.c\" \"../../src/assembler/phases/parse/lexer.l\""
        }

    filter "system:not windows"
        prebuildcommands {
            "{MKDIR} \"" .. assemblerGeneratedMakeDir .. "\"",
            "bison -d -o \"" .. assemblerGeneratedMakeDir .. "/parser.tab.c\" \"../../src/assembler/phases/parse/parser.y\"",
            "flex -o \"" .. assemblerGeneratedMakeDir .. "/lexer.yy.c\" \"../../src/assembler/phases/parse/lexer.l\""
        }

    filter {}

    links {
        "ember-core"
    }

project "ember-vm"
    kind "StaticLib"
    language "C"
    targetdir "bin/%{cfg.buildcfg}"
    location "build/ember-vm"

    files {
        "src/vm/**.c",
        "src/vm/**.h",
    }

    includedirs {
        "include",
        "src",
    }

    links {
        "ember-core"
    }

project "ember_test"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    targetdir "bin/%{cfg.buildcfg}"
    location "build/ember-test"

    files {
        "tests/unit/**.cpp",
        "tests/unit/**.h",
        "third_party/googletest/googletest/src/gtest-all.cc",
        "third_party/googletest/googletest/src/gtest_main.cc",
    }

    includedirs {
        "include",
        "src",
        "third_party/googletest/googletest",
        "third_party/googletest/googletest/include",
    }

    filter "system:not windows"
        links {
            "pthread",
        }

    filter {}

    links {
        "ember-core",
    }
