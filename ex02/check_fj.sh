#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

RUNS="${RUNS:-100}"
LENGTHS_INPUT="${LENGTHS:-5 8 10 12 16 20 21 22 24 28 33 32 40 90 3000}"
read -r -a LENGTHS <<< "$LENGTHS_INPUT"

# Ford-Johnson worst-case comparisons:
# F(n) = sum_{k=1..n} ceil(log2(3k/4))
# Computed with integer arithmetic only.
ford_johnson_max() {
	local n="$1"
	local sum=0
	local k=1
	while (( k <= n )); do
		local e=0
		local lhs=4
		local rhs=$((3 * k))
		while (( lhs < rhs )); do
			lhs=$((lhs * 2))
			e=$((e + 1))
		done
		sum=$((sum + e))
		k=$((k + 1))
	done
	echo "$sum"
}

canonical_numbers() {
	sed -E 's/[^0-9]+/ /g' | xargs
}

make >/dev/null

total_fail=0

for n in "${LENGTHS[@]}"; do
	if (( n < 1 )); then
		echo "[SKIP] n=$n (must be >= 1)"
		continue
	fi

	max_cmp="$(ford_johnson_max "$n")"
	pass=0
	fail=0
	worst=0
	best=100000
	first_fail=""

	for ((run = 1; run <= RUNS; run++)); do
		nums=()

		if (( run == 1 )); then
			# Deterministic descending
			for ((v = n; v >= 1; v--)); do
				nums+=("$v")
			done
		elif (( run == 2 )); then
			# Deterministic ascending
			for ((v = 1; v <= n; v++)); do
				nums+=("$v")
			done
		elif (( run == 3 )); then
			# Deterministic zig-zag
			low=1
			high="$n"
			while (( low <= high )); do
				nums+=("$high")
				if (( low != high )); then
					nums+=("$low")
				fi
				low=$((low + 1))
				high=$((high - 1))
			done
		else
			# Random unique values
			mapfile -t nums < <(shuf -i 1-"$((n * 20))" -n "$n")
		fi

		if ! out="$(./PmergeMe "${nums[@]}" | tr -d '\r')"; then
			fail=$((fail + 1))
			total_fail=$((total_fail + 1))
			if [[ -z "$first_fail" ]]; then
				first_fail="run=$run execution-failed input=${nums[*]} output='$out'"
			fi
			continue
		fi

		before_line="$(printf '%s\n' "$out" | sed -n '/^[[:space:]]*Before[[:space:]]*:/Ip' | head -n1)"
		after_line="$(printf '%s\n' "$out" | sed -n '/^[[:space:]]*After[[:space:]]*:/Ip' | head -n1)"
		vec_timing_line="$(printf '%s\n' "$out" | grep -im1 'std::vector' || true)"
		deq_timing_line="$(printf '%s\n' "$out" | grep -im1 'std::deque' || true)"

		# Accept either one global comparison line or separate vector/deque ones.
		mapfile -t cmp_lines < <(printf '%s\n' "$out" | sed -n '/^[[:space:]]*[0-9][0-9]*[[:space:]]*$/p' | tr -d '[:space:]')

		expected_input="${nums[*]}"
		expected_sorted="$(printf '%s\n' "${nums[@]}" | sort -n | paste -sd' ' -)"
		printed_input="$(printf '%s\n' "$before_line" | canonical_numbers)"
		vec_sorted="$(printf '%s\n' "$after_line" | canonical_numbers)"

		if [[ -z "$before_line" || -z "$after_line" ]]; then
			fail=$((fail + 1))
			total_fail=$((total_fail + 1))
			if [[ -z "$first_fail" ]]; then
				first_fail="run=$run bad-format missing Before/After lines input=${nums[*]}"
			fi
			continue
		fi

		if [[ -z "$vec_timing_line" || -z "$deq_timing_line" ]]; then
			fail=$((fail + 1))
			total_fail=$((total_fail + 1))
			if [[ -z "$first_fail" ]]; then
				first_fail="run=$run bad-format timing vec='$vec_timing_line' deq='$deq_timing_line' input=${nums[*]}"
			fi
			continue
		fi

		if [[ "$printed_input" != "$expected_input" ]]; then
			fail=$((fail + 1))
			total_fail=$((total_fail + 1))
			if [[ -z "$first_fail" ]]; then
				first_fail="run=$run bad-input-line got='$printed_input' want='$expected_input'"
			fi
			continue
		fi

		if [[ "$vec_sorted" != "$expected_sorted" ]]; then
			fail=$((fail + 1))
			total_fail=$((total_fail + 1))
			if [[ -z "$first_fail" ]]; then
				first_fail="run=$run unsorted vec='$vec_sorted' want='$expected_sorted' input=${nums[*]}"
			fi
			continue
		fi

		if (( ${#cmp_lines[@]} == 0 || ${#cmp_lines[@]} > 2 )); then
			fail=$((fail + 1))
			total_fail=$((total_fail + 1))
			if [[ -z "$first_fail" ]]; then
				first_fail="run=$run invalid-comparisons lines='$(printf '%s|' "${cmp_lines[@]}")' input=${nums[*]}"
			fi
			continue
		fi

		cmp1="${cmp_lines[0]}"
		if (( ${#cmp_lines[@]} == 2 )); then
			cmp2="${cmp_lines[1]}"
		else
			cmp2="$cmp1"
		fi
		cmp="$cmp1"
		if (( cmp2 > cmp )); then
			cmp="$cmp2"
		fi

		if (( cmp > worst )); then
			worst="$cmp"
		fi

		if (( cmp < best)); then
			best="$cmp"
		fi

		if (( cmp1 <= max_cmp && cmp2 <= max_cmp )); then
			pass=$((pass + 1))
		else
			fail=$((fail + 1))
			total_fail=$((total_fail + 1))
			if [[ -z "$first_fail" ]]; then
				first_fail="run=$run vec_cmp=$cmp1 deq_cmp=$cmp2 max=$max_cmp input=${nums[*]}"
			fi
		fi
	done

	echo "n=$n max=$max_cmp pass=$pass fail=$fail worst_seen=$worst best_seen=$best"
	if [[ -n "$first_fail" ]]; then
		echo "  first_failure: $first_fail"
	fi
done

if (( total_fail > 0 )); then
	echo "RESULT: FAIL ($total_fail failing runs)"
	exit 1
fi

echo "RESULT: PASS"
