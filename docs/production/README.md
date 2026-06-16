# Ocean Production Units

Create one folder per larger task:

```text
docs/production/{YYYY-MM-DD}-{unit-id}/
├── 00-raw-input.md
├── 01-semantics.md
├── 02-infra-audit.md
├── 03-plan.md
├── 04-tests.md
├── 05-implementation-log.md
├── 06-test-results.md
└── 07-review.md
```

Run:

```powershell
python scripts/harness_state_validator.py --json
```

Use `parallel_lock` values such as `Ocean.BuildGrid`, `Ocean.WaterSetup`, `Ocean.ResourcePCG`, or `Ocean.EditorAutomation`.
