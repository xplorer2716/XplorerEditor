# AGNOS Process — Claude Code Entry Point

@.github/instructions/agnos-sw-eng.v1.instructions.md

## Tool Mapping (Claude Code)

The imported instruction file is written to be tool-neutral, but two Claude Code specifics are
worth calling out explicitly :

- **User questions**: where the process says `askQuestion` / `AskUserQuestion`, use the
  `AskUserQuestion` tool.
- **Git workflow skill**: use the skill named `agnos-git-workflow`. Under Claude Code it is
  discovered at `.claude/skills/agnos-git-workflow/SKILL.md`, which points to the canonical
  procedure at `.github/skills/agnos-git-workflow/SKILL.md` — read and follow that procedure.
