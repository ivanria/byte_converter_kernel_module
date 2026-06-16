#!/bin/bash

# Output bits
declare -r BC_OUT_HEX=1
declare -r BC_OUT_DEC=2
declare -r BC_OUT_OCT=4
declare -r BC_OUT_BIN=8
declare -r BC_OUT_ASCII=16
declare -r BC_OUT_RAW=32
declare -r BC_OUT_INFO=64
declare -r BC_OUT_LITTLE=128
declare -r BC_OUT_BIG=0
declare -r BC_OUT_GROUP_1=0		# bit 9-10
declare -r BC_OUT_GROUP_2=256
declare -r BC_OUT_GROUP_4=512
declare -r BC_OUT_GROUP_8=768		# bit 9-10

# Input bits
declare -r BC_IN_HEX=1024
declare -r BC_IN_DEC=2048
declare -r BC_IN_OCT=4096
declare -r BC_IN_BIN=8192
declare -r BC_IN_RAW=16384

function assemble_mask()
{
	local mask=0
	local endianness_flag=0
	local group_flag=0
	local in_flag=0
	local out_required_flag=0

	declare -A out_modes

	if [[ -z "$1" ]]; then
		error_exit "No parameters were passed"
	fi

	for arg
	do
		case "$arg" in
			-h|--help)
				print_help
				;;
			BC_OUT_HEX|BC_OUT_DEC|BC_OUT_OCT|BC_OUT_BIN|BC_OUT_ASCII|BC_OUT_RAW)
				(( out_modes[$arg]++ ))
				if (( out_modes[$arg] > 1 )); then
					error_exit "The option $arg was passed more than once"
				fi
				(( out_required_flag++ ))
				mask=$(( mask + ${!arg} ))
				;;
			BC_OUT_INFO)
				(( out_modes[$arg]++ ))
				if (( out_modes[$arg] > 1 )); then
					error_exit "The option $arg was passed more than once"
				fi
				mask=$(( mask + ${!arg} ))
				;;
			BC_OUT_LITTLE|BC_OUT_BIG)
				(( endianness_flag++ ))
				if (( endianness_flag > 1 )); then
					error_exit "There can only be one Endianness flag: BC_OUT_LITTLE | BC_OUT_BIG"
				fi
				mask=$(( mask + ${!arg} ))
				;;
			BC_OUT_GROUP_1|BC_OUT_GROUP_2|BC_OUT_GROUP_4|BC_OUT_GROUP_8)
				(( group_flag++ ))
				if (( group_flag > 1 )); then
					error_exit "There can only be one group flag: BC_OUT_GROUP_1|BC_OUT_GROUP_2|BC_OUT_GROUP_4|BC_OUT_GROUP_8"
				fi
				mask=$(( mask + ${!arg} ))
				;;
			BC_IN_HEX|BC_IN_DEC|BC_IN_OCT|BC_IN_BIN|BC_IN_RAW)
				(( in_flag++ ))
				if (( in_flag > 1 )); then
					error_exit "Only one input mode (BC_IN_*) can be passed"
				fi
				mask=$(( mask + ${!arg} ))
				;;
			*)
				error_exit "Unknown argument: $arg"
				;;
		esac
	done
	if (( out_required_flag == 0 )); then
		error_exit "At least one output mode must be specified (HEX, DEC, OCT, BIN, ASCII, RAW)"
	fi

	if (( in_flag == 0 )); then
		error_exit "Strictly one input mode (BC_IN_*) must be specified"
	fi

	echo "$mask"
	exit 0
}

function error_exit() {
	echo "Error: $1" >&2
	echo "Use: '$0 --help' for more information." >&2
	exit 1
}

function print_help() {
	echo "Usage: $0 [options] <OUT_MODE> <IN_MODE> [modifiers]"
	echo ""
	echo "Required Output Modes (at least one is required):"
	echo "  BC_OUT_HEX     Output in Hexadecimal format"
	echo "  BC_OUT_DEC     Output in Decimal format"
	echo "  BC_OUT_OCT     Output in Octal format"
	echo "  BC_OUT_BIN     Output in Binary format"
	echo "  BC_OUT_ASCII   Output in ASCII text format"
	echo "  BC_OUT_RAW     Output in Raw binary bytes"
	echo ""
	echo "Required Input Modes (strictly choose only one):"
	echo "  {BC_IN_HEX | BC_IN_DEC | BC_IN_OCT | BC_IN_BIN | BC_IN_RAW}"
	echo ""
	echo "Optional Output Modifiers (can be combined):"
	echo "  BC_OUT_INFO    Print additional debug/verbose information"
	echo "  [{BC_OUT_LITTLE | BC_OUT_BIG}]"
	echo "                 Endianness selection (default: BC_OUT_BIG)"
	echo "  [{BC_OUT_GROUP_1 | BC_OUT_GROUP_2 | BC_OUT_GROUP_4 | BC_OUT_GROUP_8}]"
	echo "                 Data grouping size in bytes (default: BC_OUT_GROUP_1)"
	echo ""
	echo "System Options:"
	echo "  -h, --help     Display this help and exit"
	exit 0
}

assemble_mask "$@"

