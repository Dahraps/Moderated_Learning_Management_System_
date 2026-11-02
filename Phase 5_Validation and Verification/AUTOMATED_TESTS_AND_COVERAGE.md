# Automated Unit and Integration Tests — Phase 5

**Project:** Moderated Learning Management System (LMS)  
**Requirement:** Automated unit/integration tests (5 marks)  
**Framework:** GoogleTest 1.14.0  
**CI:** GitHub Actions (`.github/workflows/ci.yml`)

## 1. What was added

- `tests/test_phase5_gtest.cpp`: GoogleTest test suite covering late-penalty rules, invalid penalty inputs, user-role creation, timestamp validation, and SQLite integration/persistence.
- `CMakeLists.txt`: optional GoogleTest setup (`MLMS_ENABLE_GTEST`) and coverage instrumentation (`MLMS_ENABLE_COVERAGE`).
- `.github/workflows/ci.yml`: builds the project, runs all CTest tests including the GoogleTest suite, measures business-logic line coverage, and uploads the test/coverage reports.

## 2. How CI runs the tests

The GitHub Actions workflow runs on pushes, pull requests, and manual dispatch. It installs CMake, GCC, SQLite, OpenSSL, and gcovr, then configures with GoogleTest and coverage enabled, builds the project, runs `ctest`, and creates XML and text coverage reports.

To run the complete suite locally on a machine with internet access (GoogleTest is fetched by CMake):

```bash
cmake -S . -B build-ci -DCMAKE_BUILD_TYPE=Debug -DMLMS_ENABLE_GTEST=ON -DMLMS_ENABLE_COVERAGE=ON
cmake --build build-ci --parallel 2
ctest --test-dir build-ci --output-on-failure
```

## 3. Coverage scope and target

The CI coverage filter includes these business-logic files:

- `src/LMS.cpp`
- `src/User.cpp`
- `src/SubmissionRepository.cpp`
- `src/DataAccess.cpp`
- `include/mlms/PenaltyStrategy.hpp`

The required target is **at least 70% line coverage**. CI fails the coverage step if the measured line coverage is below 70%.

## 4. Actual local baseline result

The existing eight CTest programs were built with GCC coverage instrumentation and executed in this Linux environment. All eight passed. The measured business-logic source-file baseline was:

| File | Covered executable lines | Total executable lines | Line coverage |
|---|---:|---:|---:|
| `src/LMS.cpp` | 92 | 117 | 78.63% |
| `src/User.cpp` | 10 | 10 | 100.00% |
| `src/SubmissionRepository.cpp` | 5 | 5 | 100.00% |
| `src/DataAccess.cpp` | 35 | 63 | 55.56% |
| `include/mlms/PenaltyStrategy.hpp` | 5 | 6 | 83.33% |
| **Combined unique-file baseline** | **147** | **201** | **73.13%** |

**Calculation:** 147 covered executable lines / 201 executable lines × 100 = **73.13%**. This is above the 70% target for the listed scope.

### Important execution limitation

This baseline is from the eight existing project tests, which use C++ assertions. The new GoogleTest suite and GitHub Actions workflow have been added to the project, but the GoogleTest dependency could not be downloaded in this environment because outbound network access to GitHub was unavailable. Therefore, the 73.13% figure is an actual local baseline from the existing tests, **not a claim that the new GoogleTest suite or GitHub Actions has already run**. After pushing the changes, check the GitHub Actions run and retain its generated coverage artifact as the final CI evidence. The CI report may differ because it includes the additional GoogleTest cases.

## 5. Test evidence to retain

- GitHub Actions run showing successful build and test execution.
- CTest output showing test counts and pass/fail status.
- `coverage-business-logic.txt` and `coverage.xml` from the CI artifact.
- Any failure output and corresponding defect/retest record.
