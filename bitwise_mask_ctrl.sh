#!/bin/bash

# --- BIT DEFINITIONS (Base values) ---

# Output bits (0-5)
export BC_OUT_HEX=1        # Bit 0
export BC_OUT_DEC=2        # Bit 1
export BC_OUT_OCTAL=4      # Bit 2
export BC_OUT_BIN=8        # Bit 3
export BC_OUT_ASCII=16     # Bit 4
export BC_OUT_RAW=32       # Bit 5

# Input bits (6-10)
export BC_IN_HEX=64        # Bit 6
export BC_IN_DEC=128       # Bit 7
export BC_IN_OCTAL=256     # Bit 8
export BC_IN_BIN=512       # Bit 9
export BC_IN_RAW=1024      # Bit 10

# Control bits (15)
export BC_OP_REMOVE=16384           # Bit 14 (0x4000) #not real bit in mask
export BC_MODE_INCREMENTAL=32768    # Bit 15 (0x8000)

# --- HELPER FUNCTION ---
# Usage: bc_set [rem/set/add] [bit_name]
# Example: bc_set add OUT_BIN
function bc_set() {
    local op=$1
    local bit_name="BC_$2"
    local bit_val=${!bit_name}
    local final_val=0

    if [ -z "$bit_val" ]; then
        echo "Error: Unknown bit $2"
        return 1
    fi

    case $op in
        add)
            final_val=$((BC_MODE_INCREMENTAL + bit_val))
            ;;
        rem)
            final_val=$((BC_MODE_INCREMENTAL + BC_OP_REMOVE + bit_val))
            ;;
        set)
            final_val=$bit_val
            ;;
        *)
            echo "Usage: bc_set [add|rem|set] [BIT_NAME_WITHOUT_BC_PREFIX]"
            return 1
            ;;
    esac

    echo "$final_val" | sudo tee /proc/byte_conv_mask
    echo "Sent mask: $final_val (Op: $op, Bit: $2)"
}
