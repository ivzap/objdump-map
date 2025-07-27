# objdump-map
**Header only** objdump parsing for debugging

![Build status](https://github.com/ivzap/objdump-map/actions/workflows/test.yml/badge.svg)
![ELF](https://img.shields.io/badge/Supported-ELF64--x86_64-blue)


## Summary
Made for those who want to parse ```objdump --dwarf=decodedline <binary>``` dumps. A data structure called a `ObjDumpAddressMap` is made to parse such dumps and store the contents in memory for quick access. It can be treated as your typical ```std::map``` as if it were storing <address, `AddressInfo`> pairs. `AddressInfo` is a struct that stores a <line number, fileID> pair.

## Use case(s)
Originally, I wanted to write a tool that could measure thread contention in realtime and show you where those threads were located (relative to your code) but got blocked on translating a stack frame return address to a source code line number. If we can do `dump_addr = ret_addr - strt_addr + dump_offset` then we can perform a lookup on the decoded line dump and find the source file location. 
