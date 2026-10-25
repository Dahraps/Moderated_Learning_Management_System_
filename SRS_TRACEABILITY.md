# Phase 2 SRS alignment (24 October 2026)

The Phase 2 PDF lists 11 functional requirements but does not assign MoSCoW priorities or consistent requirement IDs. IDs below are implementation cross-reference labels, not quoted SRS identifiers.

| ID | Phase 2 functional requirement | Main implementation | Status |
|---|---|---|---|
| FR-01 | Authenticate users with salted hashes and roles | LMS::login, UserFactory | Implemented; username used as login identifier (SRS says surname) |
| FR-02 | Administrator creates, updates, deactivates users and manages courses | createUser, updateUser, deactivateUser, createDepartment, createCourse, assignLecturer, enrol | Implemented for user update; course/department update not yet implemented |
| FR-03 | Student submits work | submit | Implemented as local file-path reference; does not copy file into managed storage |
| FR-04 | Unlimited pre-deadline resubmission | submit | Implemented; post-deadline resubmission with an extension supported when within effective deadline |
| FR-05 | Individual extension | extension | Implemented |
| FR-06 | Lecturer marks and provides feedback | mark | Implemented |
| FR-07 | Automated late penalty | StandardLatePenalty, lateDays, mark | Implemented; day boundary uses 24-hour ceiling, while SRS says calendar days |
| FR-08 | HoD departmental review | pending, moderate | Implemented; pending screen does not display full written feedback |
| FR-09 | HoD individual and bulk release | moderate, release, bulkRelease, adjustMark | Implemented; bulk release blocks if any course submission is unmoderated |
| FR-10 | Student sees released grades and feedback | results | Implemented |
| FR-11 | Audit mark changes | audit, mark, adjustMark | Implemented for mark and HoD adjustment; additional review of all potential mark updates recommended |

## Remaining requirements before claiming full Phase 4 compliance
- Make surname login consistent with approved SRS or document the accepted identifier change.
- Provide full update operations for departments and courses and a full HoD review display with feedback.
- Copy submitted files into application-managed persistent storage and handle rollback and replacement safely.
- Resolve SRS 'calendar day' versus 24-hour interval penalty semantics.
- Separate all SQL access into dedicated DAO/repository classes to match Phase 3 architecture strictly.
- Run comprehensive automated integration and boundary tests, static analysis and measured performance/coverage tests.

The included tests are a smoke/regression baseline, not proof of complete acceptance testing.
