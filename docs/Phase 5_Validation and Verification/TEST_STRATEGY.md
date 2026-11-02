# Test Strategy and Test Plan

**Project:** Moderated Learning Management System (LMS)  
**Phase:** Phase 5 — Verification and Validation  
**Deliverable:** Test Strategy (3 marks)  
**Version:** 1.0

---

## 1. Introduction

This test plan defines the strategy for verifying and validating the Moderated Learning Management System (LMS). It maps each V-Model testing level—unit, integration, system, and acceptance—to the development artefact it verifies and states entry and exit criteria for each level.

It also applies **equivalence partitioning** and **boundary value analysis** to test inputs and business rules, using assignment deadlines and late penalties as examples.

## 2. Test Strategy

Testing will follow the V-Model, progressing from individual components to integrated system behaviour and then to stakeholder acceptance.

The strategy is to:

1. Verify individual functions and classes through unit testing.
2. Verify interactions between components and the database through integration testing.
3. Verify end-to-end LMS workflows against requirements and use cases through system testing.
4. Verify that the system meets stakeholder needs and acceptance criteria through acceptance testing.
5. Design representative tests using equivalence partitioning and test rule boundaries using boundary value analysis.
6. Record test outcomes and unresolved issues. A planned test must not be described as passed until it has actually been executed.

## 3. V-Model Mapping: Test Levels, Artefacts, and Criteria

| V-Model testing level | Development artefact verified | What is checked | Entry criteria | Exit criteria |
|---|---|---|---|---|
| **Unit testing** | Detailed class design, function specifications, and source code | Individual functions/classes and business rules, such as input validation and late-penalty calculation | Relevant source code and unit-level design are available; code builds; unit test cases are prepared | Planned unit tests have been executed; results are recorded; failures are fixed or documented for follow-up |
| **Integration testing** | Component design, interface definitions, and database design/schema | Whether connected components exchange data correctly; whether database operations and error handling work across component boundaries | Components to be integrated are built; their relevant unit tests are complete; integration environment and test data are available | Planned integration tests have been executed; interactions and persistence behave as expected or defects are documented |
| **System testing** | Software Requirements Specification (SRS), functional requirements, and use-case specifications | Complete workflows through the integrated LMS, including normal and exception flows | Integrated system build is available; configuration and test database are ready; integration testing is complete | Planned system scenarios have been executed; actual results are recorded; failures and unresolved defects are documented |
| **Acceptance testing** | Stakeholder requirements, business rules, and acceptance criteria | Whether the LMS supports intended user tasks and satisfies agreed requirements | System testing is complete; system is ready for evaluation; acceptance scenarios and expected outcomes are agreed | Each acceptance criterion is recorded as met, not met, or blocked; outstanding issues and acceptance outcome are documented; stakeholder approval is recorded if obtained |

**Traceability:** Each test case should identify the requirement, use case, design artefact, or component it verifies. If a required artefact or requirement is unavailable or ambiguous, record that limitation rather than assuming its content.

## 4. Equivalence Partitioning

Equivalence partitioning divides possible inputs into classes expected to produce similar behaviour. Instead of testing every possible value, representative values are selected from each class.

### 4.1 Assignment deadline and late submission

The following partitions are based on the stated rule that a late assignment loses 10% of the available mark per day for up to three days, and receives zero after that period.

| Input partition | Example test input | Expected behaviour |
|---|---|---|
| Submitted before deadline | Submission time is earlier than the deadline | Treat as on time; no late penalty |
| Submitted exactly at deadline | Submission time equals the deadline | Treat as on time, subject to the approved deadline interpretation |
| Late within the permitted period | Submission is 1, 2, or 3 days late | Apply the specified daily penalty |
| Later than the permitted period | Submission is 4 days late | Award zero for the assignment |
| Invalid or missing submission data | Required submission data is absent or invalid | Reject the input or report a validation error, according to the requirements |

### 4.2 Authentication input

| Input partition | Example test input | Expected behaviour |
|---|---|---|
| Valid credentials | Existing account and correct credentials | Authentication succeeds |
| Incorrect credentials | Existing account and incorrect password | Authentication fails |
| Unknown account | Account does not exist | Authentication fails |
| Missing required field | Username or password is empty | Input is rejected or a validation message is returned |

These authentication examples apply only if authentication is part of the system requirements and implementation.

## 5. Boundary Value Analysis

Boundary value analysis concentrates on values at and around the limits of a rule. For the late-submission rule, important day-count boundaries are 0, 1, 2, 3, and 4 days late.

### 5.1 Late-penalty boundary examples

For illustration, assume the assignment initially earns 100 marks and each late day deducts 10 marks from the available mark.

| Days late | Illustrative expected mark out of 100 | Explanation |
|---:|---:|---|
| 0 | 100 | Submitted on time; no penalty |
| 1 | 90 | One daily penalty |
| 2 | 80 | Two daily penalties |
| 3 | 70 | Three daily penalties |
| 4 | 0 | Beyond the three-day permitted period |

These figures illustrate the rule for an assignment initially earning 100 marks. For an assignment with a different available mark, calculate the penalty using the interpretation approved for the project.

### 5.2 Deadline timestamp boundaries

Where submission time is represented by a timestamp, the test design should include:

- A timestamp immediately before the deadline.
- A timestamp exactly at the deadline.
- A timestamp immediately after the deadline.
- Values at the transition from three days late to more than three days late.

**Clarification required:** The stated rule does not specify how partial days are counted or the precise timestamp cut-off. Confirm these details with the stakeholder before finalising expected results for submissions shortly after the deadline or near the three-day limit. Do not silently assume that every partial day counts as a full day.

## 6. Test Case Recording

For each test designed from the partitions and boundaries above, record:

- Test case ID.
- Related requirement or use case.
- Preconditions and test data.
- Steps or input value.
- Expected result.
- Actual result after execution.
- Pass/fail status.
- Defect ID, if a failure is found.

The examples in this document define the strategy and expected behaviours; they are not evidence that tests have already been executed.

## 7. Completion Criteria for This Deliverable

This test-strategy deliverable is complete when:

- All four V-Model testing levels are mapped to the artefacts they verify.
- Entry and exit criteria are defined for every testing level.
- Equivalence partitions are identified for relevant inputs.
- Boundary values are identified for the deadline and late-penalty rule.
- Assumptions and unresolved rule ambiguities are clearly stated.

---

**End of Test Strategy and Test Plan**
