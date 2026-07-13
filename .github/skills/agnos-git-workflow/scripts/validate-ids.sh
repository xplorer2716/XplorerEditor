#!/usr/bin/env bash
# Validates AGNOS artifact identifiers before git operations (RQ-GIT-001, RQ-GIT-002, RQ-PRT-005).
# Usage: validate-ids.sh -Type <TRI|TASK|ADR> -Value <value>
# Exits 0 on success (no output). Exits 1 with a human-readable error on stderr on failure.
set -u

usage() {
  echo "Usage: $0 -Type <TRI|TASK|ADR> -Value <value>" >&2
  exit 1
}

type=""
value=""

while [ $# -gt 0 ]; do
  case "$1" in
    -Type)
      type="$2"
      shift 2
      ;;
    -Value)
      value="$2"
      shift 2
      ;;
    *)
      usage
      ;;
  esac
done

if [ -z "$type" ] || [ -z "$value" ]; then
  usage
fi

case "$type" in
  TRI)
    pattern='^[A-Z]{3}$'
    example='USR'
    ;;
  TASK)
    pattern='^TASK-[A-Z]{3}-[0-9]{3}$'
    example='TASK-USR-001'
    ;;
  ADR)
    pattern='^ADR-[A-Z]{3}-[0-9]{3}$'
    example='ADR-USR-001'
    ;;
  *)
    echo "Invalid Type '$type'. Expected one of: TRI, TASK, ADR" >&2
    exit 1
    ;;
esac

if ! [[ "$value" =~ $pattern ]]; then
  echo "Invalid $type '$value'. Expected format: $example" >&2
  exit 1
fi

exit 0
