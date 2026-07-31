#include <gtest/gtest.h>

#include "ErrorRepository.hpp"

TEST(
    ErrorRepositoryTest,
    AddError)
{
    rim::ErrorRepository repository;

    repository.Add(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    const auto& errors =
        repository.GetAll();

    ASSERT_EQ(
        errors.Size(),
        1U);

    EXPECT_EQ(
        errors[0].errorCode,
        1001U);

    EXPECT_EQ(
        errors[0].state,
        rim::ErrorState::kActive);

    EXPECT_EQ(
        errors[0].severity,
        rim::ErrorSeverity::kError);

}

TEST(
    ErrorRepositoryTest,
    RemoveError)
{
    rim::ErrorRepository repository;

    repository.Add(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    EXPECT_TRUE(
        repository.Remove(
            1001));

    EXPECT_TRUE(
        repository.GetAll().Empty());
}

TEST(
    ErrorRepositoryTest,
    SetErrorState)
{
    rim::ErrorRepository repository;

    repository.Add(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
        });

    ASSERT_TRUE(
        repository.SetState(
            1001,
            rim::ErrorState::kRecovered));

    const auto& errors =
        repository.GetAll();

    ASSERT_EQ(
        errors.Size(),
        1U);

    EXPECT_EQ(
        errors[0].state,
        rim::ErrorState::kRecovered);
}

TEST(
    ErrorRepositoryTest,
    RemoveUnknownError)
{
    rim::ErrorRepository repository;

    EXPECT_FALSE(
        repository.Remove(
            9999));
}

TEST(
    ErrorRepositoryTest,
    SetStateUnknownError)
{
    rim::ErrorRepository repository;

    EXPECT_FALSE(
        repository.SetState(
            9999,
            rim::ErrorState::kRecovered));
}

TEST(
    ErrorRepositoryTest,
    AddDuplicateError)
{
    rim::ErrorRepository repository;

    EXPECT_TRUE(
        repository.Add(
            {
                1001,
                rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
            }));

    EXPECT_FALSE(
        repository.Add(
            {
                1001,
                rim::ErrorState::kActive,
            rim::ErrorSeverity::kError
            }));

    ASSERT_EQ(
        repository.GetAll().Size(),
        1U);
}

TEST(
    ErrorRepositoryTest,
    SeverityStored)
{
    rim::ErrorRepository repository;

    repository.Add(
        {
            1001,
            rim::ErrorState::kActive,
            rim::ErrorSeverity::kFatal
        });

    const auto& errors =
        repository.GetAll();

    ASSERT_EQ(
        errors.Size(),
        1U);

    EXPECT_EQ(
        errors[0].severity,
        rim::ErrorSeverity::kFatal);
}

TEST(
    ErrorRepositoryTest,
    SeverityComparison)
{
    rim::ErrorInfo lhs
    {
        1001,
        rim::ErrorState::kActive,
        rim::ErrorSeverity::kWarning
    };

    rim::ErrorInfo rhs
    {
        1001,
        rim::ErrorState::kActive,
        rim::ErrorSeverity::kError
    };

    EXPECT_FALSE(
        lhs == rhs);
}
