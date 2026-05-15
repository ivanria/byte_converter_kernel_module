
# Byte Converter Kernel Module

[![Kernel Module Build](https://github.com/ivanria/byte_converter_kernel_module/actions/workflows/kernel_build.yml/badge.svg)](https://github.com/ivanria/byte_converter_kernel_module/actions/workflows/kernel_build.yml)

A versatile data transformation tool implemented as a Linux kernel module. It allows on-the-fly conversion of input data into various numeral systems and formatting styles via a character device interface.

## Key Features

*   **Character Device `/dev/byte_converter`**: Accepts raw data (up to 4096 bytes) and provides a formatted string output.
*   **Control via `/proc/byte_conv_mask`**: Dynamic mode switching for input and output using a bitmask.
*   **High Performance**: Conversion logic is based on fast bitwise shifts and arithmetic, minimizing the overhead of heavy kernel string formatting functions.
*   **Structured Output**: Data is grouped (4 or 8 bytes per line) with offset indicators, similar to classic hex-dump utilities.

## Supported Formats (Mask-controlled)

*   **BIN**: Per-byte binary representation with nibble separation.
*   **OCT**: Octal representation.
*   **DEC**: Decimal representation.
*   **HEX**: Hexadecimal format with leading zeros.
*   **RAW/ASCII**: Transparent data pass-through.

## Roadmap

- [ ] **Endianness Control**: Support for Big Endian and Little Endian transformations.
- [ ] **Word Size Selection**: Toggle processing for 2, 4, and 8-byte words.
- [ ] **Private Data Session Isolation**: Independent buffers for concurrent user sessions using `file->private_data`.
- [ ] **Extended Semantics**: Expanding the bitmask to control byte-swapping logic.

## Build and Installation

1. Build the module using the provided Makefile:
   ```bash
   make
   ```
2. Load the module into the kernel:
   ```bash
   sudo insmod byte_converter_mod.ko
   ```
3. Configure the mask (e.g., to enable Binary output):
   ```bash
   echo 0x08 | sudo tee /proc/byte_conv_mask
   ```
4. Use the device:
   ```bash
   echo "data" > /dev/byte_converter
   cat /dev/byte_converter
   ```

## ⚠️ Warning
Be cautious when reading non-ASCII formats directly in your terminal, as it may disrupt terminal state. It is recommended to redirect output to a file or use `cat -v`.
