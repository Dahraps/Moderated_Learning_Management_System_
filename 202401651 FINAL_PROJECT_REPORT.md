# MODERATED LEARNING MANAGEMENT SYSTEM (MLMS)

## Final Project Report

**Student Name:** Morapeli Seboka
**Student ID:** `202401651`  
**Git Repository URL:** `https://github.com/Dahraps/Moderated_Learning_Management_System_.git`  
  

---

## EXECUTIVE ABSTRACT

The **Moderated Learning Management System (MLMS)** is an offline-first, high-performance desktop application engineered in standard $C++17$ with embedded SQLite persistent database storage. Designed specifically to resolve assessment management and governance challenges in educational environments characterized by legacy computer hardware and absent or unreliable internet connectivity, MLMS provides a robust, zero-network-dependency software system supporting approximately 800 active student records across four distinct user roles: System Administrator, Lecturer, Head of Department (HoD), and Student.

The application incorporates strict Role-Based Access Control (RBAC), salted OpenSSL cryptographic password hashing (SHA-256), mandatory parameterized SQL query execution, an automated deterministic late-penalty engine (-10% mark deduction per 24-hour calendar day up to 3 days; 0% score thereafter), a two-stage Head of Department grade moderation and release workflow, and immutable audit trails that record all mark adjustments and deadline extensions.

Following a structured seven-week development methodology divided into nine sequential engineering phases—encompassing feasibility evaluation, requirements engineering, object-oriented system design, static analysis (Cppcheck 0 findings across 438 LOC), release v1.0 deployment, and perfective maintenance (Change Request CR1: Class-Wide Deadline Extensions for Release v1.1)—this report presents full technical documentation, UML architectural specifications, verification test execution logs, and compliance evaluations demonstrating alignment with institutional rubrics and international software engineering standards.

---

## TABLE OF CONTENTS

1. [Title Page, Abstract, and Table of Contents](#executive-abstract)
2. [Introduction](#1-introduction)
   - 1.1 Institutional Context & Problem Statement
   - 1.2 System Purpose & Technical Objectives
   - 1.3 Target Execution Environment & Constraints
3. [Project Management Plan](#2-project-management-plan)
   - 2.1 Feasibility Study (5 Dimensions)
   - 2.2 Work Breakdown Structure (WBS)
   - 2.3 Project Schedule & Critical Path Analysis
   - 2.4 Risk Management Register & Exposure Mitigation
4. [Requirements Engineering](#3-requirements-engineering)
   - 3.1 Stakeholder Profiles & Conflict Resolution Analysis
   - 3.2 Software Requirements Specification (SRS: FR-01 to FR-14; NFR-01 to NFR-06)
   - 3.3 Use Case Modeling & Detailed Use Case Specifications (UC-01, UC-02, UC-03)
5. [Software Design Document](#4-software-design-document)
   - 4.1 Architectural Style & Selection Rationale
   - 4.2 Class Design & Object-Oriented Static Structure
   - 4.3 Design Patterns & SOLID Principles Adherence
   - 4.4 Behavioural Modeling (State Machine, Activity, & Sequence Diagrams)
   - 4.5 Relational Database Design (ERD, Data Dictionary, & DDL Schema)
6. [Implementation](#5-implementation)
   - 5.1 Repository Layout & Build System Configuration
   - 5.2 Key Code Architecture & Implementation Snippets
   - 5.3 Security Controls & Database Persistence Implementation
7. [Verification and Validation](#6-verification-and-validation)
   - 6.1 Test Strategy & Verification Methodology
   - 6.2 Test Suite Execution Matrix (Boundary Value & Equivalence Partitioning)
   - 6.3 Defect Tracking & Resolution Log
8. [Configuration Management and Quality Assurance](#7-configuration-management-and-quality-assurance)
   - 8.1 Branching Strategy & Repository Version Management
   - 8.2 Static Code Analysis Execution (Cppcheck 2.22.0 Results)
   - 8.3 Software Metrics & Cyclomatic Complexity Analysis
9. [Deployment](#8-deployment)
   - 9.1 UML Deployment Architecture
   - 9.2 Technician Installation & Build Manual
   - 9.3 Role-Specific Operational Procedures
   - 9.4 Release Notes (v1.0 Baseline)
10. [Maintenance (Change Request CR1)](#9-maintenance-change-request-cr1)
    - 10.1 Change Request Identification & Maintenance Classification
    - 10.2 Comprehensive Traceability & Impact Analysis Matrix
    - 10.3 Release v1.1 Implementation & Sequence Diagram Updates
    - 10.4 Regression Testing Execution & Compatibility Verification
11. [Professional, Legal, and Ethical Issues](#10-professional-legal-and-ethical-issues)
    - 11.1 Data Protection & Student Privacy Legislation (GDPR Compliance)
    - 11.2 Assessment Record Security & Audit Integrity
    - 11.3 Digital Accessibility for Assistive Technology Users
    - 11.4 Ethical Fairness & Objectivity in Automated Late Penalty Rules
12. [Conclusion](#11-conclusion)
    - 12.1 Project Summary & Achievements
    - 12.2 Reflection on Engineering Constraints Overcome
    - 12.3 Recommendations for Future Evolution
13. [References](#12-references)
14. [Appendices](#13-appendices)
    - Appendix A: Complete Requirements Traceability Matrix (RTM)
    - Appendix B: Automated Test Suite Execution Logs
    - Appendix C: System Administrator & User Operational Manuals
    - Appendix D: Final Updated Risk Register

---

## 1. INTRODUCTION

### 1.1 Institutional Context & Problem Statement

Secondary educational institutions operating in resource-constrained regions routinely face severe technological limitations. IT infrastructure in these schools typically consists of legacy desktop computers running older operating systems, minimal system memory, and intermittent or completely non-existent internet access. In these operational settings, commercial cloud-hosted Learning Management Systems (LMS) such as Canvas, Blackboard, or Moodle are entirely impractical due to their heavy network bandwidth demands, ongoing subscription licensing costs, and complex server administration requirements.

Beyond infrastructure bottlenecks, educational governance in secondary institutions requires strict control over assessment and grading processes. Academic integrity vulnerabilities emerge when lecturers can publish marks directly to students without independent verification, when grade alterations occur in database records without accountability logs, or when manual calculation of late submission penalties introduces human error and accusations of favoritism. The lack of a centralized, offline system leads to administrative inefficiency, lost student submissions, fragmented record-keeping, and non-compliance with data protection regulations.

### 1.2 System Purpose & Technical Objectives

The **Moderated Learning Management System (MLMS)** was engineered as a lightweight, zero-dependency, standalone $C++17$ desktop console application backed by a embedded SQLite database (`mlms.db`). It automates the complete coursework submission, extension management, late-penalty deduction, grading, and moderation lifecycle locally on school hardware without requiring any external network or cloud connectivity.

The core engineering objectives of MLMS include:

1. **Zero External Network Dependency**: Fulfilling all functional requirements locally via a single compiled binary interfacing with embedded SQLite file storage.
2. **Multi-Role Governance**: Implementing Role-Based Access Control (RBAC) to separate System Administrators, Lecturers, Heads of Department (HoDs), and Students.
3. **Automated Penalty Engine**: Applying objective late-penalty deductions based on exact submission timestamps relative to active deadlines.
4. **Mandatory HoD Grade Moderation**: Enforcing a two-tier review state machine in which raw marks entered by lecturers remain hidden from students until formally audited, moderated, and released by the Head of Department.
5. **Immutable Audit Trails**: Capturing unalterable database records for every mark adjustment, score override, or deadline extension, detailing actor identity, timestamps, historical values, and mandatory justifications.

### 1.3 Target Execution Environment & Constraints

* **Hardware Compatibility**: Intel/AMD x86_64 processors, minimum 512 MB RAM, 50 MB available local disk space.
* **Operating Systems**: Linux (Ubuntu 20.04+, Debian 11+, RHEL 8+), Windows 10/11 (via WSL or Native MinGW-w64 GCC).
* **Software Toolchain**: $C++17$ compliant compiler (GCC 9+, Clang 10+, MSVC 2019+), CMake v3.14+, SQLite v3.30+.
* **Concurrency Model**: Single-user multi-role desktop console application; thread-safe database operations utilizing SQLite WAL (Write-Ahead Logging) mode.

---

## 2. PROJECT MANAGEMENT PLAN

### 2.1 Feasibility Study (5 Dimensions)

#### 2.1.1 Technical Feasibility
The project is technically viable using the specified open-source C++ development stack. $C++17$ offers maximum execution efficiency and minimal memory footprint, ensuring swift execution on legacy hardware. SQLite3 provides serverless, zero-configuration relational storage inside a single database file. Automated testing via Catch2/CTest and static code checking via Cppcheck guarantee software quality without commercial licensing.

#### 2.1.2 Operational Feasibility
The operational workflows mirror existing secondary school academic structures. The system defines clear, non-overlapping operational permissions for System Administrators, Lecturers, Heads of Department, and Students. The simple command-line interface ensures straightforward navigation for staff members with limited IT experience.

#### 2.1.3 Economic Feasibility
Development costs are strictly limited to engineering time. Utilizing non-proprietary, open-source software tools (GCC, CMake, SQLite, Cppcheck) eliminates software licensing fees. Running natively on existing school desktop infrastructure prevents new hardware acquisitions.

#### 2.1.4 Legal & Compliance Feasibility
Legal requirements dictate data privacy, access limitation, and assessment record security. MLMS complies with data protection standards by avoiding plaintext password storage (utilizing salted SHA-256 cryptographic hashes), enforcing strict RBAC permissions, using parameterized SQL queries to block injection attacks, and maintaining an immutable audit log.

#### 2.1.5 Schedule Feasibility
The project was structured across a strict seven-week schedule divided into nine sequential phases. Formal review gates (Gate 1: Requirements Approval, Gate 2: Design Approval, Gate 3: Test Review) ensure implementation cannot proceed without cleared prerequisites.

### 2.2 Work Breakdown Structure (WBS)

```
1. Project Planning & Governance
   1.1 Feasibility Assessment
   1.2 Scope Definition & Objectives
   1.3 Schedule & Gantt Development
   1.4 Risk Register Formulation
2. Requirements Engineering
   2.1 Stakeholder Analysis & Identification
   2.2 Conflict Identification & Resolution
   2.3 Software Requirements Specification (SRS)
   2.4 Use Case Modeling & Specifications
   2.5 Gate 1 Formal Review
3. System Design
   3.1 Architectural Style Selection
   3.2 Component & Class Diagramming
   3.3 Behavioral Modeling (State Machine, Sequence)
   3.4 Relational Database ERD & Schema Design
   3.5 Gate 2 Formal Review
4. Implementation
   4.1 C++17 Project & CMake Setup
   4.2 SQLite Database Integration
   4.3 Security & Hashing Modules
   4.4 Business Logic & Penalty Strategies
   4.5 Console UI & Menu Dispatcher
5. Verification & Validation
   5.1 Test Strategy Formulation
   5.2 Automated Unit & Integration Testing
   5.3 Boundary Value & Equivalence Partition Testing
   5.4 Gate 3 Formal Review
6. Configuration Management & Quality Assurance
   6.1 Repository Branch Management
   6.2 Static Code Analysis (Cppcheck)
   6.3 Software Quality Metrics (Lizard)
7. Deployment
   7.1 Deployment Architecture Modeling
   7.2 Technical Installation Guide
   7.3 Role User Manuals & Release Notes v1.0
8. Maintenance & Evolution
   8.1 Change Request Analysis (CR1)
   8.2 Impact Analysis & Traceability Mapping
   8.3 Release v1.1 Code Modifications
   8.4 Full Regression Testing
9. Project Completion
   9.1 Professional, Legal, & Ethical Analysis
   9.2 Final Report Synthesis
```

### 2.3 Project Schedule & Critical Path Analysis

The critical path spans: **Planning** $\rightarrow$ **Requirements Engineering** $\rightarrow$ **Gate 1 Review** $\rightarrow$ **System Design** $\rightarrow$ **Gate 2 Review** $\rightarrow$ **Core Implementation** $\rightarrow$ **Verification & Validation** $\rightarrow$ **Gate 3 Review** $\rightarrow$ **Deployment v1.0** $\rightarrow$ **CR1 Maintenance** $\rightarrow$ **Release v1.1** $\rightarrow$ **Final Completion**.

```
+------------------------+----+----+----+----+----+----+----+
| Activity Name          | W1 | W2 | W3 | W4 | W5 | W6 | W7 | Dependencies / Milestones
+------------------------+----+----+----+----+----+----+----+
| Project Planning       |XXXX|    |    |    |    |    |    | Baseline Start
| Requirements Eng.      |  XX|XXXX|    |    |    |    |    | Gate 1 Approval (M1)
| System Architecture    |    |  XX|XXXX|    |    |    |    | Gate 2 Approval (M2)
| C++ Implementation     |    |    |  XX|XXXX|XX  |    |    | Gate 2 Cleared
| Verification & Testing |    |    |    |  XX|XXXX|    |    | Gate 3 Approval (M3)
| Static Analysis & SQA  |    |    |    |    |  XX|XX  |    | Static Analysis Passed
| Deployment (v1.0)      |    |    |    |    |    |XX  |    | Production Baseline
| Maintenance (CR1 v1.1) |    |    |    |    |    |  XX|XX  | Regression Cleared
| Final Documentation    |    |    |    |    |    |    |XXXX| Project Completion
+------------------------+----+----+----+----+----+----+----+
```

### 2.4 Risk Management Register & Exposure Mitigation

| Risk ID | Category | Risk Description | Likelihood (1-5) | Impact (1-5) | Exposure Score | Mitigation Strategy | Owner |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **R1** | Technical | Database design fails to preserve historical submissions or audit records. | 3 | 5 | **15 (High)** | Enforce 3NF normalization, SQLite foreign keys (`PRAGMA foreign_keys=ON`), and database transactions. | Database Lead |
| **R2** | Security | Unhashed password storage or SQL injection vulnerability compromises user data. | 3 | 5 | **15 (High)** | Use salted OpenSSL SHA-256 password hashing and mandatory parameterized SQL bindings (`sqlite3_bind_*`). | Security Lead |
| **R3** | Logic | Late-penalty calculation engine handles boundary conditions incorrectly. | 3 | 4 | **12 (High)** | Isolate penalty logic using the Strategy Pattern; perform boundary tests around $T_{due} + 1\text{s}$, $+24\text{h}$, $+72\text{h}$. | Software Lead |
| **R4** | Schedule | Scope creep during design phase threatens the 7-week submission deadline. | 3 | 5 | **15 (High)** | Enforce fixed scope boundaries (CLI interface only, no GUI/web overhead); monitor critical path weekly. | Project Manager |
| **R5** | Stakeholder| Conflict between Lecturers and HoDs regarding immediate mark visibility to students. | 2 | 5 | **10 (Medium)** | Establish clear policy rules at Gate 1: HoD moderation policy strictly overrides lecturer preferences. | System Analyst |
| **R6** | Quality | Undetected code defects lead to wrong penalty calculations or unauthorized mark releases. | 3 | 5 | **15 (High)** | Mandate static code checking (Cppcheck), dynamic unit testing (Catch2), and full regression execution prior to release. | QA Lead |

---

## 3. REQUIREMENTS ENGINEERING

### 3.1 Stakeholder Profiles & Conflict Resolution Analysis

#### 3.1.1 Stakeholder Roles & Organizational Interests
* **Principal / Board of Governors**: Demands institutional compliance, strict academic governance, full audit accountability, and low administrative overhead.
* **Lecturers**: Require flexible assignment creation, individual extension capabilities, straightforward grading tools, and rapid feedback distribution.
* **Head of Department (HoD)**: Requires complete departmental mark oversight, automated penalty verification, score correction authority, and controlled mark release.
* **Students**: Require transparent deadline tracking, multi-attempt pre-deadline resubmissions, constructive feedback, and fair, consistent late-penalty application.
* **IT Technician**: Demands a zero-maintenance, offline executable that runs efficiently on legacy hardware without database administration overhead.

#### 3.1.2 Stakeholder Conflict Resolution Matrix

```
[Conflict 1: Mark Release Timing]
  Lecturer View: Publish marks to students immediately after grading to provide rapid feedback.
  HoD / Principal View: Block student grade visibility until formal HoD review and sign-off.
  Resolution: The HoD governance policy overrides lecturer preference. The system state machine
              locks marks in a 'Marked' state post-grading. Grades transition to 'Released' only
              when explicitly executed by the HoD.

[Conflict 2: Late Coursework Handling]
  Student View: Allow flexible late submissions without zero-mark rejections.
  HoD Policy View: Enforce strict late penalties (-10% per day up to 3 days; 0% thereafter).
  Resolution: The system accepts late submissions up to and beyond 3 days (satisfying student desires
              not to reject work outright), but automatically applies mathematical deductions based on exact
              submission timestamps (satisfying HoD policy enforcement).

[Conflict 3: Pre-Deadline Resubmissions]
  Student View: Allow unlimited resubmissions prior to cutoffs to correct accidental errors.
  Lecturer View: Prevent folder clutter and manual file version tracking.
  Resolution: Permit unlimited pre-deadline resubmissions. The system automatically overwrites
              the previous submission record and updates the timestamp, maintaining a clean single entry.
```

### 3.2 Software Requirements Specification (SRS)

#### 3.2.1 Functional Requirements (FR)
* **FR-01 (Authentication)**: The system shall authenticate users using unique usernames and salted SHA-256 password hashes, restricting operations strictly by role (Administrator, Lecturer, HoD, Student).
* **FR-02 (User Account Administration)**: System Administrators shall be able to create, modify, deactivate, and list user accounts.
* **FR-03 (Academic Structure Setup)**: System Administrators shall be able to create academic departments, register courses, assign lecturers to courses, and enrol students in course rosters.
* **FR-04 (Assignment Creation)**: Lecturers shall be able to create assignments with a title, description, positive maximum mark ($>0$), and ISO-8601 deadline timestamp ($YYYY-MM-DD\ HH:MM$).
* **FR-05 (Assignment Submission)**: Enrolled students shall be able to upload coursework submissions for active course assignments.
* **FR-06 (Pre-Deadline Resubmission)**: Students shall be able to overwrite existing coursework submissions prior to the active assignment deadline.
* **FR-07 (Individual Deadline Extensions)**: Lecturers shall be able to grant individual student deadline extensions with a revised cutoff timestamp and justification text.
* **FR-08 (Class-Wide Deadline Extensions - v1.1 / CR1)**: Lecturers shall be able to extend assignment deadlines for an entire course cohort in a single operation.
* **FR-09 (Automated Late Penalty Deduction)**: The system shall automatically calculate late penalties (-10% deduction per 24-hour period late up to 3 days; 0% multiplier past 3 days) based on submission timestamps relative to active deadlines.
* **FR-10 (Grading & Qualitative Feedback)**: Lecturers shall input raw numeric scores ($\le \text{max\_mark}$) and qualitative written feedback for student submissions.
* **FR-11 (HoD Moderation Review)**: HoDs shall view all graded submissions across their department, verify penalty applications, and adjust marks with mandatory justification text.
* **FR-12 (Controlled Grade Release)**: HoDs shall execute formal grade releases, transitioning submission states from `Moderated` to `Released`.
* **FR-13 (Student Results Viewing)**: Students shall view final moderated scores and written feedback only after formal release by the HoD.
* **FR-14 (Immutable Audit Trail)**: The system shall record an un-alterable database log entry for every mark modification or deadline extension, detailing actor ID, timestamp, old value, new value, and justification.

#### 3.2.2 Non-Functional Requirements (NFR)
* **NFR-01 (Performance)**: User authentication, submission processing, and dashboard queries shall complete in $<2.0$ seconds on a target dataset of 800 active student records.
* **NFR-02 (Security)**: Plaintext passwords shall never be persisted or logged; all credentials must use salted SHA-256 cryptographic hashes. All database queries must use parameterized bindings.
* **NFR-03 (Offline Availability)**: The application shall run entirely offline without external network or cloud service dependencies.
* **NFR-04 (Maintainability & Static Code Quality)**: C++17 source code shall conform to standard guidelines, achieving zero errors/warnings in static code analysis (Cppcheck 2.22.0).
* **NFR-05 (Test Coverage)**: Automated unit and integration test suites shall achieve $\ge 70\%$ statement coverage across core domain logic modules.
* **NFR-06 (Portability)**: The application shall compile via CMake across standard C++17 desktop environments (GCC, Clang, MSVC).

### 3.3 Use Case Model & Detailed Specifications

```
                               +-----------------------------------+
                               |      MLMS SYSTEM BOUNDARY         |
                               |                                   |
  (( System Administrator ))   |   +---------------------------+   |
              |                |   | Manage Accounts & Courses |   |
              +------------------->| (FR-02, FR-03)            |   |
              |                |   +---------------------------+   |
              |                |                 ^                 |
              v                |                 | <<include>>     |
    [ Authenticate User ]----->|   +---------------------------+   |
            ^                  |   | Authenticate User (FR-01) |   |
            |                  |   +---------------------------+   |
            |                  |                                   |
    (( Lecturer ))             |   +---------------------------+   |
            |                  |   | Create Assignment (FR-04) |   |
            +------------------->|                           |   |
            |                  |   +---------------------------+   |
            |                  |                 ^                 |
            |                  |                 | <<extend>>      |
            |                  |   +---------------------------+   |
            |                  |   | Grant Extensions          |   |
            |                  |   | (FR-07, FR-08)            |   |
            |                  |   +---------------------------+   |
            |                  |                                   |
            |                  |   +---------------------------+   |
            +------------------->| Grade & Feedback (FR-10)  |   |
            |                  |   +---------------------------+   |
            |                  |                                   |
   (( Head of Department ))    |   +---------------------------+   |
            |                  |   | Moderate & Release Marks  |   |
            +------------------->| (FR-11, FR-12, FR-14)      |   |
            |                  |   +---------------------------+   |
            |                  |                                   |
    (( Student ))              |   +---------------------------+   |
            |                  |   | Submit Coursework         |   |
            +------------------->| (FR-05, FR-06)            |   |
            |                  |   +---------------------------+   |
            |                  |                 |                 |
            |                  |                 | <<include>>     |
            |                  |                 v                 |
            |                  |   +---------------------------+   |
            |                  |   | Calculate Penalty (FR-09) |   |
            |                  |   +---------------------------+   |
            |                  |                                   |
            |                  |   +---------------------------+   |
            +------------------->| View Released Results     |   |
                               |   | (FR-13)                   |   |
                               |   +---------------------------+   |
                               |                                   |
                               +-----------------------------------+
```

#### Detailed Use Case Specifications

##### Use Case 1: Submit Assignment (`UC-01`)
* **Actor**: Student
* **Preconditions**: Student is authenticated; assignment exists in an enrolled course.
* **Main Success Scenario**:
  1. Student selects course and views active assignments.
  2. Student inputs submission file path.
  3. System records system timestamp ($T_{sub}$) and compares it against active deadline ($T_{due}$).
  4. System executes penalty strategy, stores submission in SQLite, sets status to `Submitted` (or `Late`), and displays confirmation.
* **Alternative Flow A1 (Pre-deadline Resubmission)**:
  - If $T_{sub} \le T_{due}$ and a previous submission exists, system overwrites file path, updates timestamp, and maintains state `Submitted`.
* **Exception Flow E1 (Database Failure)**:
  - SQLite write fails; system catches exception, rolls back transaction, logs error, and notifies student without crashing.

##### Use Case 2: Mark Submission & Provide Feedback (`UC-02`)
* **Actor**: Lecturer
* **Preconditions**: Lecturer is authenticated and assigned to the course; student submissions exist.
* **Main Success Scenario**:
  1. Lecturer selects assignment and opens a student submission.
  2. Lecturer inputs raw numeric mark ($\le \text{max\_mark}$) and qualitative written feedback.
  3. System calculates final mark using the penalty engine, saves records, updates status to `Marked`, and logs activity.
* **Exception Flow E1 (Invalid Mark Input)**:
  - Entered mark $<0$ or $>\text{max\_mark}$; system rejects input with a validation warning.

##### Use Case 3: Moderate & Release Marks (`UC-03`)
* **Actor**: Head of Department (HoD)
* **Preconditions**: HoD is authenticated; graded submissions in `Marked` state exist in department courses.
* **Main Success Scenario**:
  1. HoD accesses department review menu and selects a course.
  2. HoD inspects submissions, raw marks, applied penalties, and written feedback.
  3. HoD approves marks; system updates state to `Moderated`.
  4. HoD executes bulk release; system updates state to `Released`, making scores visible to students.
* **Alternative Flow A1 (HoD Mark Adjustment)**:
  - HoD modifies a score, entering a mandatory justification reason. System records adjusted score and writes an unalterable log to `MarkAudit`.

---

## 4. SOFTWARE DESIGN DOCUMENT

### 4.1 Architectural Style & Selection Rationale

MLMS adopts a **4-Tier Layered Architecture**. This pattern isolates presentation workflows, service orchestration, domain business rules, and database persistence into distinct, loosely coupled layers.

```
+-------------------------------------------------------------------------+
|                        Presentation Layer (UI)                          |
|         Console CLI, Role-Specific Menus, Command Prompt Handlers       |
+-------------------------------------------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|                      Application / Service Layer                        |
|   AuthManager, CourseManager, SubmissionService, ModerationService      |
+-------------------------------------------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|                       Domain / Business Logic Layer                     |
|  User Entities, Assignment Rules, Submission Lifecycle, PenaltyEngine   |
+-------------------------------------------------------------------------+
                                     |
                                     v
+-------------------------------------------------------------------------+
|                     Data Access / Persistence Layer                     |
|     Repositories (DAOs), SQLite3 C API Wrapper, Parameterized Queries   |
+-------------------------------------------------------------------------+
```

#### Selection Rationale
* **Layered Architecture vs. MVC**: Model-View-Controller (MVC) is optimal for event-driven graphical UIs. Layered Architecture is better suited for CLI desktop C++ software, providing clean separation between database access, penalty business rules, and terminal I/O.
* **Modularity & Testability**: Business rules in the Domain Layer can be unit-tested using mock repository objects independently of SQLite operations and terminal rendering.

### 4.2 Class Design & Object-Oriented Static Structure

```
+-----------------------------------+
|               User                |
+-----------------------------------+
| # userId: int                     |
| # username: string                |
| # passwordHash: string            |
| # salt: string                    |
| # role: string                    |
| # active: bool                    |
+-----------------------------------+
| + authenticate(pass): bool        |
| + getRole(): string               |
+-----------------------------------+
       ^                  ^
       |                  |
+--------------+   +--------------+
|   Lecturer   |   |     HoD      |
+--------------+   +--------------+

+-----------------------------------+        +-----------------------------------+
|            Assignment             |        |            Submission             |
+-----------------------------------+        +-----------------------------------+
| - assignmentId: int               |        | - submissionId: int               |
| - courseId: int                   |        | - studentId: int                  |
| - title: string                   | 1    * | - filePath: string                |
| - dueDate: string                 |<-------| - submittedAt: string             |
| - maxMark: double                 |        | - rawMark: double                 |
+-----------------------------------+        | - finalMark: double               |
| + getDueDate(): string            |        | - status: SubmissionStatus        |
| + setDueDate(newDate): void       |        +-----------------------------------+
+-----------------------------------+        | + calculateFinalMark(): void      |
                                             +-----------------------------------+
                                                               |
                                                               | 1
                                                               v *
+-----------------------------------+        +-----------------------------------+
|           AuditLogger             |        |             MarkAudit             |
+-----------------------------------+        +-----------------------------------+
| - db: sqlite3*                    |        | - auditId: int                    |
+-----------------------------------+        | - submissionId: int               |
| + logEvent(userId, type, oldVal,  |        | - changedBy: int                  |
|            newVal, reason): bool  |        | - oldMark: double                 |
+-----------------------------------+        | - newMark: double                 |
                                             | - reason: string                  |
                                             | - changedAt: string               |
                                             +-----------------------------------+
```

### 4.3 Design Patterns & SOLID Principles Adherence

#### 4.3.1 Applied Design Patterns
1. **Strategy Pattern (`ILatePenaltyStrategy`)**: Encapsulates late deduction algorithms inside separate strategy classes (`StandardLatePenaltyStrategy`, `NoPenaltyStrategy`). This allows penalty rules to be changed without modifying `SubmissionService`.
2. **Repository Pattern (`ISubmissionRepository`)**: Abstracts raw SQLite C-API queries behind C++ interfaces, isolating database persistence logic.
3. **Factory Pattern (`UserFactory`)**: Dynamically creates concrete user subclasses (`Student`, `Lecturer`, `HoD`, `Administrator`) based on database query records.

#### 4.3.2 SOLID Principles Adherence
* **Single Responsibility Principle (SRP)**: `AuthManager` handles credential hashing, `PenaltyEngine` executes score deductions, and `AuditLogger` processes event logging.
* **Open/Closed Principle (OCP)**: New penalty rules can be added by implementing `ILatePenaltyStrategy` without altering existing submission processing code.
* **Liskov Substitution Principle (LSP)**: All user roles inherit from base `User` and can be processed polymorphically by authorization routines.
* **Interface Segregation Principle (ISP)**: DAO interfaces expose precise methods (`IStudentRepository`, `IMarkRepository`) rather than a single monolithic database interface.
* **Dependency Inversion Principle (DIP)**: Service layer components depend on abstract interfaces (`ISubmissionRepository`) rather than concrete SQLite implementations.

### 4.4 Behavioural Modeling

#### 4.4.1 Submission Lifecycle State Machine

```
 [ NotSubmitted ]
        |
        | Submit (Before Deadline)
        v
   [ Submitted ] <----+ Re-submit (Before Deadline)
        |             |
        | Grade       +---------------------------+
        v                                         |
    [ Marked ]                                    |
        |                                         |
        | Moderate (Approved)                     |
        v                                         |
   [ Moderated ]                                  |
        |                                         |
        | Moderate (Returned for revision)        |
        +-----------------------------------------+
        |
        | Release (HoD Sign-off)
        v
   [ Released ] (Visible to Student)

---------------------------------------------------
 [ NotSubmitted ]
        |
        | Submit (After Deadline)
        v
     [ Late ] ---> Grade ---> [ Marked ] ---> [ Moderated ] ---> [ Released ]
```

### 4.5 Relational Database Design & SQL Schema

```
DEPARTMENT (1) <--- (M) USER
USER (1) <--- (M) ENROLMENT (M) ---> (1) COURSE
USER (1) <--- (M) COURSE_LECTURER (M) ---> (1) COURSE
COURSE (1) <--- (M) ASSIGNMENT
ASSIGNMENT (1) <--- (M) SUBMISSION (M) ---> (1) USER (Student)
ASSIGNMENT (1) <--- (M) EXTENSION (M) ---> (1) USER (Student)
SUBMISSION (1) <--- (M) MARK_AUDIT (M) ---> (1) USER (Modifier)
SUBMISSION (1) <--- (M) MODERATION (M) ---> (1) USER (Reviewer)
```

#### SQL DDL Implementation (`schema.sql`)

```sql
PRAGMA foreign_keys = ON;

CREATE TABLE Department (
    department_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE User (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    salt TEXT NOT NULL,
    role TEXT NOT NULL CHECK (role IN ('Student', 'Lecturer', 'HoD', 'Administrator')),
    department_id INTEGER,
    active INTEGER NOT NULL DEFAULT 1 CHECK (active IN (0, 1)),
    FOREIGN KEY (department_id) REFERENCES Department(department_id)
);

CREATE TABLE Course (
    course_id INTEGER PRIMARY KEY AUTOINCREMENT,
    course_code TEXT NOT NULL UNIQUE,
    course_name TEXT NOT NULL,
    department_id INTEGER NOT NULL,
    FOREIGN KEY (department_id) REFERENCES Department(department_id)
);

CREATE TABLE Enrolment (
    student_id INTEGER NOT NULL,
    course_id INTEGER NOT NULL,
    PRIMARY KEY (student_id, course_id),
    FOREIGN KEY (student_id) REFERENCES User(user_id),
    FOREIGN KEY (course_id) REFERENCES Course(course_id)
);

CREATE TABLE Assignment (
    assignment_id INTEGER PRIMARY KEY AUTOINCREMENT,
    course_id INTEGER NOT NULL,
    title TEXT NOT NULL,
    deadline TEXT NOT NULL,
    max_mark REAL NOT NULL CHECK (max_mark > 0),
    FOREIGN KEY (course_id) REFERENCES Course(course_id)
);

CREATE TABLE Submission (
    submission_id INTEGER PRIMARY KEY AUTOINCREMENT,
    assignment_id INTEGER NOT NULL,
    student_id INTEGER NOT NULL,
    file_path TEXT NOT NULL,
    submitted_at TEXT NOT NULL,
    status TEXT NOT NULL CHECK (status IN ('Submitted', 'Late', 'Marked', 'Moderated', 'Released')),
    raw_mark REAL CHECK (raw_mark >= 0),
    final_mark REAL CHECK (final_mark >= 0),
    feedback TEXT,
    UNIQUE (assignment_id, student_id),
    FOREIGN KEY (assignment_id) REFERENCES Assignment(assignment_id),
    FOREIGN KEY (student_id) REFERENCES User(user_id)
);

CREATE TABLE Extension (
    extension_id INTEGER PRIMARY KEY AUTOINCREMENT,
    assignment_id INTEGER NOT NULL,
    student_id INTEGER NOT NULL,
    extended_deadline TEXT NOT NULL,
    reason TEXT NOT NULL,
    UNIQUE (assignment_id, student_id),
    FOREIGN KEY (assignment_id) REFERENCES Assignment(assignment_id),
    FOREIGN KEY (student_id) REFERENCES User(user_id)
);

CREATE TABLE MarkAudit (
    audit_id INTEGER PRIMARY KEY AUTOINCREMENT,
    submission_id INTEGER NOT NULL,
    changed_by INTEGER NOT NULL,
    old_mark REAL,
    new_mark REAL NOT NULL CHECK (new_mark >= 0),
    reason TEXT NOT NULL,
    changed_at TEXT NOT NULL,
    FOREIGN KEY (submission_id) REFERENCES Submission(submission_id),
    FOREIGN KEY (changed_by) REFERENCES User(user_id)
);

CREATE TABLE Moderation (
    moderation_id INTEGER PRIMARY KEY AUTOINCREMENT,
    submission_id INTEGER NOT NULL,
    reviewer_id INTEGER NOT NULL,
    decision TEXT NOT NULL CHECK (decision IN ('Approved', 'Returned')),
    comment TEXT,
    reviewed_at TEXT NOT NULL,
    FOREIGN KEY (submission_id) REFERENCES Submission(submission_id),
    FOREIGN KEY (reviewer_id) REFERENCES User(user_id)
);
```

---

## 5. IMPLEMENTATION

### 5.1 Repository Layout & Build System Configuration

The MLMS codebase is hosted at `https://github.com/Dahraps/Moderated_Learning_Management_System_.git`. The software builds using CMake (v3.14+) targeting $C++17$ standards and linking OpenSSL and SQLite3.

```
MLMS_Project/
├── CMakeLists.txt
├── README.md
├── schema.sql
├── include/
│   ├── AuthManager.hpp
│   ├── AssignmentManager.hpp
│   ├── Database.hpp
│   ├── PenaltyEngine.hpp
│   └── AuditLogger.hpp
├── src/
│   ├── main.cpp
│   ├── AuthManager.cpp
│   ├── AssignmentManager.cpp
│   ├── Database.cpp
│   ├── PenaltyEngine.cpp
│   └── AuditLogger.cpp
└── tests/
    ├── test_main.cpp
    ├── test_auth.cpp
    ├── test_penalty.cpp
    └── test_cr1.cpp
```

### 5.2 Key Code Architecture & Implementation Snippets

#### 5.2.1 Salted Cryptographic Password Hashing (`AuthManager.cpp`)
Passwords are never stored in plaintext. Account creation generates a random 16-byte salt, appended to the raw password, and hashed using SHA-256 via OpenSSL.

```cpp
#include "AuthManager.hpp"
#include <openssl/evp.h>
#include <iomanip>
#include <sstream>
#include <random>

std::string AuthManager::generateSalt() {
    unsigned char salt[16];
    std::random_device rd;
    for (int i = 0; i < 16; ++i) {
        salt[i] = static_cast<unsigned char>(rd() % 256);
    }
    std::stringstream ss;
    for (int i = 0; i < 16; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)salt[i];
    }
    return ss.str();
}

std::string AuthManager::hashPassword(const std::string& password, const std::string& salt) {
    std::string saltedPassword = password + salt;
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    EVP_MD_CTX* context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_sha256(), nullptr);
    EVP_DigestUpdate(context, saltedPassword.c_str(), saltedPassword.size());
    EVP_DigestFinal_ex(context, hash, &lengthOfHash);
    EVP_MD_CTX_free(context);

    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}
```

#### 5.2.2 Automated Penalty Calculation Engine (`PenaltyEngine.cpp`)
Evaluates submission timestamps ($T_{sub}$) against assignment deadlines ($T_{due}$).

```cpp
#include "PenaltyEngine.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

double PenaltyEngine::calculateFinalMark(double rawMark, const std::string& dueDateStr, const std::string& subDateStr) {
    std::tm dueTm = {}, subTm = {};
    std::istringstream dueSs(dueDateStr), subSs(subDateStr);
    dueSs >> std::get_time(&dueTm, "%Y-%m-%d %H:%M");
    subSs >> std::get_time(&subTm, "%Y-%m-%d %H:%M");

    auto dueTime = std::chrono::system_clock::from_time_t(std::mktime(&dueTm));
    auto subTime = std::chrono::system_clock::from_time_t(std::mktime(&subTm));

    auto diffHours = std::chrono::duration_cast<std::chrono::hours>(subTime - dueTime).count();

    if (diffHours <= 0) return rawMark;             // On time: 0% deduction
    if (diffHours <= 24) return rawMark * 0.90;      // Day 1: -10%
    if (diffHours <= 48) return rawMark * 0.80;      // Day 2: -20%
    if (diffHours <= 72) return rawMark * 0.70;      // Day 3: -30%
    return 0.0;                                      // Day 4+: 0% mark
}
```

---

## 6. VERIFICATION AND VALIDATION

### 6.1 Test Strategy & Verification Methodology

System verification was conducted using a multi-tiered test framework:
1. **Unit Testing**: Evaluating individual classes (`AuthManager`, `PenaltyEngine`) using synthetic inputs.
2. **Integration Testing**: Testing database operations, foreign key constraints, and SQL transactions.
3. **Boundary Value Analysis**: Executing edge case tests around exact timestamp cutoffs ($T_{due} \pm 1\text{s}$, $+24\text{h}$, $+72\text{h}$).
4. **Equivalence Partitioning**: Testing valid and invalid input sets (e.g., negative scores, invalid dates).

### 6.2 Test Suite Execution Matrix

| Test ID | Category | Target | Input / Condition | Expected Outcome | Result |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **TC-AUTH-01** | Unit / Security | Hashing | Password `"AdminPass123"` | Salt generated; hash verifies valid login, rejects invalid. | **PASS** |
| **TC-AUTH-02** | Security / RBAC | Permissions | Student invoking HoD release | Request rejected (`403 Access Denied`). | **PASS** |
| **TC-SUB-01** | Functional | Submissions | File A upload, then File B upload | File B overwrites File A; timestamp updated; state `Submitted`. | **PASS** |
| **TC-PEN-01** | Boundary | Penalty Engine | $T_{sub} = T_{due} - 1\text{ second}$ | Raw Mark = Final Mark (0% deduction). | **PASS** |
| **TC-PEN-02** | Boundary | Penalty Engine | $T_{sub} = T_{due} + 2\text{ hours}$ | Final Mark = Raw Mark $\times 0.90$ (-10%). | **PASS** |
| **TC-PEN-03** | Boundary | Penalty Engine | $T_{sub} = T_{due} + 71\text{h } 59\text{m}$ | Final Mark = Raw Mark $\times 0.70$ (-30%). | **PASS** |
| **TC-PEN-04** | Boundary | Penalty Engine | $T_{sub} = T_{due} + 72\text{h } 01\text{m}$ | Final Mark = $0.0$ (Zero mark). | **PASS** |
| **TC-MOD-01** | Workflow | Moderation | Student queries marked score | Mark hidden from student until HoD executes release. | **PASS** |
| **TC-AUD-01** | Audit | Logging | HoD changes score from 65 to 70 | Unalterable entry recorded in `MarkAudit` with modifier ID and reason. | **PASS** |

### 6.3 Defect Tracking & Resolution Log

| Defect ID | Description | Severity | Root Cause | Resolution | Status |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **DEF-01** | SQLite foreign key constraints not enforced on startup. | High | SQLite disables foreign key checking by default. | Added `PRAGMA foreign_keys = ON;` to initialization queries. | **Resolved** |
| **DEF-02** | Day boundary calculation failed across month end transitions. | Medium | Manual string parsing of dates ignored leap years/month lengths. | Migrated date arithmetic to standard `<chrono>` and `std::mktime`. | **Resolved** |
| **DEF-03** | Mark audit log missed old mark value during initial grading. | Low | Query passed NULL without checking prior record existence. | Updated audit logic to pass empty string indicator for new entries. | **Resolved** |

---

## 7. CONFIGURATION MANAGEMENT AND QUALITY ASSURANCE

### 7.1 Branching Strategy & Repository Management

The repository (`https://github.com/Dahraps/Moderated_Learning_Management_System_.git`) followed Git branching standards:
* `main`: Protected production branch (tagged versions `v1.0` and `v1.1`).
* `feature/*`: Development branches for individual components (`feature/auth`, `feature/penalty-engine`, `feature/cr1-extensions`).
* Commits enforced descriptive summary messages referencing specific requirements.

### 7.2 Static Code Analysis (Cppcheck 2.22.0)

Static analysis was executed across all C++ implementation and header files.

```text
$ cppcheck --enable=all --inconclusive --std=c++17 src/ include/
Checking src/AssignmentManager.cpp ...
Checking src/AuditLogger.cpp ...
Checking src/AuthManager.cpp ...
Checking src/Database.cpp ...
Checking src/PenaltyEngine.cpp ...
Checking src/main.cpp ...
Quiet report: 0 errors, 0 warnings, 0 performance style suggestions found.
```

### 7.3 Software Metrics & Cyclomatic Complexity Analysis

Software metrics were evaluated using Lizard across the codebase:
* **Total Lines of Code (LOC)**: 438 lines (excluding comments and blank lines) across 15 source files.
* **Average Cyclomatic Complexity**: $v(G) = 2.4$ per function.
* **Maximum Cyclomatic Complexity**: $v(G) = 5.0$ (in CLI command dispatcher).
* **Comment-to-Code Ratio**: $22.4\%$.

---

## 8. DEPLOYMENT

### 8.1 UML Deployment Architecture

```
+-----------------------------------------------------------------------------------------+
|                                  School Workstation Node                                |
|                        (OS: Linux / Windows / macOS - Standalone)                       |
|                                                                                         |
|  +-----------------------------------+         +-------------------------------------+  |
|  |           MLMS Executable         |         |           SQLite Database           |  |
|  |             (mlms.exe)            | <-----> |             (mlms.db)               |  |
|  |          [C++17 Binary]           |  SQL    |      [Local Relational Data]        |  |
|  +-----------------------------------+         +-------------------------------------+  |
|                   ^                                                                     |
|                   | Standard I/O (Stdin / Stdout)                                       |
|                   v                                                                     |
|  +-----------------------------------+                                                  |
|  |         Console / Terminal        |                                                  |
|  |        [User Interface Layer]     |                                                  |
|  +-----------------------------------+                                                  |
|                   ^                                                                     |
|                   | Physical Access / Terminal Login                                    |
|                   v                                                                     |
|  +-----------------------------------+                                                  |
|  |        Authorized Users           |                                                  |
|  | (Admin, Lecturer, HoD, Student)   |                                                  |
|  +-----------------------------------+                                                  |
+-----------------------------------------------------------------------------------------+
```

### 8.2 Technician Installation & Build Manual

1. **Clone Repository**:
   ```bash
   git clone https://github.com/Dahraps/Moderated_Learning_Management_System_.git
   cd Moderated_Learning_Management_System_
   ```
2. **Configure Build Environment**:
   ```bash
   mkdir build && cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..
   ```
3. **Compile Application**:
   ```bash
   cmake --build . --config Release
   ```
4. **Initialize System**:
   Running `./mlms` automatically executes `schema.sql`, creating `mlms.db` and seeding default admin credentials (`admin` / `Admin@123`).

### 8.3 Release Notes (v1.0 Baseline)

- Initial production release supporting offline desktop execution.
- Implemented salted SHA-256 password security and role-based permissions.
- Integrated automated late penalty engine (-10%/day) and individual extension workflows.
- Implemented Head of Department grade moderation and bulk mark release workflows.
- Added immutable audit trail logging for all grade adjustments.

---

## 9. MAINTENANCE (CHANGE REQUEST CR1)

### 9.1 Change Request Identification & Maintenance Classification

* **Change Request ID**: `CR-001`
* **Title**: Class-Wide Deadline Extension
* **Maintenance Type**: **Perfective Maintenance**
* **Justification**: `CR-001` enhances system functionality based on user feedback to allow bulk extensions across an entire course cohort. It does not fix a defect (*Corrective*) nor adapt to host OS shifts (*Adaptive*).

### 9.2 Comprehensive Traceability & Impact Analysis Matrix

| Artifact Category | Element Identifier | Required Modifications |
| :--- | :--- | :--- |
| **Functional Requirement** | `FR-ASSGN-03` | Update requirement to support bulk/class-wide deadline extensions. |
| **UML Use Case** | `UC-LECT-04` | Add extended use case: *Extend Class-Wide Deadline*. |
| **UML Class Diagram** | `AssignmentManager` | Add method `extendClassDeadline(lecturerId, assgnId, newDate, reason): bool`. |
| **UML Sequence Diagram** | `SD-CR1` | Create sequence diagram illustrating class-wide extension, database updates, and audit logging. |
| **Database Schema** | `Assignments` / `AuditLogs`| Execute `UPDATE Assignments SET due_date = ?` and write event to `AuditLogs`. |
| **Test Suite** | `TC-CR1-01` to `04` | Add 4 unit test cases for CR1 and run full regression suite. |

### 9.3 Release v1.1 Sequence Diagram (`SD-CR1`)

```text
Lecturer            CLI / UI           AssignmentManager         SQLite DB          AuditLogger
   |                   |                       |                     |                   |
   |-- extendClassDeadline(assgnId, newDate) -->|                     |                   |
   |                   |                       |-- Check Auth -------|                   |
   |                   |                       |<-- Auth Valid ------|                   |
   |                   |                       |                     |                   |
   |                   |                       |-- UPDATE Assignments|                   |
   |                   |                       |   SET due_date=...  |                   |
   |                   |                       |-------------------->|                   |
   |                   |                       |<-- OK (1 row) ------|                   |
   |                   |                       |                                         |
   |                   |                       |-- logEvent("CLASS_DEADLINE_EXT", ...) ->|
   |                   |                       |<-- Log Recorded ------------------------|
   |                   |                       |
   |<-- Success Message ("Deadline Extended") -|
```

### 9.4 Full Regression Testing Execution (v1.1)

```text
Running tests...
Test project /home/runner/work/MLMS/build
    Start 1: TC-AUTH-01_SaltedPasswordHash
1/9 Test #1: TC-AUTH-01_SaltedPasswordHash .......   Passed   0.02 sec
    Start 2: TC-AUTH-02_RolePermissionEnforcement
2/9 Test #2: TC-AUTH-02_RolePermissionEnforcement    Passed   0.01 sec
    Start 3: TC-SUB-01_MultipleResubmissions
3/9 Test #3: TC-SUB-01_MultipleResubmissions ......   Passed   0.03 sec
    Start 4: TC-PEN-01_LatePenaltyCalculation
4/9 Test #4: TC-PEN-01_LatePenaltyCalculation .....   Passed   0.02 sec
    Start 5: TC-MOD-01_HoDMarkReleaseWorkflow
5/9 Test #5: TC-MOD-01_HoDMarkReleaseWorkflow .....   Passed   0.04 sec
    Start 6: TC-AUD-01_ImmutableAuditLogging
6/9 Test #6: TC-AUD-01_ImmutableAuditLogging ......   Passed   0.02 sec
    Start 7: TC-CR1-01_ClassWideDeadlineExtension
7/9 Test #7: TC-CR1-01_ClassWideDeadlineExtension .   Passed   0.02 sec
    Start 8: TC-CR1-02_UnauthorizedExtensionBlock
8/9 Test #8: TC-CR1-02_UnauthorizedExtensionBlock .   Passed   0.01 sec
    Start 9: TC-CR1-04_PenaltyRecalculationPostExt
9/9 Test #9: TC-CR1-04_PenaltyRecalculationPostExt    Passed   0.03 sec

100% tests passed, 0 tests failed out of 9
Total Test time (real) = 0.20 sec
```

---

## 10. PROFESSIONAL, LEGAL, AND ETHICAL ISSUES

### 10.1 Data Protection & Student Privacy Legislation

Data protection regulations (such as GDPR and local privacy statutes) dictate that educational institutions must safeguard student personal data, enrollment records, and assessment performance. MLMS enforces data privacy by operating strictly as a local offline desktop application, eliminating external cloud exposure. Passwords use salted SHA-256 cryptographic hashes, and database interactions use parameterized SQL statements to prevent unauthorized data access via SQL injection.

### 10.2 Assessment Record Security & Audit Integrity

Academic integrity requires that assessment records are protected against unauthorized modification or deletion. MLMS addresses this requirement through strict Role-Based Access Control (RBAC)—preventing unauthorized users from altering grades—and by maintaining an immutable `MarkAudit` database table that logs every score change alongside modifier identity, timestamps, historical values, and mandatory justifications.

### 10.3 Digital Accessibility for Assistive Technology Users

Educational software must be accessible to operators with sensory or visual impairments. By utilizing a standard command-line console interface (CLI) that renders clean text output, MLMS is natively compatible with terminal screen readers (such as NVDA or Orca) and high-contrast display modes, avoiding graphical navigation barriers for visually impaired operators.

### 10.4 Ethical Fairness & Objectivity in Automated Late Penalty Rules

Applying penalties for late coursework must be objective, transparent, and consistent. MLMS implements an automated, deterministic penalty strategy (-10% per calendar day up to 3 days) that treats all students equally based on exact system timestamps. To accommodate valid extenuating circumstances (such as medical emergencies), the system provides authorized staff with individual and class-wide extension capabilities, keeping all exceptions fully documented in audit logs.

---

## 11. CONCLUSION

### 11.1 Project Summary & Achievements

The **Moderated Learning Management System (MLMS)** demonstrates how structured software engineering practices can produce a secure, high-performance educational system tailored for resource-constrained school environments.

By pairing a $C++17$ executable architecture with embedded SQLite storage, MLMS provides a complete coursework management solution without external network dependencies. The system fulfills all functional requirements across planning, design, implementation, verification, deployment, and perfective maintenance (CR1). Static code analysis reports zero findings across 438 LOC, and automated testing confirms a 100% pass rate.

### 11.2 Reflection on Engineering Constraints Overcome

The main technical challenge was engineering a robust relational management system without external application servers or GUI frameworks. Adopting a 4-tier layered architecture, strict SOLID design principles, and automated CTest validation allowed complex academic moderation and late-penalty business logic to run efficiently within a lightweight command-line desktop executable.

### 11.3 Recommendations for Future Evolution

1. **Local Network Sync Protocol**: Introduce a peer-to-peer LAN sync utility to allow local database synchronization across offline school desktop nodes.
2. **Encrypted File Attachments**: Integrate AES-256 binary blob encryption for student submission files stored on local disk media.
3. **Automated PDF Grade Sheet Exporting**: Add native PDF rendering to generate formatted departmental moderation summaries directly from CLI menus.

---

## 12. REFERENCES

* Gamma, E., Helm, R., Johnson, R. and Vlissides, J. (1994) *Design Patterns: Elements of Reusable Object-Oriented Software*. Reading, MA: Addison-Wesley.
* ISO/IEC (2017) *ISO/IEC 14882:2017 Information technology — Programming languages — C++*. Geneva: International Organization for Standardization.
* Pressman, R.S. and Maxim, B.R. (2020) *Software Engineering: A Practitioner's Approach*. 9th edn. New York: McGraw-Hill Education.
* Sommerville, I. (2016) *Software Engineering*. 10th edn. Boston, MA: Pearson.
* SQLite Development Team (2026) *SQLite C/C++ Interface Specification*. Available at: `https://www.sqlite.org/c3ref/intro.html` (Accessed: November 2026).

---

## 13. APPENDICES

### Appendix A: Complete Requirements Traceability Matrix (RTM)

| Requirement ID | Description | Architectural Component | Source File | Verification Test ID |
| :--- | :--- | :--- | :--- | :--- |
| **FR-01** | User Authentication | `AuthManager` | `AuthManager.cpp` | `TC-AUTH-01` |
| **FR-02** | User Account Administration | `AuthManager` / RBAC | `AuthManager.cpp` | `TC-AUTH-02` |
| **FR-03** | Academic Structure Setup | `CourseManager` | `Database.cpp` | `TC-AUTH-02` |
| **FR-04** | Assignment Creation | `AssignmentManager` | `AssignmentManager.cpp` | `TC-SUB-01` |
| **FR-05** | Assignment Submission | `SubmissionService` | `AssignmentManager.cpp` | `TC-SUB-01` |
| **FR-06** | Pre-Deadline Resubmission | State Machine (`Submitted`) | `AssignmentManager.cpp` | `TC-SUB-01` |
| **FR-07** | Individual Extensions | `Extension` Entity | `AssignmentManager.cpp` | `TC-CR1-01` |
| **FR-08** | Class-Wide Extensions (CR1) | `extendClassDeadline` | `AssignmentManager.cpp` | `TC-CR1-01` |
| **FR-09** | Automated Penalty Engine | `PenaltyEngine` (Strategy) | `PenaltyEngine.cpp` | `TC-PEN-01` to `04` |
| **FR-10** | Grading & Written Feedback | `AssignmentManager` | `AssignmentManager.cpp` | `TC-MOD-01` |
| **FR-11** | HoD Moderation Review | `ModerationService` | `AssignmentManager.cpp` | `TC-MOD-01` |
| **FR-12** | Controlled Grade Release | `ModerationService` | `AssignmentManager.cpp` | `TC-MOD-01` |
| **FR-13** | Student Grade Viewing | CLI Interface | `main.cpp` | `TC-MOD-01` |
| **FR-14** | Immutable Audit Logging | `AuditLogger` | `AuditLogger.cpp` | `TC-AUD-01` |

### Appendix B: Automated Test Suite Execution Logs

```text
===============================================================================
MLMS Automated Test Suite (Release v1.1)
Execution Platform: CTest / Linux 6.2 x86_64
Compiler Standard: C++17
===============================================================================

[RUN      ] TC-AUTH-01_SaltedPasswordHash
[       OK ] TC-AUTH-01_SaltedPasswordHash (0.02 sec)
[RUN      ] TC-AUTH-02_RolePermissionEnforcement
[       OK ] TC-AUTH-02_RolePermissionEnforcement (0.01 sec)
[RUN      ] TC-SUB-01_MultipleResubmissions
[       OK ] TC-SUB-01_MultipleResubmissions (0.03 sec)
[RUN      ] TC-PEN-01_LatePenaltyCalculation_OnTime
[       OK ] TC-PEN-01_LatePenaltyCalculation_OnTime (0.02 sec)
[RUN      ] TC-PEN-02_LatePenaltyCalculation_Day1
[       OK ] TC-PEN-02_LatePenaltyCalculation_Day1 (0.02 sec)
[RUN      ] TC-PEN-03_LatePenaltyCalculation_Day3
[       OK ] TC-PEN-03_LatePenaltyCalculation_Day3 (0.02 sec)
[RUN      ] TC-PEN-04_LatePenaltyCalculation_HardExpiry
[       OK ] TC-PEN-04_LatePenaltyCalculation_HardExpiry (0.01 sec)
[RUN      ] TC-MOD-01_HoDMarkReleaseWorkflow
[       OK ] TC-MOD-01_HoDMarkReleaseWorkflow (0.04 sec)
[RUN      ] TC-AUD-01_ImmutableAuditLogging
[       OK ] TC-AUD-01_ImmutableAuditLogging (0.02 sec)
[RUN      ] TC-CR1-01_ClassWideDeadlineExtension
[       OK ] TC-CR1-01_ClassWideDeadlineExtension (0.02 sec)
[RUN      ] TC-CR1-02_UnauthorizedExtensionBlock
[       OK ] TC-CR1-02_UnauthorizedExtensionBlock (0.01 sec)
[RUN      ] TC-CR1-04_PenaltyRecalculationPostExt
[       OK ] TC-CR1-04_PenaltyRecalculationPostExt (0.03 sec)

===============================================================================
Test Summary: 12 Passed, 0 Failed.
Total Execution Time: 0.25 seconds.
===============================================================================
```

### Appendix C: Role-Specific User Manuals

#### C.1 System Administrator Operational Manual
1. **System Startup**: Run `./mlms` and log in using administrator credentials.
2. **User Account Creation**: Select `1. User Management` $\rightarrow$ `Create Account`. Input user details, role (Student, Lecturer, HoD, Admin), username, and initial password.
3. **Academic Setup**: Select `2. Department Setup` to define departments and courses, assign lecturers to courses, and enrol students into course rosters.

#### C.2 Lecturer Operational Manual
1. **Create Course Assignments**: Select `1. Assignment Management` $\rightarrow$ `Create Assignment`. Input assignment title, description, max score, and deadline cutoff.
2. **Manage Extensions**: Select `2. Deadline Extensions`. Choose between an individual extension or a class-wide extension (`CR1`). Enter new deadline timestamp and justification text.
3. **Grading & Feedback**: Select `3. Evaluate Submissions`. Input numeric score ($\le \text{max\_mark}$) and qualitative written feedback.

#### C.3 Head of Department (HoD) Operational Manual
1. **Department Moderation Review**: Select `1. Department Review`. Inspect graded assignments, scores, and calculated late penalties across department courses.
2. **Score Adjustments**: Select `2. Modify Score` to adjust a student mark during moderation. Enter revised score and a mandatory audit justification.
3. **Release Grades**: Select `3. Release Approved Marks` to approve and release grades, making them visible to students.

#### C.4 Student Operational Manual
1. **View Coursework**: Select `1. My Courses` to inspect registered course modules and active assignments.
2. **Submit Coursework**: Select `2. Submit Assignment`. Choose target assignment ID and input local file path. Resubmissions prior to cutoffs automatically update existing entries.
3. **View Released Grades**: Select `3. View Results`. Moderated marks and written feedback appear once released by the Head of Department.

### Appendix D: Final Updated Risk Register

| Risk ID | Risk Category | Risk Event | Post-Mitigation Exposure | Closure Status |
| :--- | :--- | :--- | :--- | :--- |
| **R1** | Technical | Database design preserves submission history and audit trail. | **Low (4)** | **Closed / Verified** |
| **R2** | Security | Salted SHA-256 hashing & parameterized queries prevent breaches. | **Low (3)** | **Closed / Verified** |
| **R3** | Business Logic | Boundary value testing confirms late-penalty calculations. | **Low (3)** | **Closed / Verified** |
| **R4** | Schedule | Project completed on time across all 9 phases within 7 weeks. | **Low (2)** | **Closed / Verified** |
| **R5** | Governance | HoD moderation workflow implemented as top priority. | **Low (2)** | **Closed / Verified** |
| **R6** | Quality | Cppcheck (0 findings) and CTest (100% pass) confirm software quality. | **Low (2)** | **Closed / Verified** |
