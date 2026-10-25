# MLMS C++17 Coding Standard

This project follows a documented, pragmatic C++ Core Guidelines-inspired convention. Existing legacy files are not claimed to be fully style-compliant; all new code should follow these rules, and legacy code should be improved incrementally without altering behavior.

## Language and organization
- Use ISO C++17 (`CMAKE_CXX_STANDARD 17`, `CMAKE_CXX_EXTENSIONS OFF`).
- Put public interfaces in `include/mlms/*.hpp`, implementations in `src/*.cpp`, and automated tests in `tests/*.cpp`.
- Use `#pragma once` in headers. Include what a file uses; avoid `using namespace std;` in public headers.
- Use `PascalCase` for class and struct names (`SubmissionRepository`), `camelCase` for functions and variables (`updateMark`), and `UPPER_SNAKE_CASE` for macros, if any.
- Prefer clear names over abbreviations; indent with four spaces; put braces on the same line as the statement or declaration.

## Correctness and resource management
- Prefer RAII, standard containers, `std::unique_ptr`, and references to raw owning pointers.
- Mark methods `const` when they do not mutate state. Use `override` on virtual overrides and virtual destructors on polymorphic bases.
- Validate input at boundaries. Use exceptions for invalid operations and database failures; catch at the interactive menu boundary.
- Use parameterized SQLite statements for user-provided values. Keep SQLite access in the data-access/repository layer.
- Wrap multi-step updates and audit entries in transactions; rollback on failure.
- Never log passwords, salts, or secrets. Use OpenSSL password hashing rather than plain-text passwords.

## Testing and review
- Add a regression test for each corrected bug and for business-rule boundary cases.
- Build with `-Wall -Wextra -Wpedantic` on GCC/Clang or `/W4` on MSVC.
- Run `ctest --test-dir build --output-on-failure` before committing.
- Use `clang-format` where available; `.clang-format` is a formatting target, not proof that all legacy source is formatted.
- Review compiler warnings and document known limitations. Do not ignore test failures.
