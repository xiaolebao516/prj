# Loop Contract

This file becomes the canonical contract only for an active Loop. A Loop Goal is the bounded result that Loop must continuously advance and eventually deliver. It may represent a short requirement set, a milestone, a feature, a bug fix, or any sustained task that needs state, feedback, verification, and iteration.

## Status

`active`

DOC-001 is the first active Standard Loop in this repository.

## Goal

`Establish the first real Standard Loop by auditing, classifying, migrating, and closing references for the project documentation system so that Agent control, Agent references, temporary work, and human documents have one clear authority model without information loss.`

## Boundary / Scope

- Audit and, where justified, modify documentation, `AGENTS.md`, `CLAUDE.md`, `.agent/LOOP.md`, `.agent/STATE.md`, `.agent/LOG.md`, and necessary documentation directories.
- Preserve business code, algorithms, parameters, protocols, timing, build/test scripts, `.gitignore`, `.claude/settings.local.json`, `tmp/`, and all other out-of-scope files.
- Read but do not modify, move, delete, stage, or format `docs/param_tuning_analysis.md`.
- Do not commit or push. Do not run Qt Build or calibration tests.

## Success Criteria

- `SC-1`: Agent control, Agent reference, temporary work, and human-document responsibilities are explicit, non-overlapping, and navigated precisely from `AGENTS.md` without recursive default loading.
- `SC-2`: Product specification and project progress occupy their approved Agent-reference and human-report authorities, with valid references and no product or report content loss.
- `SC-3`: Architecture, debugging, and calibration documents are kept, moved, split, or archived according to their actual audience and lifecycle; operational Agent conclusions and human research rationale are separated without duplicated long-form content.
- `SC-4`: `docs/param_tuning_analysis.md` retains its exact content hash, size, timestamp, pre-existing working-tree modification, and location, and is documented only as a protected, default-unread exception.
- `SC-5`: All effective Markdown links and path references resolve; `CLAUDE.md` remains a thin entry; docs contain no current task state; LOOP/STATE/LOG contain no human report body; any DOC-001 work material is fully routed or removed.
- `SC-6`: Verification shows no business file changes, no staged files, unchanged `tmp/`, passing `git diff --check`, and complete old/new path searches excluding Git, tmp, and build outputs.
- `SC-7`: DOC-001 completes verification and learning, appends one minimal truthful LOG entry, reaches terminal completed State without starting another task, and remains uncommitted and unpushed.

On PERSIST for an approved new Loop, replace the inactive Goal, Boundary / Scope, and Success Criteria with the approved contract and stable numbered `SC-*` identifiers. Do not materially rewrite an active Goal, Scope, or Success Criteria without user confirmation. Starting a Loop after a completed Loop creates a new instance; it is not a Stage transition.

## SOP

1. Read the project map and current Loop contract.
2. Select the appropriate Workflow through `AGENTS.md` and load only that Workflow Skill.
3. Execute the current Stage within the approved boundary.
4. Verify the Stage exit conditions and the applicable Success Criteria.
5. Update current State or task results without duplicating this contract.
6. Review potential learning and route durable knowledge appropriately.
7. Deliver the result with verification evidence and remaining limitations.

## Stop and Escalation Conditions

- **Success:** All applicable Success Criteria are passed with sufficient evidence and the deliverable is complete.
- **Blocked:** A required dependency, permission, fact, or user decision prevents further meaningful work. Record the blocker and the next unblocking action.
- **Escalation:** Stop and ask the user when the decision boundary would be crossed or when continuing could materially change the approved result.
- **No meaningful progress:** Stop mechanical retries when a new attempt produces no new evidence, modification, or diagnosis. Reassess the cause or ask the user.
- **Iteration limit:** Perform at most five execution iterations that each produce a substantive change, new evidence, or new diagnosis for the same unresolved failure. Repeating the same failed command does not count as a meaningful iteration.

## Learning and Evolution

Review completed work for knowledge that remains useful beyond the current task. Discard temporary information; route stable facts to project documentation, repeatable operations to scripts, mechanically detectable failures to tests or preflight checks, repeated multi-step procedures to Skill candidates, and universal high-impact rules to `AGENTS.md` candidates.

Obtain user approval before modifying a Skill, `AGENTS.md`, or a strong engineering constraint. Promote knowledge only when repeated evidence justifies the added maintenance cost.
