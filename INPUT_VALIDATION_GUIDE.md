# Exception handling and input validation

- `Utility.hpp`: `askInt` and `askDouble` reject malformed numbers; `askDouble` rejects NaN and infinity. `ask` rejects empty/whitespace-only text and input over 4096 characters. `parseTime` validates UTC timestamp formatting and actual calendar dates.
- `Console.cpp`: each menu operation catches `std::exception`, displays a concise error and continues accepting choices while input remains available. Closed input terminates the menu instead of looping forever.
- `main.cpp`: catches login/menu errors and exits cleanly on closed input; unrecoverable startup failures return a nonzero exit code.
- `Database.hpp`: SQLite errors are converted to exceptions; SQLite has a 5000ms busy timeout; non-finite double bindings are rejected; all user-provided query values use bound parameters.
- `LMS.cpp`: checks permissions, role, enrollment, deadlines and mark ranges; mark and moderation transactions attempt rollback if an operation fails. A rollback failure is reported without hiding the original exception.
- `tests/test_validation.cpp`: checks invalid dates, blank text, invalid integer input, SQL constraint violations, SQL syntax errors and non-finite database parameters.

## Limits

Exceptions can prevent common user errors and SQLite failures from terminating the interactive session, but they cannot guarantee recovery from every possible failure (e.g. process termination, out-of-memory, hardware failure or severe database corruption). Back up the SQLite file for important deployments. Console password entry is not hidden.
