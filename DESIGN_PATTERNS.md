# Design patterns and class diagram mapping

- **Abstract User / inheritance / polymorphism:** `User.hpp`, `User.cpp`. `User` is abstract (`role()` and `menu()` are pure virtual). Student, Lecturer, HoD, Administrator override them. `Console.cpp` uses `u.menu()` via a `User&`. Service methods still check role in SQLite; a menu alone is not an authorization boundary.
- **Factory:** `UserFactory::create` in `User.cpp` constructs a role-specific object and rejects unknown roles. `LMS::login` uses this factory after verifying the salted password hash.
- **Strategy:** `ILatePenaltyStrategy`, `StandardLatePenalty`, `NoPenaltyStrategy` in `PenaltyStrategy.hpp`. `LMS` accepts an injected strategy, allowing tests to replace the policy without modifying its implementation. The default runtime strategy is StandardLatePenalty. NoPenaltyStrategy is for alternative policy scenarios; it is not selected by the current console.
- **Repository/DAO:** `ISubmissionRepository` and `SubmissionRepository` in `SubmissionRepository.hpp/.cpp`. `LMS` accepts an injected repository, and the SQLite-backed implementation uses parameterized queries. Other LMS methods still contain SQL; moving all SQL to repositories is a separate refactoring task for full strict layer separation.

## UML / implementation mapping
`User` -> `Student`, `Lecturer`, `HoD`, `Administrator`; `UserFactory` creates these subclasses; `LMS` depends on `ISubmissionRepository` and `ILatePenaltyStrategy` interfaces.

## Test
Run `ctest --test-dir build --output-on-failure` after building. `test_patterns.cpp` checks abstract interfaces, polymorphic dispatch, factory validation, policy substitution and repository substitution.
