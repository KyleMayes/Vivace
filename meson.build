project('vivace', 'cpp')

# Flags

dependencies = []

add_project_arguments('-std=c++1z', '-Wall', '-Wextra', '-pedantic', language : 'cpp')

if get_option('use_libcxx')
    add_project_arguments('-stdlib=libc++', language : 'cpp')
    dependencies += meson.get_compiler('cpp').find_library('c++')
    dependencies += meson.get_compiler('cpp').find_library('cxxrt')
endif

# Library

headers = [include_directories('headers')]
sources = [
    'sources/utility.cpp',
]

libraries = [static_library('vce', sources, include_directories : headers)]

# Dependencies

options = ['use_libcxx=@0@'.format(get_option('use_libcxx'))]
accel = subproject('accelerando', default_options : options)
headers += accel.get_variable('headers')
libraries += accel.get_variable('accel')

# Benchmarks

benchmarks = [
]

if get_option('benchmarks')
    foreach benchmark : benchmarks
        name = 'B@0@'.format(benchmark)
        source = 'benchmarks/@0@.cpp'.format(benchmark)
        exe = executable(name, source,
            include_directories : headers,
            link_with : libraries,
            dependencies : dependencies)
        benchmark(name, exe)
    endforeach
endif

# Tests

tests = [
    'meta',
    'option',
    'result',
    'utility',
]

if get_option('tests')
    foreach test : tests
        name = 'T@0@'.format(test)
        source = 'tests/@0@.cpp'.format(test)
        exe = executable(name, source,
            include_directories : headers,
            link_with : libraries,
            dependencies : dependencies)
        test(name, exe)
    endforeach
endif
