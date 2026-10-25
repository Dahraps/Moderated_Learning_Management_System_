# CS4433 Phase 4 — Moderated Learning Management System

C++17 console implementation based on the submitted Phase 3 system design and SQL schema. This is a modular educational implementation, not a production deployment.

## Project structure

- `include/mlms/User.hpp`: abstract User, Student, Lecturer, HoD, Administrator, UserFactory
- `include/mlms/Domain.hpp`: Submission, late-penalty Strategy, submission repository interface and implementation
- `include/mlms/Database.hpp`: SQLite connection and prepared statement wrappers
- `include/mlms/Utility.hpp`: password hashing, date and input helpers
- `include/mlms/LMS.hpp` and `src/LMS.cpp`: application operations and business rules
- `include/mlms/Console.hpp` and `src/Console.cpp`: role-specific menus
- `src/main.cpp`: startup and database initialization
- `sql/schema.sql`: original Phase 3 schema with a moderation revision marker
- `tests/test_logic.cpp`: starter automated tests

## Build from existing Git repository

Copy these files into your existing repository. No new Git repository is needed.

Requirements: C++17 compiler, CMake 3.16+, SQLite3 development package, OpenSSL development package.

On Ubuntu/WSL, install dependencies with `sudo apt install cmake g++ libsqlite3-dev libssl-dev`.

From the project directory:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
./build/mlms
```

On Windows, use a CMake generator and installed SQLite3/OpenSSL packages compatible with your compiler; the executable may be `build/Debug/mlms.exe`.

Run the executable from the project root, where `sql/schema.sql` is available. The application creates `mlms.db` locally. At first launch, create an Administrator account with a password of at least eight characters.

## Demonstration sequence

1. Administrator creates department, lecturer, student and HoD accounts (set the HoD department ID).
2. Administrator creates a course, assigns the lecturer, and enrols the student.
3. Lecturer creates an assignment with a UTC deadline, e.g. `2026-11-15T23:59:00`.
4. Student views assignments and submits an existing local file path; before the applicable deadline, they may resubmit.
5. Lecturer optionally grants an individual extension and enters a mark with written feedback and a reason.
6. HoD reviews, approves or returns the mark. If returned, lecturer revises it before the next approval.
7. HoD releases the approved result; only then may the student see mark and feedback.

Late work loses 10% of maximum available mark per day or part-day late, up to three days; later submissions receive zero. This interpretation should be checked against your Phase 2 SRS.

## Design and security

- Abstract `User` and derived roles; `UserFactory` for polymorphic user creation.
- Strategy pattern: `ILatePenaltyStrategy` / `StandardLatePenalty` / `NoPenaltyStrategy`.
- Repository pattern: `ISubmissionRepository` / `SubmissionRepository`.
- Prepared SQL statements, foreign keys, transaction for marking and moderation.
- OpenSSL PBKDF2-HMAC-SHA256 (210,000 iterations) with 16-byte random salt. No plaintext password is stored in the database.
- `MarkAudit` stores mark changes with actor, old/new mark, reason, and UTC timestamp.

## Important limitations to address before final assessment

This version compiles and its starter unit tests pass, but **it is not yet fully validated against every Must requirement of your Phase 2 SRS**. In particular:

- For strict four-layer architecture, some SQL in `src/LMS.cpp` must be moved to dedicated DAO/repository source files. SQLite is also used in `Domain.hpp` by `SubmissionRepository`.
- The program stores a path to the student's submission rather than copying the file into managed secure storage. File access and immutability need strengthening.
- Passwords are entered visibly in the console. Add hidden password entry for deployment.
- There is no account lockout or comprehensive access-control test suite.
- The initial tests check penalty calculations, user factory, hashing and date validation; they do not yet provide the complete integration, boundary, or acceptance coverage required for Phase 5.
- Database schema changes on an existing `mlms.db` require a migration or a fresh test database; do not delete a database with important data.
- The submitted `.drawio` diagrams and exact Phase 2 SRS should be reconciled with every source operation before declaring full design traceability.

## Coding standard

C++17; PascalCase types, camelCase methods/variables, RAII SQLite handles, prepared SQL, exceptions for invalid inputs and database errors, smart pointers for polymorphic users. Use Git in your existing repository to commit the code.

## Role-based access update (24 October 2026)

The application now checks the authenticated account's role and active status **inside each application-service operation**, not only in the console menu. Administrators can list users/departments/courses, deactivate accounts, and perform the original management functions. Lecturers can list their assignments and submissions; all roles can change their own password by supplying their current password. Department and course ownership checks still restrict marking and moderation. Students only see their own released results. Two CTest targets now run, including authentication, cross-role denial and inactive-account checks.

**Source-of-truth limitation:** The Phase 2 SRS with its exact Must IDs was not provided with this implementation. These features follow the Final Project case study and Phase 3 design, not a verified one-to-one mapping against the user's Phase 2 Must list. Compare against the approved SRS before claiming complete requirement coverage.

**Remaining engineering work:** SQL statements are still located in `LMS.cpp` rather than entirely inside dedicated repository classes, and submission files are referenced by path rather than copied to an immutable managed storage area. These should be addressed before deployment. The automated tests cover foundational business logic and access control; full acceptance tests, deadline-boundary tests and coverage measurement remain necessary.

## Phase 2 SRS reconciliation update
See `SRS_TRACEABILITY.md` for a requirement-by-requirement mapping and known gaps. This revision adds Administrator user update, HoD audited mark adjustment, course-level bulk release, and corresponding automated regression tests. The supplied Phase 2 document does not specify MoSCoW labels, so none are invented.
