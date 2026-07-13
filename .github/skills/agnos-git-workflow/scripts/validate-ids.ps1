<#
.SYNOPSIS
  Validates AGNOS artifact identifiers before git operations (RQ-GIT-001, RQ-GIT-002).

.PARAMETER Type
  The type of ID to validate: 'TRI' | 'TASK' | 'ADR'.

.PARAMETER Value
  The value to validate.

.OUTPUTS
  Writes a human-readable error to stderr and exits 1 on failure.
  Exits 0 on success (no output).
#>
param(
    [Parameter(Mandatory)][ValidateSet('TRI','TASK','ADR')][string]$Type,
    [Parameter(Mandatory)][string]$Value
)

$patterns = @{
    TRI  = '^[A-Z]{3}$'
    TASK = '^TASK-[A-Z]{3}-\d{3}$'
    ADR  = '^ADR-[A-Z]{3}-\d{3}$'
}

$examples = @{
    TRI  = 'USR'
    TASK = 'TASK-USR-001'
    ADR  = 'ADR-USR-001'
}

if ($Value -cnotmatch $patterns[$Type]) {
    Write-Error "Invalid $Type '$Value'. Expected format: $($examples[$Type])"
    exit 1
}

exit 0
