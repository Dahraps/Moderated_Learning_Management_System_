
PRAGMA foreign_keys = ON;

CREATE TABLE Department (
    department_id INTEGER PRIMARY KEY,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE User (
    user_id INTEGER PRIMARY KEY,
    username TEXT NOT NULL UNIQUE,
    password_hash TEXT NOT NULL,
    salt TEXT NOT NULL,
    role TEXT NOT NULL CHECK (
        role IN ('Student', 'Lecturer', 'HoD', 'Administrator')
    ),
    department_id INTEGER,
    active INTEGER NOT NULL DEFAULT 1
        CHECK (active IN (0, 1)),
    FOREIGN KEY (department_id)
        REFERENCES Department(department_id)
);

CREATE TABLE Course (
    course_id INTEGER PRIMARY KEY,
    course_code TEXT NOT NULL UNIQUE,
    course_name TEXT NOT NULL,
    department_id INTEGER NOT NULL,
    FOREIGN KEY (department_id)
        REFERENCES Department(department_id)
);

CREATE TABLE CourseLecturer (
    course_id INTEGER NOT NULL,
    lecturer_id INTEGER NOT NULL,
    PRIMARY KEY (course_id, lecturer_id),
    FOREIGN KEY (course_id)
        REFERENCES Course(course_id),
    FOREIGN KEY (lecturer_id)
        REFERENCES User(user_id)
);

CREATE TABLE Enrolment (
    student_id INTEGER NOT NULL,
    course_id INTEGER NOT NULL,
    PRIMARY KEY (student_id, course_id),
    FOREIGN KEY (student_id)
        REFERENCES User(user_id),
    FOREIGN KEY (course_id)
        REFERENCES Course(course_id)
);

CREATE TABLE Assignment (
    assignment_id INTEGER PRIMARY KEY,
    course_id INTEGER NOT NULL,
    title TEXT NOT NULL,
    deadline TEXT NOT NULL,
    max_mark REAL NOT NULL CHECK (max_mark > 0),
    FOREIGN KEY (course_id)
        REFERENCES Course(course_id)
);

CREATE TABLE Submission (
    submission_id INTEGER PRIMARY KEY,
    assignment_id INTEGER NOT NULL,
    student_id INTEGER NOT NULL,
    file_path TEXT NOT NULL,
    submitted_at TEXT NOT NULL,
    status TEXT NOT NULL CHECK (
        status IN (
            'Submitted', 'Late', 'Marked',
            'Moderated', 'Released'
        )
    ),
    raw_mark REAL CHECK (raw_mark >= 0),
    final_mark REAL CHECK (final_mark >= 0),
    feedback TEXT,
    UNIQUE (assignment_id, student_id),
    FOREIGN KEY (assignment_id)
        REFERENCES Assignment(assignment_id),
    FOREIGN KEY (student_id)
        REFERENCES User(user_id)
);

CREATE TABLE Extension (
    extension_id INTEGER PRIMARY KEY,
    assignment_id INTEGER NOT NULL,
    student_id INTEGER NOT NULL,
    extended_deadline TEXT NOT NULL,
    reason TEXT NOT NULL,
    UNIQUE (assignment_id, student_id),
    FOREIGN KEY (assignment_id)
        REFERENCES Assignment(assignment_id),
    FOREIGN KEY (student_id)
        REFERENCES User(user_id)
);

CREATE TABLE MarkAudit (
    audit_id INTEGER PRIMARY KEY,
    submission_id INTEGER NOT NULL,
    changed_by INTEGER NOT NULL,
    old_mark REAL,
    new_mark REAL NOT NULL CHECK (new_mark >= 0),
    reason TEXT NOT NULL,
    changed_at TEXT NOT NULL,
    FOREIGN KEY (submission_id)
        REFERENCES Submission(submission_id),
    FOREIGN KEY (changed_by)
        REFERENCES User(user_id)
);

CREATE TABLE Moderation (
    moderation_id INTEGER PRIMARY KEY,
    submission_id INTEGER NOT NULL,
    reviewer_id INTEGER NOT NULL,
    decision TEXT NOT NULL CHECK (
        decision IN ('Approved', 'Returned')
    ),
    comment TEXT,
    reviewed_at TEXT NOT NULL,
    mark_audit_id_at_review INTEGER NOT NULL DEFAULT 0,
    FOREIGN KEY (submission_id)
        REFERENCES Submission(submission_id),
    FOREIGN KEY (reviewer_id)
        REFERENCES User(user_id)
);
