# Digraph
Simple program that lets you find paths of specified length in huge graphs.
## Requirements
- Linux or Windows
- Processor capable of executing AVX2 instructions (most of modern processors support it, thus you shouldn't be concerned)

> [!IMPORTANT]
> Windows machines must have [Visual C++ Runtime](https://aka.ms/vc14/vc_redist.x64.exe) installed. 

## Usage
> [!NOTE]
> You might also drag and drop the description file onto the executable to run it.
### Windows
```
digraph.exe {graph_path}
```

### Linux
```
./digraph {graph_path}
```

## Graph Description JSON
Writing your own JSON is quite simple. Please take a look at [example1](/example_graphs/example1.json) or [example2](/example_graphs/example2.json).

## Used Open Source Projects
- [fmt](https://github.com/fmtlib/fmt) by Victor Zverovich and {fmt} contributors [MIT License]
- [argparse](https://github.com/p-ranav/argparse.git) by Pranav Srinivas Kumar [MIT License]
- [json](https://github.com/nlohmann/json) by Niels Lohmann [MIT License]
- [googletest](https://github.com/google/googletest) by Google Inc. [BSD-3-Clause License](https://github.com/google/googletest/blob/main/LICENSE)