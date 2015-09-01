#! /bin/sh
# This is intended to be run from "make check".
trap 'exit 1' 1 2 13 15
status=0
for form in $TEST_BINS; do
  "$RUBY" "$CHECK_RB" "$form" || status=1
done
exit $status
