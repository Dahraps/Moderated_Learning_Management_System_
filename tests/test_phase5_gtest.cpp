#include <gtest/gtest.h>
#include "mlms/Database.hpp"
#include "mlms/PenaltyStrategy.hpp"
#include "mlms/User.hpp"
#include "mlms/Utility.hpp"

#include <cmath>
#include <stdexcept>
#include <string>

TEST(LatePenaltyTest, NoPenaltyWhenSubmittedOnTime) {
    StandardLatePenalty policy;
    EXPECT_DOUBLE_EQ(policy.penalty(0, 100.0), 0.0);
}

TEST(LatePenaltyTest, AppliesTenPercentPerDayForFirstThreeDays) {
    StandardLatePenalty policy;
    EXPECT_DOUBLE_EQ(policy.penalty(1, 100.0), 10.0);
    EXPECT_DOUBLE_EQ(policy.penalty(2, 100.0), 20.0);
    EXPECT_DOUBLE_EQ(policy.penalty(3, 100.0), 30.0);
}

TEST(LatePenaltyTest, PenaltyBecomesFullAvailableMarkAfterThreeDays) {
    StandardLatePenalty policy;
    EXPECT_DOUBLE_EQ(policy.penalty(4, 100.0), 100.0);
}

TEST(LatePenaltyTest, RejectsInvalidInputs) {
    StandardLatePenalty policy;
    EXPECT_THROW(policy.penalty(-1, 100.0), std::invalid_argument);
    EXPECT_THROW(policy.penalty(1, 0.0), std::invalid_argument);
    EXPECT_THROW(policy.penalty(1, -10.0), std::invalid_argument);
}

TEST(LatePenaltyTest, NoPenaltyStrategyAlwaysReturnsZeroForValidInputs) {
    NoPenaltyStrategy policy;
    EXPECT_DOUBLE_EQ(policy.penalty(4, 100.0), 0.0);
}

TEST(UserFactoryTest, CreatesEachSupportedRole) {
    const std::pair<const char*, const char*> cases[] = {
        {"Student", "Student"}, {"Lecturer", "Lecturer"},
        {"HoD", "HoD"}, {"Administrator", "Administrator"}
    };
    for (const auto& item : cases) {
        auto user = UserFactory::create(7, "test-user", item.first);
        ASSERT_NE(user, nullptr);
        EXPECT_EQ(user->role(), item.second);
        EXPECT_FALSE(user->menu().empty());
    }
}

TEST(UserFactoryTest, RejectsUnknownRole) {
    EXPECT_THROW(UserFactory::create(1, "test-user", "UnknownRole"), std::runtime_error);
}

TEST(TimeValidationTest, ParsesValidTimestampAndOrdersTimes) {
    EXPECT_LT(parseTime("2026-10-10T12:00:00"), parseTime("2026-10-11T12:00:00"));
}

TEST(TimeValidationTest, RejectsMalformedAndImpossibleDates) {
    EXPECT_THROW(parseTime("not-a-date"), std::runtime_error);
    EXPECT_THROW(parseTime("2026-02-30T12:00:00"), std::runtime_error);
}

TEST(DatabaseIntegrationTest, PersistsAndReadsBoundValues) {
    DB db(":memory:");
    db.exec("CREATE TABLE test_items(id INTEGER PRIMARY KEY, label TEXT NOT NULL, score REAL NOT NULL)");
    {
        Statement insert(db, "INSERT INTO test_items(label, score) VALUES(?, ?)");
        insert.bind(1, std::string("assignment"));
        insert.bind(2, 87.5);
        insert.run();
    }
    Statement select(db, "SELECT label, score FROM test_items WHERE id = 1");
    ASSERT_TRUE(select.next());
    EXPECT_EQ(select.str(0), "assignment");
    EXPECT_DOUBLE_EQ(select.number(1), 87.5);
    EXPECT_FALSE(select.next());
}

TEST(DatabaseIntegrationTest, RejectsDuplicatePrimaryKey) {
    DB db(":memory:");
    db.exec("CREATE TABLE test_items(id INTEGER PRIMARY KEY)");
    db.exec("INSERT INTO test_items(id) VALUES(1)");
    EXPECT_THROW(db.exec("INSERT INTO test_items(id) VALUES(1)"), std::runtime_error);
}
