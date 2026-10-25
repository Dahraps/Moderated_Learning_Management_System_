# SQLite persistence and parameterised data access

The service layer (`src/LMS.cpp`) does not contain SQL or call SQLite directly. It uses `DataAccess` and `DataQuery`, which are defined in `include/mlms/DataAccess.hpp` and implemented in `src/DataAccess.cpp`. The `SubmissionRepository` is another DAO; it provides the `ISubmissionRepository` abstraction used by `LMS`.

All 48 queries extracted from the service layer are in `src/DataAccess.cpp` and are identified by `SqlId`. `DataQuery::bind` and `bindNull` bind all runtime values to SQLite prepared statements. The SQL schema is applied at startup through `DataAccess::initializeSchema`.

`DataAccess::begin`, `commit`, and `rollback` provide transaction boundaries for marking, auditing, and moderation. The database file is `mlms.db` in the working directory; data survives program restarts. Foreign-key enforcement is enabled in `DB`.

`SqlId` values are numbered to keep the migration small; descriptive query names and domain-specific repository methods would improve maintainability further. The existing `DB`/`Statement` low-level classes remain public because the repository and test fixtures rely on them; production application services do not use them directly.

Build from the project root:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Dependencies: C++17 compiler, CMake, SQLite3 development library, OpenSSL development library.
