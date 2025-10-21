# Agent Debrief Format

Append one JSON object per line to `DEBRIEF.json` at the end of each session.
The file is treated as JSON Lines (JSONL), so each entry MUST stay on a single
line. Use the following schema:

```json
{
  "date":"YYYY-MM-DD",
  "time":"HH:MM",
  "summary":"One-sentence high-level outcome.",
  "topics":[
    {
      "topic":"Topic name",
      "what":"Short description",
      "why":"Reason it came up",
      "context":"Relevant background",
      "issue":"The core problem",
      "resolution":"How it was handled",
      "future_work":"Next steps or open questions",
      "time_percent":25
    }
  ],
  "key_decisions":["Decision 1","Decision 2"],
  "action_items":[{"task":"Task description","owner":"Owner"}]
}
```

Guidelines:

- Write entries to `DEBRIEF.json` in the repository root; do not duplicate
  content in `AGENTS.md`.
- JSON must remain single-line (JSONL format).
- Append only; never rewrite or reflow existing entries.
- Fill in today’s date/time automatically in UTC-8 (PST) unless otherwise
  specified by the maintainer.
- Keep `time_percent` values roughly proportional and totaling 100 across
  topics.
