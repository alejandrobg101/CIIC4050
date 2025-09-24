#!/bin/bash

# --- Configuration ---
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color
SCORE=0
TOTAL_TESTS=5

# --- Cleanup from previous runs ---
# rm -f parent_program geo_calc
# 
# --- Compilation ---
echo "--- Compiling the programs ---"
# # Compile geo_calc first
# gcc -Wall -Werror ../src/geo_calc.c -o geo_calc -lm
# if [ $? -ne 0 ]; then
#     echo -e "${RED}Compilation of geo_calc failed.${NC}"
#     exit 1
# fi
# Compile the parent program
gcc -Wall -Werror ../src/main.c -o parent_program
if [ $? -ne 0 ]; then
    echo -e "${RED}Compilation of parent_program failed.${NC}"
    exit 1
fi
echo -e "${GREEN}Compilation successful!${NC}"
echo "-------------------"

# --- Run the main program and capture output ---
echo "--- Running the parent program and capturing output ---"
# We run the parent and capture all its output, both stdout and stderr.
output=$(./parent_program 2>&1)
echo "$output"

# --- Test 1: Check for circle_area output ---
echo "--- Running Test 1: Validate circle_area output ---"
expected_output="78.539750"
actual_output=$(echo "$output" | grep "78.539750")
if [[ -n "$actual_output" ]]; then
    echo -e "${GREEN}Test 1 Passed: Found correct circle_area output.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 1 Failed: Expected output '$expected_output' not found.${NC}"
fi

# --- Test 2: Check for sphere_volume output ---
echo "--- Running Test 2: Validate sphere_volume output ---"
expected_output="65.449792"
actual_output=$(echo "$output" | grep "65.449792")
if [[ -n "$actual_output" ]]; then
    echo -e "${GREEN}Test 2 Passed: Found correct sphere_volume output.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 2 Failed: Expected output '$expected_output' not found.${NC}"
fi

# --- Test 3: Check for circle_perimeter output ---
echo "--- Running Test 3: Validate circle_perimeter output ---"
expected_output="62.831800"
actual_output=$(echo "$output" | grep "62.831800")
if [[ -n "$actual_output" ]]; then
    echo -e "${GREEN}Test 3 Passed: Found correct circle_perimeter output.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 3 Failed: Expected output '$expected_output' not found.${NC}"
fi

# --- Test 4: Validate Child Process Creation ---
echo "--- Running Test 4: Validate child process creation messages ---"
child_creation_count=$(echo "$output" | grep -c "is running")
if [ "$child_creation_count" -eq 3 ]; then
    echo -e "${GREEN}Test 4 Passed: Correct number of child creation messages found.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 4 Failed: Expected 3 child creation messages, but found $child_creation_count.${NC}"
fi

# --- Test 5: Validate Parent waited for all children ---
echo "--- Running Test 5: Validate Parent's final message ---"
if echo "$output" | grep -q "Parent process is finishing."; then
    echo -e "${GREEN}Test 5 Passed: Parent's final message was correctly printed.${NC}"
    SCORE=$((SCORE + 1))
else
    echo -e "${RED}Test 5 Failed: Parent's final message was not found.${NC}"
fi

# --- Final Score ---
echo "-------------------"
echo "Tests Passed: $SCORE / $TOTAL_TESTS"
FINAL_SCORE=$(( (SCORE * 100) / TOTAL_TESTS ))
echo -e "Final Grade: ${GREEN}${FINAL_SCORE}%${NC}"

# --- Cleanup ---
rm -f parent_program